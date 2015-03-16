#ifndef _H_SIMULATION_H_
#define _H_SIMULATION_H_

#include "Particle.h"
#include "Wall.h"
#include "Vector.h"
#include "Kernel.h"
#include "Solver.h"
#include "Fluid.h"
#include "NeighborSearch.h"

#include <vector>

class Simulation
{
public:
	Simulation();
	void initialise();
	void initialiseWalls();
	void initialiseParticles();
	void updateGravity();
	void integrate(Particle& par);
	void executeTimestep();

	std::vector<Fluid*> fluids;

	CollisionObject* collidingObject(Particle& p);

	void toggleSlidingWall();

	int frozen;
	int dynamicGravity;

	Vector3d boxMin;
	Vector3d boxMax;

	// g: gravitational acceleration in m/s^2
	Vector3d gravity;
	// dt: timestep in seconds
	double timeStep;

	// collision objects
	std::vector<CollisionObject*> collision_objects;

	// Solver
	Solver solver;

	// Search grid for NN search
	NeighborSearch neighbors;

	// Sliding wall
	SlidingWall* slider;
	int slidingWall;

	unsigned int counter;
};

#endif
