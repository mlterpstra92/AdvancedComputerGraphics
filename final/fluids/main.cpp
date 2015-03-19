#include <iostream>
#include <vector>

#include <math.h>
#include <time.h>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "OpenGL.h"
#include <GL/glui.h>

#include "UI.h"
#include "Wall.h"
#include "Simulation.h"
#include "Visualisation.h"
#include "main.h"
#include "Shader.h"

// view parameter for glFrustum
float view_left = -1.0;
float view_right = 1.0;
float view_bottom = -1.0;
float view_top = 1.0;

float view_near = 1.0;
float view_far = 50.0;

float fovy = 60.0f;

// window parameters
unsigned int w_width = 1280;
unsigned int w_height = 720;

const float eye[] = {0.0, 0.0, 1.0};
const float origin[] = { 0.0, 0.0, 0.0 };
const float up[] =     { 0.0, 1.0, 0.0 };

int mainWindow;
Visualisation vis;
Simulation sim;
Shader shader;
int frames = 0;

void perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
    const GLdouble pi = 3.1415926535897932384626433832795;
    GLdouble fW, fH;

    fH = tan( fovY / 360 * pi ) * zNear;
    fW = fH * aspect;
    
    view_left = -fW;
    view_right = fW;
    view_bottom = -fH;
    view_top = fH;

    glFrustum( view_left, view_right, view_bottom, view_top, zNear, zFar );
}

void surfaceDepthPass()
{
    glColorMask(GL_FALSE,GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    vis.renderParticles();
    glEnable(GL_DEPTH_TEST);
    glColorMask(GL_TRUE,GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_FALSE);
}

void display(void)
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    perspectiveGL(vis.zoomFactor * fovy, (float)w_width/w_height, view_near, view_far);

    // Set camera position
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2],
              origin[0], origin[1], origin[2],
              up[0], up[1], up[2]);

    glTranslatef(vis.baseX, vis.baseY, vis.baseZ);
    glRotatef(vis.rotationX, 1.0, 0.0, 0.0);
    glRotatef(vis.rotationY, 0.0, 1.0, 0.0);

    // Bind variables to shaders
    cgGLSetStateMatrixParameter(
        cgGetNamedParameter(shader.vertexProgram, "modelViewProj"), 
        CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(
        cgGetNamedParameter(shader.vertexProgram, "modelView"), 
        CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetParameter2f(cgGetNamedParameter(shader.vertexProgram, "wsize"), w_width, w_height);
    cgGLSetParameter1f(cgGetNamedParameter(shader.vertexProgram, "near"), view_near);
    cgGLSetParameter1f(cgGetNamedParameter(shader.vertexProgram, "top"), view_top);
    cgGLSetParameter1f(cgGetNamedParameter(shader.vertexProgram, "bottom"), view_bottom);
    float radius;
    if (sim.fluids[0]->particles.size() != 0)
    {
        radius = sim.fluids[0]->particles[0].renderRadius();
    }
    else
    {
        radius = 0;
    }
    cgGLSetParameter1f(cgGetNamedParameter(shader.vertexProgram, "radius"), radius);
    
    // Bind shaders
    cgGLEnableProfile(shader.vertexProfile);
    cgGLBindProgram(shader.vertexProgram);    
    cgGLEnableProfile(shader.fragmentProfile);
    cgGLBindProgram(shader.fragmentProgram);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    // Bind fbo
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, vis.fbo);
    GLenum bufs[2] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT};
    glDrawBuffers(2, bufs);
    

    surfaceDepthPass();

    // Bind to screen rendering
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    vis.renderParticles();
    cgGLDisableProfile(shader.vertexProfile);
    cgGLDisableProfile(shader.fragmentProfile);


    // draw texture to full screen quad
    // cgGLEnableProfile(shader.fragmentProfile);
    // cgGLBindProgram(shader.fragmentProgram);


    // glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    // glMatrixMode (GL_MODELVIEW);
    // glPushMatrix ();
    // glLoadIdentity ();
    // glMatrixMode (GL_PROJECTION);
    // glPushMatrix ();
    // glLoadIdentity ();
    // glBegin (GL_QUADS);
    // glVertex3i (-1, -1, -1);
    // glVertex3i (1, -1, -1);
    // glVertex3i (1, 1, -1);
    // glVertex3i (-1, 1, -1);
    // glEnd ();
    // glPopMatrix ();
    // glMatrixMode (GL_MODELVIEW);
    // glPopMatrix ();
    frames++;
    glFlush();
    glutSwapBuffers();
}

void reshape(int width, int height)
{
    w_width = width;
    w_height = height;
    glViewport(0, 0, w_width, w_height);
    perspectiveGL(vis.zoomFactor * fovy, (float)w_width/w_height, view_near, view_far);
    vis.setupFBOs(w_width, w_height);
    glutPostRedisplay();
}

void idle()
{
    static long framebase = 0;

    glutSetWindow(mainWindow);

    long current = glutGet(GLUT_ELAPSED_TIME);

    if (current - framebase > 1000)
    {
        float fps = frames*1000.0/(current-framebase);
        framebase = current;
        frames = 0;

        char windowtitle[64];
        sprintf(windowtitle, "SPH - %0.2f FPS", fps);
        glutSetWindowTitle(windowtitle);
    }

    sim.executeTimestep();

    glutPostRedisplay();
}

void OnMouseDown(int button, int state, int x, int y)
{
    vis.isRotating = false;
    if (button == GLUT_LEFT_BUTTON)
    {
        vis.oldX = x;
        vis.oldY = y;
        vis.isRotating = true;
    }
    else if (button == 3)
    {
        if (vis.zoomFactor > 0.03)
            vis.zoomFactor -= 0.03;
        reshape(w_width, w_height);
    }
    else if (button == 4)
    {
        if (vis.zoomFactor < 2.0)
            vis.zoomFactor += 0.03;
        reshape(w_width, w_height);
    }

    glutSetWindow(mainWindow);
    glutPostRedisplay();
}

void OnMouseMove(int x, int y)
{
    if (vis.isRotating)
    {
        vis.rotationY += x - vis.oldX;
        vis.rotationX += y - vis.oldY;
    }

    vis.oldX = x;
    vis.oldY = y;

    glutSetWindow(mainWindow);
    glutPostRedisplay();
}

void OnKeyPress(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'w':
        case 'W':
            vis.baseZ += 0.05;
        break;

        case 's':
        case 'S':
            vis.baseZ -= 0.05;
        break;

        case 'a':
        case 'A':
            vis.baseX += 0.02;
        break;

        case 'd':
        case 'D':
            vis.baseX -= 0.02;
        break;

        case 'r':
        case 'R':
            vis.rotationX = 0.0;
            vis.rotationY = 0.0;
        break;

        case 'z':
        case 'Z':
            vis.zoomFactor += 0.03;
        break;

        case 'x':
        case 'X':
            vis.zoomFactor -= 0.03;
        break;
	
        case 'q':
        case 'Q':
            exit(0);
        break;
    }

    glutSetWindow(mainWindow);
    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w_width, w_height);
    mainWindow = glutCreateWindow("SPH");
    GLUI_Master.set_glutReshapeFunc(reshape);

    glewInit();
    sim.initialise();
    vis.initialise();

    glutDisplayFunc(display);
    GLUI_Master.set_glutIdleFunc(idle);
    glutMouseFunc(OnMouseDown);
    glutMotionFunc(OnMouseMove);
    glutKeyboardFunc(OnKeyPress);
    UI theUI;
    theUI.initialise(mainWindow);
    glutMainLoop();

    return 0;
}