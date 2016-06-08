#ifndef _PLANE_H_
#define _PLANE_H_

#include "PhysicsObject.h"

class Plane :
	public PhysicsObject
{
public:
	Plane();
	~Plane();
	Plane(glm::vec2 _normal, float _distanceToOrigin, glm::vec4 _colour);
	void virtual update(glm::vec3 gravity, float timeStep) {};
	void virtual debug() {};
	void virtual makeGizmo();
	glm::vec2 normal;
	float distanceToOrigin;
	glm::vec4 colour;
	float lineSegmentLength = 1000;
	glm::vec2 centrePoint;
	glm::vec2 parallel;
	glm::vec2 start;
	glm::vec2 end;
};

#endif // !_PLANE_H_