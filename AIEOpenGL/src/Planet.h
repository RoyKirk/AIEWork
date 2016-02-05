#ifndef _PLANET_H_
#define _PLANET_H_

#include <glm/glm.hpp>
using glm::vec3;
using glm::vec4;
using glm::mat4;

class Planet
{
public:
	Planet();
	Planet(Planet* parentIn, float distanceIn, float orbitSpeedIn, float sizeIn);
	~Planet();

	public:
	Planet* m_parent;
	float m_distance;
	mat4 m_local = mat4(1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						m_distance, 0, m_distance, 1);;
	float m_rotation = 0;
	float m_orbitSpeed;
	float m_size;
};

#endif