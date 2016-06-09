#include "RigidBody.h"

RigidBody::RigidBody()
{
}


RigidBody::~RigidBody()
{
}

RigidBody::RigidBody(glm::vec3 _position, glm::vec3 _velocity, float _rotation, float _mass, float _linearDrag)
{
	position = _position.xy;
	velocity = _velocity.xy;
	mass = _mass;
	rotation2D = _rotation;
	linearDrag = _linearDrag;
}

void RigidBody::update(glm::vec3 gravity, float timeStep)
{
	applyForce(gravity.xy*mass*timeStep);
	//velocity += gravity.xy*timeStep;
	position += velocity*timeStep;
	//applyForce(-velocity*linearDrag*mass*timeStep);
	velocity *= linearDrag;
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