#include <cstdlib>
#include "Wall.h"
#include "Particle.h"
#include "Vector.h"
#include "CollisionObject.h"
#include "OpenGL.h"
#include <math.h>

/* actually a plane */
Wall::Wall(Vector3d p0, Vector3d p1, Vector3d p2, Vector3d p3, Vector3d color)
{
	this->p0 = p0;
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	this->color = color;

	this->normal = (p1 - p0).crossProduct(p2 - p0);
	this->normal.normalize();

	velocity = Vector3d();
	useShader = false;
}

bool Wall::hasCollision(Particle& par)
{
	double d = normal.dotProduct(par.position - p0);

	if (d < -EPSILON)
	{
		contactPoint = par.position - normal * d;
		penetrationDepth = -d;

		return true;
	}

	return false;
}

void Wall::render()
{
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_LINE_LOOP);
	    glVertex3f(p0[0], p0[1], p0[2]);
	    glVertex3f(p1[0], p1[1], p1[2]);
	    glVertex3f(p2[0], p2[1], p2[2]);
	    glVertex3f(p3[0], p3[1], p3[2]);
    glEnd();
}

SlidingWall::SlidingWall(Wall *from, Vector3d color, double offset, double speed /* = 1.0*/)
  : Wall(from->p0, from->p1, from->p2, from->p3, color),
    from(from),
    offset(offset),
    speed(speed),
    lastp(0.0)
{
}

void SlidingWall::reset(unsigned int timestep)
{
	p0 = from->p0;
	p1 = from->p1;
	p2 = from->p2;
	p3 = from->p3;
	start = timestep;
	lastp = 0.0;
}

void SlidingWall::move(unsigned int timestep)
{
	double p = 0.5 - 0.5*cos((timestep - start)/ 50.0 * speed);
	velocity = normal * (p - lastp) * 30.0;
	Vector3d d = normal * p * offset;

	p0 = from->p0 + d;
	p1 = from->p1 + d;
	p2 = from->p2 + d;
	p3 = from->p3 + d;
	lastp = p;
}
