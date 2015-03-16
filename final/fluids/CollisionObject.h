#ifndef _H_COLISSION_OBJECT_H_
#define _H_COLISSION_OBJECT_H_
#include "Vector.h"

class CollisionObject
{
public:
	virtual bool hasCollision(Particle& par) = 0;

	virtual void render() = 0;

	Vector3d contactPoint;
	float penetrationDepth;
	Vector3d normal;
	Vector3d velocity;

	bool useShader;
};

#endif
