#include <GL/glew.h>
#include <GL/glut.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void init();
void display();

int const fbo_width = 512;
int const fbo_height = 512;

GLuint fb, color, depth;

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

    glutCreateWindow("FBO test");
    glutDisplayFunc(display);
    glutIdleFunc(glutPostRedisplay);

    glewInit();

    init();
    glutMainLoop();

    return 0;
}

void CHECK_FRAMEBUFFER_STATUS()
{                                                         
    GLenum status;
    status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER); 
    switch(status) {
    case GL_FRAMEBUFFER_COMPLETE:
        break;

    case GL_FRAMEBUFFER_UNSUPPORTED:
    /* choose different formats */
        break;

    default:
        /* programming error; will fail on all hardware */
        fputs("Framebuffer Error\n", stderr);
        exit(-1);
    }
}

float const light_dir[]={1,1,1,0};
float const light_color[]={1,0.95,0.9,1};

void init()
{
    glGenFramebuffers(1, &fb);
    glGenTextures(1, &color);
    glGenRenderbuffers(1, &depth);

    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glBindTexture(GL_TEXTURE_2D, color);
    glTexImage2D(   GL_TEXTURE_2D, 
            0, 
            GL_RGBA, 
            fbo_width, fbo_height,
            0, 
            GL_RGBA, 
            GL_UNSIGNED_BYTE, 
            NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fbo_width, fbo_height);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

    CHECK_FRAMEBUFFER_STATUS();
}

void prepare()
{
    // static float a=0, b=0, c=0;

    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_TEXTURE_2D);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glViewport(0,0, fbo_width, fbo_height);

    glClearColor(1,1,1,0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glLightfv(GL_LIGHT0, GL_POSITION, light_dir);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);

    glTranslatef(0,0,-5);

    glutSolidTeapot(0.75);
}

void final()
{
    static float a=0, b=0, c=0;

    const int win_width  = glutGet(GLUT_WINDOW_WIDTH);
    const int win_height = glutGet(GLUT_WINDOW_HEIGHT);
    const float aspect = (float)win_width/(float)win_height;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0,0, win_width, win_height);

    glClearColor(1.,1.,1.,0.);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, aspect, 1, 10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-5);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, color);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_LIGHTING);
    float ratio = (float)glutGet(GLUT_WINDOW_WIDTH)/glutGet(GLUT_WINDOW_HEIGHT);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(-1.0 * ratio, -1.0);
    glTexCoord2f(1.0, 0.0); glVertex2f(1.0 * ratio, -1.0);
    glTexCoord2f(1.0, 1.0); glVertex2f(1.0 * ratio, 1.0);
    glTexCoord2f(0.0, 1.0); glVertex2f(-1.0 * ratio, 1.0);
    glEnd();
}

void display()
{
    prepare();
    final();

    glutSwapBuffers();
}