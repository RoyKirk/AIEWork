#pragma once
#include "RigidBody.h"
class Sphere :
	public RigidBody
{
public:
	Sphere();
	~Sphere();
	float _radius;
	Sphere(glm::vec3 position, glm::vec3 velocity, float mass, float radius, glm::vec4 colour);
	virtual void makeGizmo();
};

