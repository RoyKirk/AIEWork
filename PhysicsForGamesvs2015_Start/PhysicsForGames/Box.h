#ifndef _BOX_H_
#define _BOX_H_

#include "RigidBody.h"
class Box :
	public RigidBody
{
public:
	Box();
	~Box();
	glm::vec3 minCorner;
	glm::vec3 maxCorner;
	float height;
	float width;
	float depth;
	glm::mat4 rotation;
	glm::vec3 extents;
	Box(glm::vec3 _position, glm::vec3 _velocity, float _mass, float _width, float _height, float _depth, glm::mat4 _rotation, glm::vec4 _colour, float _linearDrag);
	virtual void makeGizmo();
};

#endif // !_BOX_H_