#ifndef _H_SOLVER_H_
#define _H_SOLVER_H_

#include "Particle.h"
#include "NeighborSearch.h"
#include "Kernel.h"

#include <vector>

class Solver
{
public:
	Kernel W;

	void initialise(double h);

	void calculateDensity(Particle& par);
	void calculatePressure(Particle& par);
	void calculatePressureForce(Particle& par);
	void calculatePressureForceSymmetric(Particle& par);
	void calculateViscosityForce(Particle& par);
	void calculateGravityForce(Particle& par, Vector3d gravity);
	void calculateBuoyancyForce(Particle& par, Vector3d gravity);
	void calculateSurfaceNormal(Particle& par);
	double evaluateColorField(Particle& par);
	void calculateSurfaceTension(Particle& par);
};

#endif