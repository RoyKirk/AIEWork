#pragma once

#include "glm/glm.hpp"

class Water
{
private:
	glm::vec2 _centre;
	float _range;
	bool active;
public:
	Water();
	Water(glm::vec2, float range);
	~Water();
	void addGizmo();
	float checkRange(glm::vec2 testPoint);
};

