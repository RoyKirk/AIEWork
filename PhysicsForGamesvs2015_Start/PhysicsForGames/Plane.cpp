#include "Plane.h"



Plane::Plane()
{
	_shapeID = PLANE;
}


Plane::~Plane()
{
}

Plane::Plane(glm::vec2 _normal, float _distanceToOrigin, glm::vec4 _colour)
{
	_shapeID = PLANE;
	normal = _normal;
	distanceToOrigin = _distanceToOrigin;
	colour = _colour;
	centrePoint = normal*distanceToOrigin;
	parallel = glm::vec2(normal.y, -normal.x); //easy to rotate normal through 90degrees aroung z
	start = centrePoint + (parallel * lineSegmentLength);
	end = centrePoint - (parallel * lineSegmentLength);
}

void Plane::makeGizmo()
{
	Gizmos::add2DLine(start.xy, end.xy, colour);
}