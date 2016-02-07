#include "Planet.h"



Planet::Planet()
{
}

Planet::Planet(Planet* parentIn, float distanceIn, float orbitSpeedIn, float sizeIn)
{
	this->m_parent = parentIn;
	this->m_distance = distanceIn;
	this->m_orbitSpeed = orbitSpeedIn;
	this->m_size = sizeIn;
}

Planet::~Planet()
{
}
