#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>

#include <Cg/cg.h>
#include <Cg/cgGL.h>

CGcontext   context;
CGprofile   vertexProfile, fragmentProfile;
CGprogram   vertexProgram, fragmentProgram, normalizeProgram;
float       near_val, bottom_val, top_val; 
float x_scale, y_scale;
float x_offset, y_offset;
float zb_scale_val, zb_offset_val;
CGparameter modelView = NULL;
CGparameter modelViewProj = NULL;
CGparameter wsize = NULL;
CGparameter near = NULL;
CGparameter near_f = NULL;
CGparameter top = NULL;
CGparameter bottom = NULL;
CGparameter epsilon = NULL;
CGparameter unproj_scale = NULL;
CGparameter unproj_offset = NULL;
CGparameter zb_scale = NULL;
CGparameter zb_offset = NULL;
CGparameter texture_input = NULL;

int w_width=512, w_height=512;

GLuint fbo         = 0;
GLuint depthbuffer = 0;
GLuint color_tex   = 0;

struct Surfel
{
    float pos[3];
    float color[4];
    float uvec[3];
    float vvec[3];
};

#include "points2.h"

void keyboard(unsigned char key, int x, int y)
{
    switch(key) {
        case 'q':
        case 'Q':
        case 27:
            exit(0);
            break;
    }
}

void glhPerspectivef2(float fovyInDegrees, float aspectRatio, float znear, float zfar)
{
    top_val = znear * tanf(fovyInDegrees * M_PI / 360.0);
    float right = top_val * aspectRatio;
    float left = -right;
    bottom_val = -top_val;
    near_val = znear;
    // Based on: https://www.cs.mtsu.edu/~jhankins/files/4250/notes/WinToView/WinToViewMap.html
    float xvmax = right;
    float xvmin = left;
    float xwmax = glutGet(GLUT_WINDOW_WIDTH);
    float xwmin = 0;

    float yvmax = top_val;
    float yvmin = bottom_val;
    float ywmax = glutGet(GLUT_WINDOW_HEIGHT);
    float ywmin = 0;

    x_scale = (xvmax - xvmin) / (xwmax - xwmin);
    y_scale = (yvmax - yvmin) / (ywmax - ywmin);
    x_offset = (xvmax - xvmin) / 2;
    y_offset = (yvmax - yvmin) / 2;

    zb_scale_val = (zfar * znear) / (zfar - znear);
    zb_offset_val = zfar / (zfar - znear);

    glFrustum(left, right, bottom_val, top_val, znear, zfar);
}

void handleCgError() 
{
    fprintf(stderr, "Cg error: %s\n", cgGetErrorString(cgGetError()));
}

void display() 
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glhPerspectivef2(30.0f, (float)w_width/w_height, 0.1, 100.0);   
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(4, 0, -3, 0, 0, 0, 0, 1, 0);

    //Enable this so OpenGL listens to PSIZE returned by CG
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

    //Pass relevant parameters to CG
    cgGLSetStateMatrixParameter(modelView, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(modelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetParameter2f(wsize, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    cgGLSetParameter1f(near, near_val);
    cgGLSetParameter1f(near_f, near_val);
    cgGLSetParameter1f(top, top_val);
    cgGLSetParameter1f(bottom, bottom_val);
    cgGLSetParameter1f(epsilon, -10e-3);
    cgGLSetParameter2f(unproj_scale, x_scale, y_scale);
    cgGLSetParameter2f(unproj_offset, x_offset, y_offset);
    cgGLSetParameter1f(zb_scale, zb_scale_val);
    cgGLSetParameter1f(zb_offset, zb_offset_val);

    //Select coordinates, color and vectors(?) from points
    glClientActiveTexture(GL_TEXTURE0);
    glTexCoordPointer(4, GL_FLOAT, sizeof(Surfel), &pts[0].color);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glClientActiveTexture(GL_TEXTURE1);
    glTexCoordPointer(3, GL_FLOAT, sizeof(Surfel), &pts[0].uvec);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glClientActiveTexture(GL_TEXTURE2);
    glTexCoordPointer(3, GL_FLOAT, sizeof(Surfel), &pts[0].vvec);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(Surfel), &pts[0].pos);
    glEnableClientState(GL_VERTEX_ARRAY);

    //Bind and load the shader
    cgGLEnableProfile(vertexProfile);
    cgGLBindProgram(vertexProgram);
    cgGLEnableProfile(fragmentProfile);
    cgGLBindProgram(fragmentProgram);

    // bind to FBO and clear it
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Visibility splatting pass
    // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    // glDepthMask(GL_TRUE);
    // glDrawArrays(GL_POINTS, 0, numpoints);

    //Enable color writing and alpha blending
    cgGLSetParameter1f(epsilon, 0);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
    glDrawArrays(GL_POINTS, 0, numpoints);

    // Draw the texture to the screen
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    cgGLBindProgram(normalizeProgram);
    cgGLUnbindProgram(vertexProfile);
    cgGLDisableProfile(vertexProfile);
    glClearColor(0, 0, 0, 1e-6);

    cgGLSetTextureParameter(texture_input, color_tex);
    cgGLEnableTextureParameter(texture_input);

    float ratio = (float)w_width / w_height;
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(-1.0 * ratio, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex2f(1.0 * ratio, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex2f(1.0 * ratio, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex2f(-1.0 * ratio, 1.0);
    glEnd();

    // Disable after drawing
    glClientActiveTexture(GL_TEXTURE0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE2);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    cgGLDisableProfile(fragmentProfile);
    glDepthMask(GL_TRUE);
    glutSwapBuffers();
}


/* Choose profiles, set optimal options */
void chooseCgProfiles()
{
    vertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
    cgGLSetOptimalOptions(vertexProfile);
    fragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
    cgGLSetOptimalOptions(fragmentProfile);
    printf("vertex profile:   %s\n",
         cgGetProfileString(vertexProfile));
    printf("fragment profile: %s\n",
         cgGetProfileString(fragmentProfile));
}

/* Load Cg program from disk */
CGprogram loadCgProgram(CGprofile profile, const char *filename)
{
    CGprogram program;
    assert(cgIsContext(context));

    fprintf(stderr, "Cg program %s creating.\n", filename);
    program = cgCreateProgramFromFile(context, CG_SOURCE,
            filename, profile, NULL, NULL);
    
    if(!cgIsProgramCompiled(program)) {
        printf("%s\n",cgGetLastListing(context));
        exit(1);
    }
    
    fprintf(stderr, "Cg program %s loading.\n", filename);
    cgGLLoadProgram(program);
    
    return program;
}


void loadCgPrograms()
{
    vertexProgram = loadCgProgram(vertexProfile, "pointscalingvertex.cg");
    modelView = cgGetNamedParameter(vertexProgram, "modelView");
    modelViewProj = cgGetNamedParameter(vertexProgram, "modelViewProj");
    wsize = cgGetNamedParameter(vertexProgram, "wsize");
    top = cgGetNamedParameter(vertexProgram, "top");
    bottom = cgGetNamedParameter(vertexProgram, "bottom");
    near = cgGetNamedParameter(vertexProgram, "near");

    fragmentProgram = loadCgProgram(fragmentProfile, "perspectivelycorrect.cg");
    epsilon = cgGetNamedParameter(fragmentProgram, "epsilon");
    near_f = cgGetNamedParameter(fragmentProgram, "near");
    unproj_scale = cgGetNamedParameter(fragmentProgram, "unproj_scale");
    unproj_offset = cgGetNamedParameter(fragmentProgram, "unproj_offset");
    zb_scale = cgGetNamedParameter(fragmentProgram, "zb_scale");
    zb_offset = cgGetNamedParameter(fragmentProgram, "zb_offset");

    normalizeProgram = loadCgProgram(fragmentProfile, "normalize.cg");
    texture_input = cgGetNamedParameter(normalizeProgram, "input");
}

void idle()
{
    glutPostRedisplay();
}

void reshape(int width, int height)
{
    w_width = width;
    w_height = height;

    // Delete previous items. Initialize to zero so it does not do anything the first time
    glDeleteFramebuffersEXT(1, &fbo);
    glDeleteRenderbuffersEXT(1, &depthbuffer);
    glDeleteTextures(1, &color_tex);

    //Generate color texture with 16 bit floating point values
    glGenTextures(1, &color_tex);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGBA16F_ARB,  w_width, w_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Generate depth buffer render buffer
    glGenRenderbuffersEXT(1, &depthbuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,  GL_DEPTH_COMPONENT, w_width, w_height);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

    //Generate the actual framebuffer and set color texture and depthbuffer renderbuffer
    glGenFramebuffersEXT(1, &fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER_EXT, depthbuffer);
    //Make sure the FBO is fully defined and rebind the default renderbuffer
    assert(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glViewport(0, 0, w_width, w_height);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(w_width, w_height);
    glutCreateWindow("Point based rendering in Cg");
    
    glewInit();
    
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    
    cgSetErrorCallback(handleCgError);
    context = cgCreateContext();
    chooseCgProfiles();
    loadCgPrograms();
    
    glutMainLoop();
    
    return 0;
}


