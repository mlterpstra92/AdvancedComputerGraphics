#ifndef _H_VISUALISATION_H_
#define _H_VISUALISATION_H_

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

	float rotationX;
	float rotationY;
	float zoomFactor;
	int oldX, oldY;
	bool isRotating;

	float baseX;
	float baseY;
	float baseZ;
};

#endif
