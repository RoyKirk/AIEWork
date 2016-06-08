#include "PhysicsScene.h"

//function pointer array for doing our collisions
typedef bool(*fn)(PhysicsObject*, PhysicsObject*);
//function pointer array for doing our collisions
static fn collisionFunctionArray[] = 
{ 
	PhysicsScene::plane2Plane, PhysicsScene::plane2SPhere, 
	PhysicsScene::sphere2Plane, PhysicsScene::sphere2Sphere,
};

PhysicsScene::PhysicsScene()
{
}


PhysicsScene::~PhysicsScene()
{
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
			int functionIndex = (_shapeID1*NUMBERSHAPE) + _shapeID2;
			fn collisionFunctionPtr = collisionFunctionArray[functionIndex];
			if (collisionFunctionPtr != NULL)
			{
				collisionFunctionPtr(object1, object2);
			}
		}
	}
}
bool PhysicsScene::plane2Plane(PhysicsObject* object1, PhysicsObject* object2)
{

}
bool PhysicsScene::plane2SPhere(PhysicsObject* object1, PhysicsObject* object2)
{

}
bool PhysicsScene::sphere2Plane(PhysicsObject* object1, PhysicsObject* object2)
{

}
bool PhysicsScene::sphere2Sphere(PhysicsObject* object1, PhysicsObject* object2)
{

}