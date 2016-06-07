#include "RigidBody.h"

RigidBody::RigidBody()
{
}


RigidBody::~RigidBody()
{
}

RigidBody::RigidBody(glm::vec3 _position, glm::vec3 _velocity, float _rotation, float _mass)
{
	position = _position.xy;
	velocity = _velocity.xy;
	mass = _mass;
	rotation2D = _rotation;
}

void RigidBody::update(glm::vec3 gravity, float timeStep)
{
	position += velocity*timeStep + gravity.xy*timeStep;
}
void RigidBody::debug()
{

}

void RigidBody::applyForce(glm::vec2 force)
{

}
void RigidBody::applyForceToActor(RigidBody* actor2, glm::vec3 force)
{

}