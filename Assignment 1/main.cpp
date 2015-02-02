#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>

int w_width=512, w_height=512;

static void display()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
    glutWireSphere(1.0, 10, 10);

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
    initializeGlut(&argc, argv);
    glutMainLoop();
        
    // never gets here.
    return 0;
}
