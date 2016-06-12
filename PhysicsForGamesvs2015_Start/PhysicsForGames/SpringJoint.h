#pragma once
#include "PhysicsObject.h"
#include "RigidBody.h"
class SpringJoint :
	public PhysicsObject
{
public:
	SpringJoint();
	~SpringJoint();
	SpringJoint(RigidBody* _connection1, RigidBody* _connection2, float _springCoefficient, float _damping);
	void virtual update(glm::vec3 gravity, float timeStep);
	void virtual debug();
	void virtual makeGizmo();

	RigidBody* connections[2];
	float damping;
	float restLength;
	float springCoefficient;


};

