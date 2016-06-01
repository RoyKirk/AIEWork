#ifndef _PHYSICSOBJECT_H_
#define _PHYSICSOBJECT_H_

#define GLM_SWIZZLE
#include "glm/glm.hpp"

enum ShapeType
{
	PLANE = 0,
	SPHERE = 1,
	BOX = 2
};

class PhysicsObject
{
public:
	PhysicsObject();
	~PhysicsObject();
	ShapeType _shapeID;
	void virtual update(glm::vec3 gravity, float timeStep) = 0;
	void virtual debug() = 0;
	void virtual makeGizmo() = 0;
	void virtual resetPosition() {};
};

#endif // !_PHYSICSOBJECT_H_