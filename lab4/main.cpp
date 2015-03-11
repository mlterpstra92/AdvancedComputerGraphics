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

CGprogram vertexProgram = NULL;
CGprogram fragmentProgram = NULL;
CGprogram normalizeFP = NULL;

CGparameter modelView = NULL;
CGparameter modelViewProj = NULL;
CGparameter wsize = NULL;
CGparameter near_vp = NULL;
CGparameter top = NULL;
CGparameter bottom = NULL;

CGparameter unproj_scale = NULL;
CGparameter unproj_offset = NULL;
CGparameter near_fp = NULL;
CGparameter screenSpaceRadius_v = NULL;
CGparameter screenSpaceRadius_f = NULL;
CGparameter zb_scale = NULL;
CGparameter zb_offset = NULL;

CGparameter epsilon = NULL;

GLuint fbo = 0;
GLuint depthbuffer = 0;
GLuint color_tex;
GLuint normal_tex;

float unproj_scale_x = 0.0f;
float unproj_scale_y = 0.0f;
float unproj_offset_x = 0.0f;
float unproj_offset_y = 0.0f;

int w_width=512, w_height=512;

struct Surfel
{
    float pos[3];
    float color[3]; // Changed from 4 to save space
    float uvec[3];
    float vvec[3];
};

int numpoints;
Surfel *pts;

void read_points(const char *fname)
{
    FILE *f = fopen(fname, "rb");
    assert(f);
    fread(&numpoints, sizeof(int), 1, f);
    printf("Reading %i points from %s ...\n", numpoints, fname);
    pts = new Surfel[numpoints];
    fread(pts, sizeof(Surfel)*numpoints, 1, f);    
    fclose(f);
}

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

float xmin = 0.0f, xmax = 0.0f;
float ymin = 0.0f, ymax = 0.0f;
void myGluPerspective(float fovyInDegrees, float aspectRatio, float znear, float zfar)
{
    ymax = znear * tanf(fovyInDegrees * M_PI / 360.0);
    xmax = ymax * aspectRatio;
    
    xmin = -xmax;
    ymin = -ymax;
    
    glFrustum(xmin, xmax, ymin, ymax, znear, zfar);
}

CGparameter getNamedParameter( CGprogram program, const char * name )
{
    CGparameter result = cgGetNamedParameter(program, name);
    if (!result)
    {
      printf("Parameter %s was not defined in the shader\n", name);
    }
    
    return result;
}

void display() 
{
    int curTime = glutGet(GLUT_ELAPSED_TIME);    

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float znear = 0.1f;
    float zfar = 100.0f;
    myGluPerspective(30.0f, (float)w_width/w_height, znear, zfar);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(4, 0, -3, 0, 0, 0, 0, 1, 0);
    
    glRotatef((float)curTime/20.0, 0, 1, 0);
    
    cgGLEnableProfile(vertexProfile);
    cgGLBindProgram(vertexProgram);
    
    cgGLEnableProfile(fragmentProfile);
    cgGLBindProgram(fragmentProgram);
    
    cgGLSetStateMatrixParameter(
      modelViewProj,
      CG_GL_MODELVIEW_PROJECTION_MATRIX,
      CG_GL_MATRIX_IDENTITY);
    
    cgGLSetStateMatrixParameter(
      modelView,
      CG_GL_MODELVIEW_MATRIX,
      CG_GL_MATRIX_IDENTITY);
    
    cgSetParameter2f(wsize, (float)w_width, (float)w_height);
    cgSetParameter1f(near_vp, znear);
    cgSetParameter1f(top, float(ymax));
    cgSetParameter1f(bottom, float(-ymax));
    
    cgSetParameter2f(unproj_scale, float(xmax - xmin) / float(w_width), float(ymax - ymin) / float(w_height));
    cgSetParameter2f(unproj_offset, float(xmax - xmin) / 2.0f, float(ymax - ymin) / 2.0f);
    cgSetParameter1f(near_fp, znear);
    cgSetParameter1f(zb_scale, (zfar*znear)/(zfar-znear));
    cgSetParameter1f(zb_offset, zfar/(zfar-znear));
    cgSetParameter1f(screenSpaceRadius_v, 3.0f);
    cgSetParameter1f(screenSpaceRadius_f, 3.0f);
        
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    
    glClientActiveTexture(GL_TEXTURE0);
    glTexCoordPointer(3, GL_FLOAT, sizeof(Surfel), &pts[0].color);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glClientActiveTexture(GL_TEXTURE1);
    glTexCoordPointer(3, GL_FLOAT, sizeof(Surfel), &pts[0].uvec);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    
    glClientActiveTexture(GL_TEXTURE2);
    glTexCoordPointer(3, GL_FLOAT, sizeof(Surfel), &pts[0].vvec);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(Surfel), &pts[0].pos);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    
    GLenum bufs[2] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT};
    glDrawBuffers(2, bufs);
    
    glClearColor(0, 0, 0, 1e-6);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Visibility pass
    cgSetParameter1f(epsilon, -0.1f);
    glColorMask(GL_FALSE,GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_TRUE);
    glDrawArrays(GL_POINTS, 0, numpoints);
    
    // Blending pass
    cgSetParameter1f(epsilon, 0.0f);
    glColorMask(GL_TRUE,GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDrawArrays(GL_POINTS, 0, numpoints);
    glDepthMask(GL_TRUE);
    
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    glClientActiveTexture(GL_TEXTURE0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE2);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    
    
    //////////////////////////////////////// Begin normalization pass ////////////////////////////////////////
    
    //For the normalization pass we do not need a vertex shader, so only set up the fragment shader.
    cgGLDisableProfile(vertexProfile);
    cgGLBindProgram(normalizeFP);

    cgGLSetTextureParameter(cgGetNamedParameter(normalizeFP,"colorin"), color_tex);
    cgGLEnableTextureParameter(cgGetNamedParameter(normalizeFP,"colorin"));
    
    cgGLSetTextureParameter(cgGetNamedParameter(normalizeFP,"normalin"), normal_tex);
    cgGLEnableTextureParameter(cgGetNamedParameter(normalizeFP,"normalin"));
    
    cgSetParameter2f(cgGetNamedParameter(normalizeFP,"unproj_scale"), float(xmax - xmin) / float(w_width), float(ymax - ymin) / float(w_height));
    cgSetParameter2f(cgGetNamedParameter(normalizeFP,"unproj_offset"), float(xmax - xmin) / 2.0f, float(ymax - ymin) / 2.0f);
    cgSetParameter1f(cgGetNamedParameter(normalizeFP,"near"), znear);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    // Example of drawing full-screen quad from opengl.org
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
    glLoadIdentity ();
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glLoadIdentity ();
    glBegin (GL_QUADS);
    glVertex3i (-1, -1, -1);
    glVertex3i (1, -1, -1);
    glVertex3i (1, 1, -1);
    glVertex3i (-1, 1, -1);
    glEnd ();
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ();
    
    cgGLDisableProfile(fragmentProfile);
    
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
    vertexProgram = loadCgProgram(vertexProfile, "VertexProgram.cg");
    fragmentProgram = loadCgProgram(fragmentProfile, "FragmentProgram.cg");
    normalizeFP = loadCgProgram(fragmentProfile, "normalizeFP.cg");
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
    
    glDeleteFramebuffersEXT(1, &fbo);
    glDeleteRenderbuffers(1, &depthbuffer);
    glDeleteTextures(1, &color_tex);
    glDeleteTextures(1, &normal_tex);
    
    glGenTextures(1, &color_tex);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, w_width, w_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenTextures(1, &normal_tex);
    glBindTexture(GL_TEXTURE_2D, normal_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, w_width, w_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenRenderbuffersEXT(1, &depthbuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT16, w_width, w_height);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
    
    glGenFramebuffersEXT(1, &fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_tex, 0);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER_EXT, depthbuffer);

    assert(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

int main(int argc, char *argv[])
{
    assert(argc>=2 && argv[1]);
    
    read_points(argv[1]);
    
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
    
    modelView = getNamedParameter(vertexProgram, "modelView");
    modelViewProj = getNamedParameter(vertexProgram, "modelViewProj");
    wsize = getNamedParameter(vertexProgram, "wsize");
    near_vp = getNamedParameter(vertexProgram, "near");
    top = getNamedParameter(vertexProgram, "top");
    bottom = getNamedParameter(vertexProgram, "bottom");
    screenSpaceRadius_v = getNamedParameter(vertexProgram, "screenSpaceRadius");
    
    unproj_scale = getNamedParameter(fragmentProgram, "unproj_scale");
    unproj_offset = getNamedParameter(fragmentProgram, "unproj_offset");
    near_fp = getNamedParameter(fragmentProgram, "near");
    screenSpaceRadius_f = getNamedParameter(fragmentProgram, "screenSpaceRadius");
    
    zb_scale = getNamedParameter(fragmentProgram, "zb_scale");
    zb_offset = getNamedParameter(fragmentProgram, "zb_offset");
    
    epsilon = getNamedParameter(fragmentProgram, "epsilon");
    
    glutMainLoop();
    
    return 0;
}


