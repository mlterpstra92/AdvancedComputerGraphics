#ifndef _H_PARTICLE_H_
#define _H_PARTICLE_H_

#include "Vector.h"
#include <list>

class Fluid;

class Particle
{
public:
	Particle(Fluid* fluid);

	Fluid *fluid;

	std::list<Particle*> neighs;

	// the position of this particle
	Vector3d position;

	// the approximated velocity of this particle based on halfway velocity
	Vector3d velocity;

	// the velocity of this particle, at time t - 0.5
	Vector3d velocityHalfway;

	// the acceleration of this particle at t=t
	Vector3d acceleration;

	// aggregate Forces
	Vector3d force() { return f_internal() + f_external(); }

	Vector3d f_internal() { return f_pressure + f_viscosity; }
	Vector3d f_external() { return f_gravity + f_surface + f_buoyancy; }

	Vector3d f_pressure;
	Vector3d f_gravity;
	Vector3d f_viscosity;
	Vector3d f_surface;
	Vector3d f_buoyancy;

	Vector3d surface_normal;

	double distSq(Particle& other)
	{
		return (position - other.position).lengthSq();
	}

	Vector3d to(Particle& other)
	{
		return position - other.position;
	}

	// the render radius (determined by 5.20)
	double const renderRadius();
	double dynamicRenderRadius();

	void render(bool drawDynamicParticleSize);

	// particle density
	double density;

	// pressure at this particle
	double pressure;
};

#endif
