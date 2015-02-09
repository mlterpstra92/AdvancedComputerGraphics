#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <iostream>
#include <vector>
#include "glm.h"
#include "glm/glm.hpp"

using namespace std;

int w_width=512, w_height=512;
CGcontext context = NULL;
CGprogram myVertexProgram = NULL;
CGprofile vertexProfile = CG_PROFILE_VP40;
CGparameter modelViewProj = NULL;
CGparameter modelView = NULL;
CGparameter modelViewIT = NULL;
CGparameter shininess = NULL;
CGparameter lightdir = NULL;
CGprofile fragmentProfile = CG_PROFILE_FP40;
CGprogram myFragmentProgram = NULL;
float shininess_val = 100.0;
float lightdir_val_x = 0.5;
float lightdir_val_y = 0.5;
float lightdir_val_z = 1.0;
int last_called = 0;
int frames = 0;
float bounce_y = 0.02;
GLMmodel* teapot = NULL;

vector<glm::vec3> teapot_vertices;
vector<glm::vec3> teapot_normals;
vector<glm::vec3> teapot_faces;


void calcFPS(int theTimeInterval = 1000, std::string theWindowTitle = "NONE")
{
    // Static values which only get initialised the first time the function runs
    static int t0Value = glutGet(GLUT_ELAPSED_TIME); // Set the initial time to now
    static int fpsFrameCount = 0; // Set the initial FPS frame count to 0
    static double fps = 0.0; // Set the initial FPS value to 0.0
    // Get the current time in seconds since the program started (non-static, so executed every time)
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    // Calculate and display the FPS every specified time interval
    if ((currentTime - t0Value) > theTimeInterval)
    {
        // Calculate the FPS as the number of frames divided by the interval in seconds
        fps = ((double)(fpsFrameCount * 1000.0)) / (currentTime - t0Value);
        char buf[6];
        snprintf(buf, 6, "%5.2f", fps);
        std::string fpsStr = buf;
        // Append the FPS value to the window title details
        theWindowTitle += " | FPS: " + fpsStr;
        // Convert the new window title to a c_str and set it
        const char* pszConstString = theWindowTitle.c_str();
        glutSetWindowTitle(pszConstString);
        // Reset the FPS frame counter and set the initial time to be now
        fpsFrameCount = 0;
        t0Value = glutGet(GLUT_ELAPSED_TIME);
    }
    else // FPS calculation time interval hasn't elapsed yet? Simply increment the FPS frame counter
        fpsFrameCount++;
}

static void display()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cgGLSetStateMatrixParameter(modelView, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(modelViewIT, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
    cgGLSetStateMatrixParameter(modelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetParameter1f(shininess, shininess_val);
    cgGLSetParameter3f(lightdir, lightdir_val_x, lightdir_val_y, lightdir_val_z);
    cgGLEnableProfile(vertexProfile);
    cgGLBindProgram(myVertexProgram);
    cgGLEnableProfile(fragmentProfile);
    cgGLBindProgram(myFragmentProgram);

    if (teapot != NULL)
    {
        glmDrawVBO(teapot);
    }
    else
    {
        glutSolidSphere(3.0, 10, 10);
    }

    cgGLDisableProfile(vertexProfile);
    cgGLDisableProfile(fragmentProfile);
    calcFPS(1000, "Advanced Computer Graphics | Assignment 1");
    glutSwapBuffers();
}

static void initializeGlut(int *argc, char *argv[])
{
    glutInit (argc, argv);

    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(w_width, w_height);
    glutCreateWindow(argv[0]);

    glewInit();

    glutDisplayFunc(display);

    glEnable(GL_DEPTH_TEST);


    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    gluPerspective (
        90.0, // fov
        1.0,  // aspect ratio
        .05,  // near
        40);  // far

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        0, 0, 5, // eye
        0, 0, 0, // lookat point
        0, 1, 0);// up
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case '+':
            shininess_val += 10;
            break;
        case '-':
            if (shininess_val < 0)
            {
                shininess_val = 0;
            }
            else
            {
                shininess_val -= 10;
            }
            break;
        case 'X':
            lightdir_val_x += 0.1;
            break;
        case 'x':
            lightdir_val_x -= 0.1;
            break;
        case 'Y':
            lightdir_val_y += 0.1;
            break;
        case 'y':
            lightdir_val_y -= 0.1;
            break;
        case 'Z':
            lightdir_val_z += 0.1;
            break;
        case 'z':
            lightdir_val_z -= 0.1;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void idle()
{
    int current_time = glutGet(GLUT_ELAPSED_TIME);
    int elapsed_time = current_time - last_called;
    last_called = current_time;
    frames += 1;

    float angle = elapsed_time * (360.0 / 3000.0);
    glRotatef(angle, 0.0, 1.0, 0.0);

    if (frames % 40 == 0)
    {   
        frames = 0;
        bounce_y *= -1.0;
    }
    glTranslatef(0.0, bounce_y, 0.0);

    glutPostRedisplay();
}

int main (int argc, char *argv[])
{
    //Initialize GLUT
    initializeGlut(&argc, argv);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    char filename[100];
    snprintf(filename, sizeof(char) * sizeof(filename), "%s.obj", "teapot");
    teapot = glmReadOBJ(filename);
    glmUnitize(teapot);
    glmScale(teapot, 3);
    glmFacetNormals(teapot);
    glmVertexNormals(teapot, 90);
    glmInitVBO(teapot);
    //Compile shader from file
    context = cgCreateContext();
    myVertexProgram = cgCreateProgramFromFile(context, CG_SOURCE, "colorful_v.cg", vertexProfile, "main", NULL);
    if(!myVertexProgram)
    {
        printf("Shader failed, could not load vertex program\n");
        printf("%s\n", cgGetLastListing(context));
        return -1;
    }
    cgGLLoadProgram(myVertexProgram);
    myFragmentProgram = cgCreateProgramFromFile(context, CG_SOURCE, "colorful_f.cg", fragmentProfile, "main", NULL);
    if(!myFragmentProgram)
    {
        printf("Couldn’t load fragment program.\n");
        printf("%s\n",cgGetLastListing(context));
        return 0;
    }
    cgGLLoadProgram(myFragmentProgram);

    modelViewProj = cgGetNamedParameter(myVertexProgram, "modelViewProj");
    modelView = cgGetNamedParameter(myVertexProgram, "modelView");
    modelViewIT = cgGetNamedParameter(myVertexProgram, "modelViewIT");
    shininess = cgGetNamedParameter(myFragmentProgram, "shininess");
    lightdir = cgGetNamedParameter(myFragmentProgram, "lightdir");


    if (!(modelViewProj && modelView && modelViewIT && shininess && lightdir))
    {
        printf("Parameter modelViewProj, modelView or modelViewIT was not defined in the shader\n");
        return -2;
    }

    //Enter main loop
    glutMainLoop();
        
    // never gets here.
    return 0;
}