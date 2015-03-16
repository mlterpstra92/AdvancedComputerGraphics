#ifndef _H_FLUID_H_
#define _H_FLUID_H_

#include "Vector.h"
#include "Emitter.h"
#include "Particle.h"
#include "Solver.h"

#include <vector>

class Fluid
{
public:
	Fluid(double volume);

	Fluid* init();

	double volume;
	double supportRadius;

	std::vector<Particle> particles;

	Emitter* emitter;
	Solver solver;

	unsigned int maxParticles;
	unsigned int numParticles;

	// the color of this fluid
	Vector3d color;

	double kernelParticles;

	// opacity
	float opacity;

	// p0: density at rest in kg/m^3
	double restDensity;

	// m: mass in kg
	double mass;

	bool isGas;

	// b: buoyancy diffusion
	double buoyancy;

	// mu: viscosity in pascal per second
	double viscosity;

	// sigma: surface tension in Newton/meter
	double surfaceTension;

	// l: threshold value
	double threshold;

	// k: gas stiffness in Joule
	double gasStiffness;

	// cr: restitution for
	double restitution;
};

class Water : public Fluid
{
public:
	Water(double volume);
};

class Steam : public Fluid
{
public:
	Steam(double volume);
};

class Mucus : public Fluid
{
public:
	Mucus(double volume);
};

#endif
