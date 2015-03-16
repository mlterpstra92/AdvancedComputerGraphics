#include "Fluid.h"

#include "Particle.h"
#include "Vector.h"
#include "OpenGL.h"
#include "Emitter.h"
#include "Solver.h"

#include <cstdlib>
#include <math.h>
#include <iostream>


Fluid::Fluid(double volume)
  : volume(volume)
{
	emitter = new StreamEmitter();
}

Fluid* Fluid::init()
{
	maxParticles = ceil(restDensity * volume / mass);
	supportRadius = pow((3 * volume * kernelParticles) / (4 * M_PI * maxParticles), 1.0/3.0);

	emitter->initialise(maxParticles, this);
	solver.initialise(supportRadius);

	std::cout << "Fluid added of of mass " << mass << " and rest density " << restDensity << '\n';
	std::cout << "volume: " << volume << '\n';
	std::cout << "max particles: " << maxParticles << '\n';
	std::cout << "supportradius: " << supportRadius << '\n';

	return this;
}

Water::Water(double volume)
  : Fluid(volume)
{
	restDensity = 998.29;
	mass = 0.02;
	buoyancy = 0;
	viscosity = 3.5;
	surfaceTension = 0.0728;
	threshold = 7.065;
	gasStiffness = 3.0;
	restitution = 0.0;
	kernelParticles = 20.0;
	color = Vector3d(0.24, 0.56, 0.82);
	opacity = 0.5f;
	isGas = false;
}

Steam::Steam(double volume)
  : Fluid(volume)
{
	restDensity = 0.59;
	mass = 5e-5;
	buoyancy = 5.0;
	viscosity = 0.01 * 1.2e-5;
	surfaceTension = 0.0;
	threshold = -1.0; // n/a
	gasStiffness = 4.0;
	restitution = 0.0;
	kernelParticles = 12.0;
	color = Vector3d(1.0, 1.0, 1.0);
	opacity = 0.5f;
	isGas = true;
}

Mucus::Mucus(double volume)
  : Fluid(volume)
{
	restDensity = 100.0;
	mass = 0.04;
	buoyancy = 0.0;
	viscosity = 36.0;
	surfaceTension = 6.0;
	threshold = 5.0;
	gasStiffness = 5.0;
	restitution = 0.5;
	kernelParticles = 40.0;
	color = Vector3d(0.0, 1.0, 0.0);
	opacity = 0.8f;
	isGas = false;
}
