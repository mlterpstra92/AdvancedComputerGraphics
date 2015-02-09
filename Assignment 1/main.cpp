#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

int w_width=512, w_height=512;
CGcontext context = NULL;
CGprogram myVertexProgram = NULL;
CGprofile vertexProfile = CG_PROFILE_VP40;
CGparameter modelViewProj = NULL;
CGparameter modelView = NULL;
CGparameter modelViewIT = NULL;
CGprofile fragmentProfile = CG_PROFILE_FP40;
CGprogram myFragmentProgram = NULL;

static void display()
{

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cgGLSetStateMatrixParameter(modelView, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLSetStateMatrixParameter(modelViewIT, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
    cgGLSetStateMatrixParameter(modelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    cgGLEnableProfile(vertexProfile);
    cgGLBindProgram(myVertexProgram);
    cgGLEnableProfile(fragmentProfile);
    cgGLBindProgram(myFragmentProgram);
    glutSolidSphere(3.0, 10, 10);

    cgGLDisableProfile(vertexProfile);
    cgGLDisableProfile(fragmentProfile);
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

    if (!(modelViewProj && modelView && modelViewIT))
    {
        printf("Parameter modelViewProj, modelView or modelViewIT was not defined in the shader\n");
        return -2;
    }

    //Enter main loop
    glutMainLoop();
        
    // never gets here.
    return 0;
}