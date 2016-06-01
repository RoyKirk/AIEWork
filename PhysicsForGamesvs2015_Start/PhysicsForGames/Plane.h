#ifndef _PLANE_H_
#define _PLANE_H_

#include "PhysicsObject.h"

class Plane :
	public PhysicsObject
{
public:
	Plane();
	~Plane();
	Plane(glm::vec3 _normal, float _distanceToOrigin);
	glm::vec3 normal;
	float distanceToOrigin;
};

#endif // !_PLANE_H_