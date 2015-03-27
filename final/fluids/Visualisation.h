#ifndef _H_VISUALISATION_H_
#define _H_VISUALISATION_H_

#include <GL/glew.h>
#include <GL/glut.h>
#include "Particle.h"
#include "Wall.h"
#include "Simulation.h"

#include <vector>

class Visualisation
{
public:
	Visualisation();
	void renderParticles();
	void initialise();
	void setupFBOs(int w_width, int w_height);

	float rotationX;
	float rotationY;
	float zoomFactor;
	int oldX, oldY;
	bool isRotating;
	int smoothSteps;

	float baseX;
	float baseY;
	float baseZ;
	GLuint fbo, smooth_fbo, depth_tex, color_tex, alternate_depth_tex;
};

#endif
