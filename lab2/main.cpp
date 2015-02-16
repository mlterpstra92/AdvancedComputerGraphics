#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include <Cg/cg.h>
#include <Cg/cgGL.h>

CGcontext	context;
CGprofile	vertexProfile, fragmentProfile;

int w_width=512, w_height=512;

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
    gluPerspective(30.0f, (float)w_width/w_height, 0.1, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(4, 0, -3, 0, 0, 0, 0, 1, 0);

    //Select coordinates, color and vectors(?) from points
    glClientActiveTexture(GL_TEXTURE0);
    glTexCoordPointer(3, GL_FLOAT, sizeof(Surfel), &pts[0].uvec);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glClientActiveTexture(GL_TEXTURE1);
    glTexCoordPointer(3, GL_FLOAT, sizeof(Surfel), &pts[0].vvec);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(Surfel), &pts[0].pos);
    glColorPointer(3, GL_FLOAT, sizeof(Surfel), &pts[0].color);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    //Actually draw the thing
    glDrawArrays(GL_POINTS, 0, numpoints);

    //Disable after drawing
    glClientActiveTexture(GL_TEXTURE0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    
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
    /* Load all Cg programs that are used with loadCgProgram */
}

void idle()
{
    glutPostRedisplay();
}

void reshape(int width, int height)
{
    w_width = width;
    w_height = height;
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


