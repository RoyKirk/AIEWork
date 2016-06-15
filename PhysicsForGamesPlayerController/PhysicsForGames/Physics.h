#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include "Application.h"
#include "Camera.h"
#include "Render.h"
#include "ParticleEmitter.h"
#include "ParticleFluidEmitter.h"

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>
#include <vector>



using namespace physx;

class MyControllerHitReport : public PxUserControllerHitReport
{
public:
	//overload the onShapeHit function
	virtual void onShapeHit(const PxControllerShapeHit &hit);
	//other collision functions which we must overload
	//these handle collision with other controllers and hitting obstacles
	virtual void onControllerHit(const PxControllersHit &hit) {};
	//Called when current controller hits another controller. More...
	virtual void onObstacleHit(const PxControllerObstacleHit &hit) {};
	//Called when current controller hits a user-defined obstacle
	MyControllerHitReport() :PxUserControllerHitReport() {};
	PxVec3 getPlayerContactNormal() { return _playerContactNormal; };
	void clearPlayerContactNormal() { _playerContactNormal = PxVec3(0, 0, 0); };
	PxVec3 _playerContactNormal;
};

//Parts which make up the ragdoll
enum RagDollParts
{
	NO_PARENT = -1,
	LOWER_SPINE,
	LEFT_PELVIS,
	RIGHT_PELVIS,
	LEFT_UPPER_LEG,
	RIGHT_UPPER_LEG,
	LEFT_LOWER_LEG,
	RIGHT_LOWER_LEG,
	UPPER_SPINE,
	LEFT_CLAVICLE,
	RIGHT_CLAVICLE,
	NECK,
	HEAD,
	LEFT_UPPER_ARM,
	RIGHT_UPPER_ARM,
	LEFT_LOWER_ARM,
	RIGHT_LOWER_ARM,
};

struct RagdollNode
{
	PxQuat globalRotation; //rotation of this link in model space - we could have done this relative to the parent node but it"s harder to visualise when setting up the data by hand
	PxVec3 scaledGlobalPos; //position of the link centre in world space which is calculated when we process the node. It's easiest if we store it here so we have it when we transform tje child
	int parentNodeIdx; //Index of the parent node
	float halfLength; //half length of the capsule for this node
	float radius; //radius of capsule
	float parentLinkPos; //relative position of link centre in parent to this node. 0 is the centre of the node, -1 is left end of the capsule and 1 is right end of the capsule relative to x
	float childLinkPos; //relative position of link centre in child
	char* name; //name of link
	float ySwingLimit;
	float zSwingLimit;
	PxArticulationLink* linkPtr;
	//constructor
	RagdollNode(PxQuat _globalRotation, int _parentNodeIdx, float _halfLength, float _radius, float _parentLinkPos, float _childLinkPos, char* _name, float _ySwingLimit, float _zSwingLimit)
	{
		globalRotation = _globalRotation;
		parentNodeIdx = _parentNodeIdx;
		halfLength = _halfLength;
		radius = _radius;
		parentLinkPos = _parentLinkPos;
		childLinkPos = _childLinkPos;
		name = _name;
		ySwingLimit = _ySwingLimit;
		zSwingLimit = _zSwingLimit;
	};
};


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


	void Shoot();
	PxArticulation* makeRagdoll(PxPhysics* g_Physics, RagdollNode** nodeArray, PxTransform worldPos, float scaleFactor, PxMaterial* ragdollMaterial);
    Renderer* m_renderer;
    FlyCamera m_camera;
    float m_delta_time;
	float muzzleSpeed;
	float shootTimer;
	float shootTimeOut;

	void updatePlayerController(float delta);

	float _characterYVelocity = 0;
	float _characterRotation = 0;
	float _playerGravity = -0.5f;
	
	std::vector<PxRigidDynamic*> box_list;

	MyControllerHitReport* myHitReport;
	
	PxControllerManager* gCharacterManager;
	
	PxController* gPlayerController;

	PxExtendedVec3 startingPosition;

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



#endif //CAM_PROJ_H_
