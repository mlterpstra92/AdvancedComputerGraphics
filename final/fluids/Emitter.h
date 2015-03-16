#ifndef _H_EMITTER_H_
#define _H_EMITTER_H_

#include "Vector.h"
#include "Particle.h"

#include <time.h>
#include <vector>
#include <stdlib.h>

class Fluid;

class Emitter
{
public:
	Fluid* fluid;
	unsigned int maxParticles;

	void initialise(int maxParticles, Fluid* fluid)
	{
		this->maxParticles = maxParticles;
		this->fluid = fluid;
	}

	virtual void emit(std::vector<Particle>& particles) = 0;

	double generateRandomDouble(double fMin, double fMax)
	{
	    double f = (double)rand() / RAND_MAX;
	    return fMin + f * (fMax - fMin);
	}
};

class StreamEmitter : public Emitter
{
public:
	unsigned int emitPerTimestep;
	unsigned int emitted;

	Vector3d initialPosition;
	Vector3d initialVelocity;
	double maxVariance;

	StreamEmitter()
	{
		emitPerTimestep = 2;
		initialPosition = Vector3d(-0.15, 0.1, 0.0);
		initialVelocity = Vector3d(2.0, 0.0, 0.0);
		maxVariance = 0.05;
		emitted = 0;
	}

	virtual void emit(std::vector<Particle>& particles)
	{
		for (unsigned int i = 0; i < emitPerTimestep && emitted < maxParticles; emitted++, i++)
		{
			Particle p(fluid);
			double xvar = generateRandomDouble(-0.01, 0.01);
			double yvar = generateRandomDouble(-maxVariance, maxVariance);
			double zvar = generateRandomDouble(-maxVariance, maxVariance);

			p.position = Vector3d(initialPosition.x + xvar, initialPosition.y + yvar, initialPosition.z + zvar);
			p.velocityHalfway = initialVelocity;

			particles.push_back(p);
		}
	}
};

class DamBreakEmitter : public Emitter
{
public:
	Vector3d initialPosition;
	Vector3d between;

	virtual void emit(std::vector<Particle>& particles)
	{
		static bool ran = false;
		if (ran) return;
		ran = true;

		int n = pow(maxParticles, 1.0/3.0);

		for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j)
		for (int k = 0; k < n; ++k)
		{
			Particle p(fluid);
			p.position = initialPosition + Vector3d(i, j, k) * between;

			particles.push_back(p);
		}
	}
};

#endif