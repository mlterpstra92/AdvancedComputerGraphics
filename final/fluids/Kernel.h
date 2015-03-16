#ifndef _H_KERNEL_H_
#define _H_KERNEL_H_

#include "Vector.h"

#include <math.h>
#include <iostream>

class Kernel
{
public:
	void initialise(double h)
	{
		this->h = h;
		this->hSq = h*h;

		polyBase = 315.0 / (64.0 * M_PI * pow(h, 9));
		gradientBase = -945.0 / (32.0 * M_PI * pow(h, 9));
		laplacianBase = -945.0 / (32.0 * M_PI * pow(h, 9));
		spikyBase = 45.0 / (M_PI * pow(h, 6));
	}

	double poly(Vector3d r)
	{
		return polyBase * pow(hSq - r.lengthSq(), 3);
	}

	Vector3d gradient(Vector3d r)
	{
		return r * gradientBase * pow(hSq - r.lengthSq(), 2);
	}

	Vector3d spikyGradient(Vector3d r)
	{
		double L = r.length();
		return r / L * (h-L) * (h-L) * -spikyBase;
	}

	double laplacianViscosity(Vector3d r)
	{
		return spikyBase * (h - r.length());
	}

	double laplacian(Vector3d r)
	{
		return laplacianBase * (hSq - r.lengthSq()) * (3*hSq - 7 * r.lengthSq());
	}

private:
	double h;
	double hSq;
	double polyBase;
	double gradientBase;
	double laplacianBase;
	double spikyBase;
};

#endif
