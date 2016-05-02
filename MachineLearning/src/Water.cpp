#include "Water.h"
#include "Gizmos.h"

Water::Water(glm::vec2 centre, float range)
{
	_centre = centre;
	_range = range;
	active = true;
}
Water::~Water()
{
}

Water::Water()
{
	active = false;
}

void Water::addGizmo()
{
	if (active)
	{
		Gizmos::add2DCircle(_centre, _range, 20, glm::vec4(0, 0, 1, .3));
	}
}

float Water::checkRange(glm::vec2 testPoint)
{
	if (active)
	{
		glm::vec2 displacment = testPoint - _centre;
		float distance = glm::length(displacment);
		if (distance < _range)
		{
			return 1;
		}
	}
	return 0;
}