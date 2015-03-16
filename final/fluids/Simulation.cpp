#include "Simulation.h"
#include "Wall.h"
#include "Particle.h"
#include "Vector.h"
#include "Kernel.h"
#include "Solver.h"
#include "Emitter.h"
#include "Visualisation.h"
#include "Fluid.h"
#include "main.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h>

Simulation::Simulation()
  : frozen(false),
    dynamicGravity(false),
    boxMin(-0.35, -0.35, -0.35),
    boxMax(0.35, 0.35, 0.35),
    gravity(0.0, -9.81, 0.0),
    timeStep(0.005),
    slidingWall(false),
    counter(0)
{
	Fluid* water = new Water(0.2);
	((StreamEmitter*)water->emitter)->initialPosition = Vector3d(-0.34, 0.25, 0.0);
	((StreamEmitter*)water->emitter)->initialVelocity = Vector3d(2.0, 0.0, 0.0);
	((StreamEmitter*)water->emitter)->maxVariance = 0.1;
	((StreamEmitter*)water->emitter)->emitPerTimestep = 15;
	fluids.push_back(water->init());
}

void Simulation::toggleSlidingWall()
{
	if (slidingWall)
	{
		slider->reset(counter);
		collision_objects.push_back(slider);
	}
	else
	{
		collision_objects.erase(std::remove(collision_objects.begin(), collision_objects.end(), slider), collision_objects.end());
	}
}

void Simulation::executeTimestep()
{
	if (frozen) return;

	if (slidingWall) slider->move(counter);

	if (dynamicGravity) updateGravity();

	for (auto fit = fluids.begin(); fit != fluids.end(); ++fit)
		(*fit)->emitter->emit((*fit)->particles);

	// Update spatial hash of neigbours
	neighbors.clear();
	for (auto fit = fluids.begin(); fit != fluids.end(); ++fit)
		neighbors.update((*fit)->particles);

	// Compute particle state
	for (auto fit = fluids.begin(); fit != fluids.end(); ++fit)
	{
		#pragma omp parallel for
		for (unsigned int i=0; i < (*fit)->particles.size(); ++i)
		{
			Particle& p = (*fit)->particles[i];

			p.neighs = neighbors.query(p);

			//5.6.2ii: compute mass density for particles in neighbourhood using (4.6)
			(*fit)->solver.calculateDensity(p);
			//5.6.2iii: compute pressure using (4.12)
			(*fit)->solver.calculatePressure(p);
		}
	}

	// Compute forces
	for (auto fit = fluids.begin(); fit != fluids.end(); ++fit)
	{
		#pragma omp parallel for
		for (unsigned int i=0; i < (*fit)->particles.size(); ++i)
		{
			Particle& p = (*fit)->particles[i];

			//5.6.3ii: compute pressure force density acting on p using (4.10)
			(*fit)->solver.calculatePressureForce(p);

			//5.6.3iii: compute viscosity force density acting on p using (4.17)
			(*fit)->solver.calculateViscosityForce(p);

			//5.6.4i: compute the gravity force density using (4.24)
			if ((*fit)->isGas)
			{
				(*fit)->solver.calculateBuoyancyForce(p, gravity);
			}
			else
			{
				(*fit)->solver.calculateGravityForce(p, gravity);
				(*fit)->solver.calculateSurfaceNormal(p);

				p.f_surface = Vector3d();
				if (p.surface_normal.length() >= (*fit)->threshold)
					(*fit)->solver.calculateSurfaceTension(p);
			}


		}
	}

	// Integrate
	for (auto fit = fluids.begin(); fit != fluids.end(); ++fit)
	{
		for (unsigned int i=0; i < (*fit)->particles.size(); ++i)
		{
			integrate((*fit)->particles[i]);
		}
	}

	counter++;
}

void Simulation::updateGravity()
{
	gravity = Vector3d(0.0, -9.81, 0.0);

	double rotX = vis.rotationX / 180.0 * M_PI;
	double rotY = vis.rotationY / 180.0 * M_PI;

	// Rotate around x
	gravity = Vector3d(0.0, gravity.y * cos(rotX), gravity.y * sin(rotX));

	// Rotate around y
	gravity = Vector3d(gravity.z * sin(rotY), gravity.y, -gravity.z * cos(rotY));
}

//leapfrog integration
void Simulation::integrate(Particle& par)
{
	// compute a at time t
	par.acceleration = par.force() / par.density;

	Vector3d old = par.velocityHalfway;

	// compute v at time t + 0.5 using v at time t - 0.5
	par.velocityHalfway += par.acceleration * timeStep;

	// compute x at time t + 1, using v at time t + 0.5
	par.position += par.velocityHalfway * timeStep;

	// collision detection
	unsigned int numCollisions = 0;
	CollisionObject *object;
	Vector3d averagenormal;
	while ((object = collidingObject(par)))
	{
		par.position = object->contactPoint;

		double restitution = par.fluid->restitution > 0.0 ? par.fluid->restitution * (object->penetrationDepth / (timeStep * par.velocityHalfway.length())) : 0.0;

		par.velocityHalfway -= (object->normal * par.velocityHalfway.dotProduct(object->normal)) * (1.0 + restitution);		
		par.velocityHalfway += object->velocity;
		numCollisions++;
		averagenormal += object->normal;
	}
	if (numCollisions > 1)
		par.position += averagenormal * (0.001 *(double)rand() / RAND_MAX);

	// Update halfway velocity to time t + 0.5
	par.velocity = (par.velocityHalfway + old) * 0.5;
}


void Simulation::initialise()
{
	double maxSupportRadius = 0.0;
	for (unsigned int f = 0; f < fluids.size(); ++f)
		maxSupportRadius = fmax(maxSupportRadius, fluids[f]->supportRadius);

	std::cout << "max support radius: " << maxSupportRadius << '\n';
	neighbors.initialise(maxSupportRadius);

	initialiseWalls();
	toggleSlidingWall();
}

CollisionObject* Simulation::collidingObject(Particle& p)
{
	for (std::vector<CollisionObject*>::iterator it = collision_objects.begin(); it != collision_objects.end(); ++it)
	{
		if ((*it)->hasCollision(p)) return *it;
	}

	return NULL;
}

void Simulation::initialiseWalls()
{
	Vector3d p0(boxMin.x, boxMin.y, boxMin.z);
	Vector3d p1(boxMin.x, boxMin.y, boxMax.z);
	Vector3d p2(boxMax.x, boxMin.y, boxMax.z);
	Vector3d p3(boxMax.x, boxMin.y, boxMin.z);

	Vector3d p4(boxMin.x, boxMax.y, boxMin.z);
	Vector3d p5(boxMin.x, boxMax.y, boxMax.z);
	Vector3d p6(boxMax.x, boxMax.y, boxMax.z);
	Vector3d p7(boxMax.x, boxMax.y, boxMin.z);

	Vector3d color(0.5, 0.5, 0.5);

	Wall* bottom = new Wall(p0, p1, p2, p3, color);
	Wall* top = new Wall(p7, p6, p5, p4, color);
	Wall* front = new Wall(p4, p0, p3, p7, color);
	Wall* left = new Wall(p4, p5, p1, p0, color);
	Wall* rear = new Wall(p1, p5, p6, p2, color);
	Wall* right = new Wall(p3, p2, p6, p7, color);

	slider = new SlidingWall(right, color, 0.2, 1.0);

	collision_objects.push_back(bottom);
	collision_objects.push_back(top);
	collision_objects.push_back(front);
	collision_objects.push_back(left);
	collision_objects.push_back(rear);
	collision_objects.push_back(right);
}
