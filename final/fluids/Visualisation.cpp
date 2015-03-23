#include "Visualisation.h"
#include "Wall.h"
#include "Simulation.h"
#include "Shader.h"
#include "main.h"

#define GL_GLEXT_PROTOTYPES
#include "OpenGL.h"
#include <vector>

Visualisation::Visualisation()
{
    zoomFactor = 0.6;
    rotationX = 0;
    rotationY = 0;
    baseX = 0.0;
    baseY = 0.0;
    baseZ = -1.0;
    oldX = 0;
    oldY = 0;
    isRotating = false;
}

void Visualisation::renderParticles()
{   
    //support only 1 fluid this way
    if (sim.fluids[0]->particles.size() > 0)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, sizeof(Particle), sim.fluids[0]->particles[0].position); // position

        glDrawArrays(GL_POINTS, 0, sim.fluids[0]->particles.size());

        if (glGetError() != 0)
                std::cout << gluErrorString(glGetError()) << '\n';

        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

void Visualisation::setupFBOs(int w_width, int w_height)
{
    glDeleteFramebuffersEXT(1, &fbo);
    glDeleteRenderbuffers(1, &depth_tex);
    glDeleteTextures(1, &color_tex);
    
    // Color texture
    glGenTextures(1, &color_tex);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, w_width, w_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Depth texture
    glGenTextures(1, &depth_tex);
    glBindTexture(GL_TEXTURE_2D, depth_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w_width, w_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Attach textures to FBO
    glGenFramebuffersEXT(1, &fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depth_tex, 0);

    assert(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void Visualisation::initialise()
{
    shader.init();
}
