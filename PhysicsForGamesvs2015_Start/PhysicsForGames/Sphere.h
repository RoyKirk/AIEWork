#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "RigidBody.h"
class Sphere :
	public RigidBody
{
public:
	Sphere();
	~Sphere();
	float radius;
	Sphere(glm::vec3 _position, glm::vec3 _velocity, float _mass, float _radius, glm::vec4 _colour, float _linearDrag, bool _dynamic);
	virtual void makeGizmo();
};


#endif // !_SPHERE_H_