#ifndef _RIGIDBODY_H_
#define _RIGIDBODY_H_

#include "PhysicsObject.h"

class RigidBody :
	public PhysicsObject
{
public:
	RigidBody();
	~RigidBody();
	RigidBody(glm::vec3 _position, glm::vec3 _velocity, float _rotation, float _mass);
	glm::vec2 position;
	glm::vec2 velocity;
	float mass;
	float rotation2D; //2D so we only need a single float to represent our rotation
	virtual void update(glm::vec3 gravity, float timeStep);
	virtual void debug();
	void applyForce(glm::vec2 force);
	void applyForceToActor(RigidBody* actor2, glm::vec3 force);
};

#endif // !_RIGIDBODY_H_