#ifndef _PHYSICSOBJECT_H_
#define _PHYSICSOBJECT_H_

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "Gizmos.h"

enum ShapeType
{
	PLANE = 0,
	SPHERE = 1,
	BOX = 2,
	JOINT = 3
};

class PhysicsObject
{
public:
	PhysicsObject();
	~PhysicsObject();
	ShapeType _shapeID;
	glm::vec4 colour;
	void virtual update(glm::vec3 gravity, float timeStep) = 0;
	void virtual debug() = 0;
	void virtual makeGizmo() = 0;
	void virtual resetPosition() {};
};

#endif // !_PHYSICSOBJECT_H_