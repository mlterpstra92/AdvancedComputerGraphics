#include "Particle.h"

#include "Vector.h"
#include "OpenGL.h"
#include "Fluid.h"

#include <cstdlib>
#include <math.h>
#include <iostream>


Particle::Particle(Fluid* fluid)
  : fluid(fluid)
{
	pressure = 0.0;
	density = 0.0;
}

void Particle::render(bool drawDynamicParticleSize)
{
	glColor3f(fluid->color[0], fluid->color[1], fluid->color[2]);
	glTexCoord3f(position[0], position[1], position[2]);
	glutSolidSphere(renderRadius(), 5, 5);
}

double const Particle::renderRadius()
{
	return pow((3.0 * fluid->mass) / (4.0 * M_PI * fluid->restDensity), 1.0/3.0);
}

double Particle::dynamicRenderRadius()
{
	return pow((3.0 * fluid->mass) / (4.0 * M_PI * density), 1.0/3.0);
}
