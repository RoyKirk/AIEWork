#include "Sphere.h"



Sphere::Sphere()
{
	_shapeID = SPHERE;
}


Sphere::~Sphere()
{
}

Sphere::Sphere(glm::vec3 _position, glm::vec3 _velocity, float _mass, float _radius, glm::vec4 _colour)
{
	_shapeID = SPHERE;
	position = _position.xy;
	velocity = _velocity.xy;
	mass = _mass;
	radius = _radius;
	colour = _colour;
}

void Sphere::makeGizmo()
{
	Gizmos::addSphereFilled(glm::vec3(position,0), radius, 16, 16, colour);
}
