#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

int w_width=512, w_height=512;
CGcontext context = NULL;
CGprogram myVertexProgram = NULL;
CGprofile vertexProfile = CG_PROFILE_VP40;
static void display()
{
    printf("Drawing callback\n");
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
    cgGLEnableProfile(vertexProfile);
    cgGLBindProgram(myVertexProgram);
    glutWireSphere(1.0, 10, 10);

    cgGLDisableProfile(vertexProfile);
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

int main (int argc, char *argv[])
{
    //Initialize GLUT
    initializeGlut(&argc, argv);

    //Compile shader from file
    context = cgCreateContext();
    myVertexProgram = cgCreateProgramFromFile(context, CG_SOURCE, "colorful_v.cg", vertexProfile, "main", NULL);
    if(!myVertexProgram)
    {
        printf("Shader failed, could not load vertex program\n");
        printf("%s\n", cgGetLastListing(context));
        return -1;
    }

    //Enter main loop
    glutMainLoop();
        
    // never gets here.
    return 0;
}