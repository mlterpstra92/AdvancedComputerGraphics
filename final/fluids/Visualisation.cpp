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
        cgGLSetStateMatrixParameter(
            cgGetNamedParameter(shader.vertexProgram, "modelViewProj"), 
            CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
        cgGLEnableProfile(shader.vertexProfile);
        cgGLBindProgram(shader.vertexProgram);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_DOUBLE, sizeof(Particle), sim.fluids[0]->particles[0].position); // position

        glDrawArrays(GL_POINTS, 0, sim.fluids[0]->particles.size());

        if (glGetError() != 0)
                std::cout << gluErrorString(glGetError()) << '\n';

        glDisableClientState(GL_VERTEX_ARRAY);
        cgGLDisableProfile(shader.vertexProfile);
    }
}

void Visualisation::initialise()
{
    shader.context = cgCreateContext();
    shader.chooseCgProfiles();
    shader.loadCgPrograms();
}
