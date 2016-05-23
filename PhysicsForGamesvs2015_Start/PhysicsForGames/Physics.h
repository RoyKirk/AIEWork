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
	

	void Shoot();

    Renderer* m_renderer;
    FlyCamera m_camera;
    float m_delta_time;
	float muzzleSpeed;
	float shootTimer;
	float shootTimeOut;
	

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

//create some constants for axis of rotation to make definition of wuaternions a bit neater
const PxVec3 X_AXIS = PxVec3(1, 0, 0);
const PxVec3 Y_AXIS = PxVec3(0, 1, 0);
const PxVec3 Z_AXIS = PxVec3(0, 0, 1);

struct RagdollNode
{
	PxQuat globalRotation; //rotation of this link in model space - we could have done this relative to the parent node but it"s harder to visuakise when setting up the data by hand
	PxVec3 scaledGlobalPos; //position of the link centre in world space which is calculated when we process the node. It's easiest if we store it here so we have it when we transform tje child
	int parentNodeIdx; //Index of the parent node
	float halfLength; //half length of the capsule for this node
	float radius; //radius of capsule
	float parentLinkPos; //relative position of link centre in parent to this node. 0 is the centre of the node, -1 is left end of the capsule and 1 is right end of the capsule relative to x
	float childLinkPos; //relative position of link centre in child
	char* name; //name of link
	PxArticulationLink* linkPtr;
	//constructor
	RagdollNode(PxQuat _globalRotation, int _parentNodeIdx, float _halfLength, float _radius, float _parentLinkPos, float _childLinkPos, char* _name)
	{
		globalRotation = _globalRotation;
		parentNodeIdx = _parentNodeIdx;
		halfLength = _halfLength;
		radius = _radius;
		parentLinkPos = _parentLinkPos;
		childLinkPos = _childLinkPos;
		name = _name;
	};
};

RagdollNode* ragdollData[] =
{
	new RagdollNode(PxQuat(PxPi / 2.0f,Z_AXIS), NO_PARENT,1,3,1,1,"lower spine"),
	new RagdollNode(PxQuat(PxPi, Z_AXIS), LOWER_SPINE,1,1,-1,1,"left pelvis"),
	new RagdollNode(PxQuat(0, Z_AXIS), LOWER_SPINE,1,1,-1,1,"right pelvis"),
	new RagdollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS), LEFT_PELVIS,5,2,-1,1,"L upper leg"),
	new RagdollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS), RIGHT_PELVIS,5,2,-1,1,"R upper leg"),
	new RagdollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS), LEFT_UPPER_LEG,5,1.75,-1,1,"L lower leg"),
	new RagdollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS), RIGHT_UPPER_LEG,5,1.75,-1,1,"R lower leg"),
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), LOWER_SPINE,1,3,1,-1,"upper spine"),
	new RagdollNode(PxQuat(PxPi, Z_AXIS), UPPER_SPINE,1,1.5,1,1,"left clavicle"),
	new RagdollNode(PxQuat(0, Z_AXIS), UPPER_SPINE,1,1.5,1,1,"right clavicle"),
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), UPPER_SPINE,1,1,1,-1,"neck"),
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), NECK,1,3,1,-1, "HEAD"),
	new RagdollNode(PxQuat(PxPi - 0.3, Z_AXIS), LEFT_CLAVICLE,3,1.5,-1,1,"left upper arm"),
	new RagdollNode(PxQuat(0.3, Z_AXIS), RIGHT_CLAVICLE,3,1.5,-1,1,"right upper arm"),
	new RagdollNode(PxQuat(PxPi - 0.3, Z_AXIS), LEFT_UPPER_ARM,3,1,-1,1,"left lower arm"),
	new RagdollNode(PxQuat(0.3, Z_AXIS), RIGHT_UPPER_ARM,3,1,-1,1,"right lower arm"),
	NULL
};


PxArticulation* makeRagdoll(PxPhysics* g_Physics, RagdollNode** nodeArray, PxTransform worldPos, float scaleFactor, PxMaterial* ragdollMaterial)
{
	//create the articulation for our ragdoll
	PxArticulation *articulation = g_Physics->createArticulation();
	RagdollNode** currentNode = nodeArray;
	//while there are more nodes to process...
	while (*currentNode != NULL)
	{
		//get a pointer to the current node
		RagdollNode* currentNodePtr = *currentNode;
		//create a pointer ready to hold the parent node pointer if there is one
		RagdollNode* parentNode = nullptr;
		//get scaled values for capsule
		float radius = currentNodePtr->radius*scaleFactor;
		float halfLength = currentNodePtr->halfLength*scaleFactor;
		float childHalfLength = radius + halfLength;
		float parentHalfLength = 0; //will be set later if there is a parnet
		//get a pointer to the parent
		PxArticulationLink* parentLinkPtr = NULL;
		currentNodePtr->scaledGlobalPos = worldPos.p;
		if (currentNodePtr->parentNodeIdx != 1)
		{
			//if there is a parent then we need to work out our local position for the link
			//get a pointer to the parent node			parentNode = *(nodeArray + currentNodePtr->parentNodeIdx);
			//get a pointer to the link for the parent
			parentLinkPtr = parentNode->linkPtr;
			parentHalfLength = (parentNode->radius + parentNode->halfLength) *scaleFactor;
			//work out the local position of the node
			PxVec3 currentRelative = currentNodePtr->childLinkPos * currentNodePtr->globalRotation.rotate(PxVec3(childHalfLength, 0, 0));
			PxVec3 parentRelative = -currentNodePtr->parentLinkPos * parentNode->globalRotation.rotate(PxVec3(parentHalfLength, 0, 0));
			currentNodePtr->scaledGlobalPos = parentNode->scaledGlobalPos - (parentRelative + currentRelative);
		}
		//build the ransform for the link
		PxTransform linkTransform = PxTransform(currentNodePtr->scaledGlobalPos, currentNodePtr->globalRotation);
		//create the link in the articulation		PxArticulationLink* link = articulation->createLink(parentLinkPtr, linkTransform);
		//add the pointer to this link into the ragdoll data so we have it for later when we want to link to it		currentNodePtr->linkPtr = link;
		float jointSpace = 0.01f; //gap between joints
		float capsuleHalfLength = (halfLength > jointSpace ? halfLength - jointSpace : 0) + 0.01f;
		PxCapsuleGeometry capsule(radius, capsuleHalfLength);
		link->createShape(capsule, *ragdollMaterial); //adds a capsule collider to the link
		PxRigidBodyExt::updateMassAndInertia(*link, 50.0f); //adds some mass, mass should really be part of the data!
		if (currentNodePtr->parentNodeIdx != -1)
		{
			//get the pointer to the joint from the link
			PxArticulationJoint* joint = link->getInboundJoint();
			//get the relative rotation of this link
			PxQuat frameRotation = parentNode->globalRotation.getConjugate() * currentNodePtr->globalRotation;
			//set the parent constraint frame
			PxTransform parentConstraintFrame = PxTransform(PxVec3(currentNodePtr->parentLinkPos*parentHalfLength, 0, 0), frameRotation);
			//set the child constraint frame (this the constraint frame of the newly added link)
			PxTransform thisConstraintFrame = PxTransform(PxVec3(currentNodePtr->childLinkPos*childHalfLength, 0, 0));
			//set up the poses for the joint so it is in the correct place
			joint->setParentPose(parentConstraintFrame);
			joint->setChildPose(thisConstraintFrame);
			//set up some constraints to stop it flopping around
			joint->setStiffness(20);
			joint->setDamping(20);
			joint->setSwingLimit(0.4f, 0.4f);
			joint->setSwingLimitEnabled(true);
			joint->setTwistLimit(-0.1f, 0.1f);
			joint->setTwistLimitEnabled(true);
		}
		currentNode++;
	}
	return articulation;
}

#endif //CAM_PROJ_H_
