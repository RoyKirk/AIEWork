#ifndef _PHYSICSSCENE_H_
#define _PHYSICSSCENE_H_

#include <vector>
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "PhysicsObject.h"

class PhysicsScene
{
public:
	PhysicsScene();
	~PhysicsScene();
	glm::vec3 gravity;
	float timeStep;
	std::vector<PhysicsObject*> actors;
	const unsigned int NUMBERSHAPE = BOX;//last object in shapetype enumeration
	void addActor(PhysicsObject* _actor);
	void removeActor(PhysicsObject* _actor);
	void update();
	void debugScene();
	void addGizmos();
	void checkForCollision();
	bool plane2Plane(PhysicsObject* object1, PhysicsObject* object2);
	bool plane2SPhere(PhysicsObject* object1, PhysicsObject* object2);
	bool sphere2Plane(PhysicsObject* object1, PhysicsObject* object2);
	bool sphere2Sphere(PhysicsObject* object1, PhysicsObject* object2);
};


#endif