#include "Sphere.h"



Sphere::Sphere()
{
	_shapeID = SPHERE;
}


Sphere::~Sphere()
{
}

Sphere(glm::vec3 position, glm::vec3 velocity, float mass, float radius, glm::vec4 colour)