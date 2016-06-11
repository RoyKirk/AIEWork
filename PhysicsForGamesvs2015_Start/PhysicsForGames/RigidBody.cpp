#include "RigidBody.h"

RigidBody::RigidBody()
{
}


RigidBody::~RigidBody()
{
}

RigidBody::RigidBody(glm::vec3 _position, glm::vec3 _velocity, float _rotation, float _mass, float _linearDrag, bool _dynamic)
{
	position = _position.xy;
	velocity = _velocity.xy;
	mass = _mass;
	rotation2D = _rotation;
	linearDrag = _linearDrag;
	dynamic = _dynamic;
}

void RigidBody::update(glm::vec3 gravity, float timeStep)
{
	applyForce(gravity.xy*mass*timeStep);
	//velocity += gravity.xy*timeStep;
	position += velocity*timeStep;
	if (glm::length(velocity)>0)
	{
		//glm::vec2 dragVelocity = velocity;
		applyForce(-velocity*linearDrag);
		//velocity *= linearDrag;
	}
	if (glm::length(velocity) < 1)
	{
		velocity = glm::vec2(0);
	}
}
void RigidBody::debug()
{

}

void RigidBody::applyForce(glm::vec2 force)
{
	velocity += force / mass;
}
void RigidBody::applyForceToActor(RigidBody* actor2, glm::vec2 force)
{
	applyForce(force);
	actor2->applyForce(-force);
}