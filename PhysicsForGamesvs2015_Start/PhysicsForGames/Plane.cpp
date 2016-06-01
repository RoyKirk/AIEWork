#include "Plane.h"



Plane::Plane()
{
	_shapeID = PLANE;
}


Plane::~Plane()
{
}

Plane::Plane(glm::vec3 _normal, float _distanceToOrigin)
{
	_shapeID = PLANE;
	normal = _normal;
	distanceToOrigin = _distanceToOrigin;
}
