#include "SpringJoint.h"



SpringJoint::SpringJoint()
{
}


SpringJoint::~SpringJoint()
{
}

SpringJoint::SpringJoint(RigidBody* _connection1, RigidBody* _connection2, float _springCoefficient, float _damping)
{
	connections[0] = _connection1;
	connections[1] = _connection2;
	springCoefficient = _springCoefficient;
	damping = _damping;
	restLength = glm::length(connections[0]->position - connections[1]->position);
	_shapeID = JOINT;
}
void SpringJoint::update(glm::vec3 gravity, float timeStep)
{
	float x = springCoefficient*(restLength - glm::length(connections[0]->position - connections[1]->position));
	connections[0]->applyForceToActor(connections[1], x*damping*glm::normalize(connections[0]->position - connections[1]->position));
}
void SpringJoint::debug()
{

}
void SpringJoint::makeGizmo()
{
	Gizmos::add2DLine(connections[0]->position, connections[1]->position, glm::vec4(1, 1, 1, 1));
}