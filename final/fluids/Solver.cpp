#include "Solver.h"
#include "Particle.h"
#include "NeighborSearch.h"
#include "Fluid.h"
#include "Kernel.h"

#include <vector>

void Solver::initialise(double h)
{
	W.initialise(h);
}

void Solver::calculateDensity(Particle& par)
{
	par.density = par.fluid->mass * W.poly(Vector3d());

	for (NeighborList::iterator it = par.neighs.begin(); it != par.neighs.end(); ++it)
	{
		Particle* neighbor = *it;

		par.density += neighbor->fluid->mass * neighbor->fluid->solver.W.poly(par.to(*neighbor));
	}
}

void Solver::calculatePressure(Particle& par)
{
	par.pressure = par.fluid->gasStiffness * (par.density - par.fluid->restDensity);
}

void Solver::calculatePressureForce(Particle& par)
{
	par.f_pressure = Vector3d(0.0, 0.0, 0.0);
	double rho = par.pressure / (par.density * par.density);

	for (NeighborList::iterator it = par.neighs.begin(); it != par.neighs.end(); ++it)
	{
		Particle* neighbor = *it;

		Vector3d kernel = neighbor->fluid->solver.W.spikyGradient(par.to(*neighbor));
		double res = (rho + neighbor->pressure / (neighbor->density * neighbor->density)) * neighbor->fluid->mass;
		par.f_pressure += kernel * res;
	}

	par.f_pressure *= -par.density;
}

void Solver::calculatePressureForceSymmetric(Particle& par)
{
	par.f_pressure = Vector3d(0.0, 0.0, 0.0);

	for (NeighborList::iterator it = par.neighs.begin(); it != par.neighs.end(); ++it)
	{
		Particle* neighbor = *it;

		Vector3d kernel = neighbor->fluid->solver.W.spikyGradient(par.to(*neighbor));
		double res = (par.pressure + neighbor->pressure) / 2.0 * neighbor->fluid->mass / neighbor->density;
		par.f_pressure += kernel * res;
	}

	par.f_pressure *= -1.0;
}

void Solver::calculateViscosityForce(Particle& par)
{
	par.f_viscosity = Vector3d(0.0, 0.0, 0.0);

	for (NeighborList::iterator it = par.neighs.begin(); it != par.neighs.end(); ++it)
	{
		Particle* neighbor = *it;

		double kernel = neighbor->fluid->solver.W.laplacianViscosity(par.to(*neighbor));
		par.f_viscosity += ((neighbor->velocity - par.velocity) / neighbor->density) * neighbor->fluid->mass * kernel;
	}

	par.f_viscosity *= par.fluid->viscosity;
}

void Solver::calculateGravityForce(Particle& par, Vector3d gravity)
{
	par.f_gravity = gravity * par.density;
}

void Solver::calculateBuoyancyForce(Particle& par, Vector3d gravity)
{
	par.f_buoyancy = gravity * par.fluid->buoyancy * (par.density - par.fluid->restDensity);
}

void Solver::calculateSurfaceNormal(Particle& par)
{
	par.surface_normal = W.gradient(Vector3d()) * (par.fluid->mass / par.density);

	for (NeighborList::iterator it = par.neighs.begin(); it != par.neighs.end(); ++it)
	{
		Particle* neighbor = *it;

		Vector3d kernel = neighbor->fluid->solver.W.gradient(par.to(*neighbor));
		par.surface_normal += kernel * (neighbor->fluid->mass / neighbor->density);
	}
}

double Solver::evaluateColorField(Particle& par)
{
	double c = (par.fluid->mass / par.density) * W.poly(Vector3d());
	for (NeighborList::iterator it = par.neighs.begin(); it != par.neighs.end(); ++it)
	{
		Particle* neighbor = *it;
		c += (neighbor->fluid->mass / neighbor->density) * neighbor->fluid->solver.W.poly(par.to(*neighbor));
	}
	return c;
}

void Solver::calculateSurfaceTension(Particle& par)
{
	par.f_surface = par.surface_normal.normalized() * evaluateColorField(par) * -par.fluid->surfaceTension;
}
