#include "Box.h"



Box::Box()
{
	_shapeID = BOX;
}


Box::~Box()
{
}

Box::Box(glm::vec3 _position, glm::vec3 _velocity, float _mass, float _width , float _height , float _depth, glm::mat4 _rotation, glm::vec4 _colour, float _linearDrag, bool _dynamic)
{
	_shapeID = BOX;
	position = _position.xy;
	velocity = _velocity.xy;
	mass = _mass;
	width = _width;
	height = _height;
	depth = _depth;
	extents = glm::vec3(width / 2, height / 2, depth / 2);
	rotation = _rotation;
	colour = _colour;
	linearDrag = _linearDrag;
	dynamic = _dynamic;
}
void Box::makeGizmo()
{	
	Gizmos::addAABBFilled(glm::vec3(position, 0), extents , colour, &rotation);
}
