#ifndef _H_WALL_H_
#define _H_WALL_H_

#include "Vector.h"
#include "Particle.h"
#include "CollisionObject.h"

class Wall : public CollisionObject
{
public:
	Wall(Vector3d p0, Vector3d p1, Vector3d p2, Vector3d p3, Vector3d color);

	virtual bool hasCollision(Particle& par);
	virtual void render();

	Vector3d p0, p1, p2, p3;
	Vector3d color;
};

class SlidingWall : public Wall
{
public:
	Wall* from;
	double offset;
	double speed;
	unsigned int start;
	double lastp;

	SlidingWall(Wall *from, Vector3d color, double offset, double speed = 1.0);

	void reset(unsigned int timestep);
	void move(unsigned int timestep);
};

#endif
