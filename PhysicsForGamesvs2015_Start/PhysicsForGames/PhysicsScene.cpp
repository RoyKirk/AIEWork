#include "PhysicsScene.h"

typedef bool(PhysicsScene::*fn)(PhysicsObject*, PhysicsObject*);
//function pointer array for doing our collisions
static fn collisionFunctionArray[] = 
{ 
	&PhysicsScene::plane2Plane,		&PhysicsScene::plane2SPhere,	&PhysicsScene::plane2Box,
	&PhysicsScene::sphere2Plane,	&PhysicsScene::sphere2Sphere,	&PhysicsScene::sphere2Box,
	&PhysicsScene::box2Plane,		&PhysicsScene::box2Sphere,		&PhysicsScene::box2Box,
};

PhysicsScene::PhysicsScene()
{
}


PhysicsScene::~PhysicsScene()
{
	actors.clear();
}

void PhysicsScene::addActor(PhysicsObject* _actor)
{
	actors.push_back(_actor);
}

void PhysicsScene::removeActor(PhysicsObject* _actor)
{
	for (unsigned int i = 0; i < actors.size(); i++)
	{
		if (actors[i] == _actor)
		{
			actors.erase(actors.begin() + i);
			break;
		}
	}
}

void PhysicsScene::update()
{
	for each (PhysicsObject* _actor in actors)
	{
		_actor->update(gravity,timeStep);
	}
	checkForCollision();
}

void PhysicsScene::debugScene()
{

}

void PhysicsScene::addGizmos()
{
	for each (PhysicsObject* _actor in actors)
	{
		_actor->makeGizmo();
	}
}

void PhysicsScene::checkForCollision()
{
	int actorCount = actors.size();
	//need to check for collisions against all objects except this one.
	for (int outer = 0; outer < actorCount - 1; outer++)
	{
		for (int inner = outer + 1; inner < actorCount; inner++)
		{
			PhysicsObject* object1 = actors[outer];
			PhysicsObject* object2 = actors[inner];
			int _shapeID1 = object1->_shapeID;
			int _shapeID2 = object2->_shapeID;
			//using function pointers
			int functionIndex = _shapeID1*NUMBERSHAPE+_shapeID2;
			fn collisionFunctionPtr = collisionFunctionArray[functionIndex];
			if (collisionFunctionPtr != NULL)
			{
				(this->*collisionFunctionPtr)(object1, object2);
			}
		}
	}
}
bool PhysicsScene::plane2Plane(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}
bool PhysicsScene::plane2SPhere(PhysicsObject* object1, PhysicsObject* object2)
{
	//try to cast objects to sphere and sphere
	Plane *plane = dynamic_cast<Plane*>(object1);
	Sphere *sphere = dynamic_cast<Sphere*>(object2);
	//if we are successful then test for collision
	if (plane != NULL && sphere != NULL)
	{
		glm::vec2 collisionNormal = plane->normal;
		float sphereToPlaneDsitance = glm::dot(sphere->position, plane->normal) - plane->distanceToOrigin;
		if (sphereToPlaneDsitance < 0)
		{
			collisionNormal *= -1;
			sphereToPlaneDsitance *= -1;
		}
		float intersection = sphere->radius - sphereToPlaneDsitance;
		if (intersection > 0)
		{
			//find the point where the collision occured (we need this for collision response later)
			//the plane is always static so collision response only applies to the sphere
			glm::vec2 planeNormal = plane->normal;
			if(sphereToPlaneDsitance < 0)
			{
				planeNormal *= -1; //flip the normal if we are behind the plane
			}
			glm::vec2 forceVector = -1 * sphere->mass * planeNormal * (glm::dot(planeNormal, sphere->velocity));
			sphere->applyForce(2.0f * forceVector);
			sphere->position += collisionNormal * intersection * 0.5f;

			return true;
		}
	}
	return false;
}
bool PhysicsScene::plane2Box(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}
bool PhysicsScene::sphere2Plane(PhysicsObject* object1, PhysicsObject* object2)
{
	return plane2SPhere(object2,object1);
}
bool PhysicsScene::sphere2Sphere(PhysicsObject* object1, PhysicsObject* object2)
{
	//try to cast objects to sphere and sphere
	Sphere *sphere1 = dynamic_cast<Sphere*>(object1);
	Sphere *sphere2 = dynamic_cast<Sphere*>(object2);
	//if we are successful then test for collision
	if (sphere1 != NULL && sphere2 != NULL)
	{
		glm::vec2 delta = sphere2->position - sphere1->position;
		float distance = glm::length(delta);
		float intersection = sphere1->radius + sphere2->radius - distance;
		if (intersection > 0)
		{
			glm::vec2 collisionNormal = glm::normalize(delta);
			glm::vec2 relativeVelocity = sphere2->velocity - sphere1->velocity;
			glm::vec2 collisionVector = collisionNormal * (glm::dot(relativeVelocity, collisionNormal));
			glm::vec2 forceVector = collisionVector * 1.0f / (1 / sphere1->mass + 1 / sphere2->mass);
			//use newtons third law to apply colision forces to colliding bodies.
			sphere1->applyForceToActor(sphere2, 2.0f * forceVector);
			//move our spheres out of collision
			glm::vec2 separationVector = collisionNormal * intersection *0.5f;
			sphere1->position -= separationVector;
			sphere2->position += separationVector;
			return true;
		}

	}
	return false;
}
bool PhysicsScene::sphere2Box(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}
bool PhysicsScene::box2Box(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}
bool PhysicsScene::box2Plane(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}
bool PhysicsScene::box2Sphere(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}