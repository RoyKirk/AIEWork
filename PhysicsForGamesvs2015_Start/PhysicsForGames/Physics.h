#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include "Application.h"
#include "Camera.h"
#include "Render.h"

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>

using namespace physx;
class Physics : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
    virtual bool update();
    virtual void draw();

	void renderGizmos(PxScene* physics_scene);

	void SetUpPhysX();
	void UpdatePhysX(float a_deltaTime);
	void SetUpVisualDebugger();
	void SetUpTutorial1();

    Renderer* m_renderer;
    FlyCamera m_camera;
    float m_delta_time;

	PxFoundation* g_PhysicsFoundation;
	PxPhysics* g_Physics;
	PxScene* g_PhysicsScene;
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;
	PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
	PxMaterial* g_PhysicsMaterial;
	PxMaterial* g_boxMaterial;
	PxCooking* g_PhysicsCooker;
};

class myAllocator : public PxAllocatorCallback
{
public:
	virtual ~myAllocator() {}
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line)
	{
		void* pointer = _aligned_malloc(size, 16);
		return pointer;
	}
	virtual void deallocate(void* ptr)
	{
		_aligned_free(ptr);
	}
};

//enum ShapeType
//{
//	PLANE = 0,
//	SPHERE = 1,
//	BOX = 2
//};
//
//class PhysicsObject
//{
//public:
//	ShapeType _shapeID;
//	void virtual update(glm::vec3 gravity, float timeStep) = 0;
//	void virtual denug() = 0;
//	void virtual makeGizmo() = 0;
//	void virtual resetPosition() {};
//};
//
//class DIYRigidBody: public PhysicsObject
//{
//public:
//	DIYRigidBody(glm::vec3 position, glm::vec3 velocity, glm::quat rotation, float mass);
//	glm::vec2 position;
//	glm::vec2 velocity;
//	float mass;
//	float rotation2D; //2D so we only need a single float to represent our rotation
//	virtual void update(glm::vec2 gravity, float timeStep);
//	virtual void debug();
//	void applyForce(glm::vec2 force);
//	void applyForceToActor(DIYRigidBody* actor2, glm::vec3 force);
//};
//
//class SphereClass : public DIYRigidBody
//{
//public:
//	float _radius;
//	SphereClass(glm::vec3 position, glm::vec3 velocity, float mass, float radius, glm::vec4 colour);
//	virtual void makeGizmo();
//};
//
//class DIYPhysicScene
//{
//public:
//	glm::vec3 gravity;
//	float timeStep;
//	std::vector<PhysicsObject*> actors;
//	void addActor(PhysicsObject*);
//	void removeActor(PhysicsObject*);
//	void update();
//	void debugScene();
//	void addGizmos();
//};

#endif //CAM_PROJ_H_
