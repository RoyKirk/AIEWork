#include "Physics.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include "glm/ext.hpp"
#include "glm/gtc/quaternion.hpp"

#define Assert(val) if (val){}else{ *((char*)0) = 0;}
#define ArrayCount(val) (sizeof(val)/sizeof(val[0]))

//create some constants for axis of rotation to make definition of wuaternions a bit neater
const PxVec3 X_AXIS = PxVec3(1, 0, 0);
const PxVec3 Y_AXIS = PxVec3(0, 1, 0);
const PxVec3 Z_AXIS = PxVec3(0, 0, 1);


void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
PxFilterFlags myFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);


RagdollNode* ragdollData[] =
{
	new RagdollNode(PxQuat(PxPi / 2.0f,Z_AXIS), NO_PARENT,1,3,1,1,"lower spine",0.01f,0.3f),
	new RagdollNode(PxQuat(PxPi, Z_AXIS), LOWER_SPINE,1,1,-1,1,"left pelvis",0.1f,0.8f),
	new RagdollNode(PxQuat(0, Z_AXIS), LOWER_SPINE,1,1,-1,1,"right pelvis",0.1f,0.8f),
	new RagdollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS), LEFT_PELVIS,5,2,-1,1,"L upper leg",0.01f,0.8f),
	new RagdollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS), RIGHT_PELVIS,5,2,-1,1,"R upper leg",0.01f,0.8f),
	new RagdollNode(PxQuat(PxPi / 2.0f + 0.2f, Z_AXIS), LEFT_UPPER_LEG,5,1.75,-1,1,"L lower leg",0.01f,0.8f),
	new RagdollNode(PxQuat(PxPi / 2.0f - 0.2f, Z_AXIS), RIGHT_UPPER_LEG,5,1.75,-1,1,"R lower leg",0.01f,0.8f),
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), LOWER_SPINE,1,3,1,-1,"upper spine",0.01f,0.3f),
	new RagdollNode(PxQuat(PxPi, Z_AXIS), UPPER_SPINE,1,1.5,1,1,"left clavicle",0.1f,0.1f),
	new RagdollNode(PxQuat(0, Z_AXIS), UPPER_SPINE,1,1.5,1,1,"right clavicle",0.1f,0.1f),
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), UPPER_SPINE,1,1,1,-1,"neck",0.01f,0.8f),
	new RagdollNode(PxQuat(PxPi / 2.0f, Z_AXIS), NECK,1,3,1,-1, "HEAD",0.3f,0.3f),
	new RagdollNode(PxQuat(PxPi - 0.3, Z_AXIS), LEFT_CLAVICLE,3,1.5,-1,1,"left upper arm",0.5f,0.8f),
	new RagdollNode(PxQuat(0.3, Z_AXIS), RIGHT_CLAVICLE,3,1.5,-1,1,"right upper arm",0.5f,0.8f),
	new RagdollNode(PxQuat(PxPi - 0.3, Z_AXIS), LEFT_UPPER_ARM,3,1,-1,1,"left lower arm",0.01f,0.8f),
	new RagdollNode(PxQuat(0.3, Z_AXIS), RIGHT_UPPER_ARM,3,1,-1,1,"right lower arm",0.01f,0.8f),
	NULL
};

bool Physics::startup()
{
    if (Application::startup() == false)
    {
        return false;
    }
	
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    Gizmos::create();

    m_camera = FlyCamera(1280.0f / 720.0f, 10.0f);
    m_camera.setLookAt(vec3(30, 30, 30), vec3(0,0,0), vec3(0, 1, 0));
    m_camera.sensitivity = 3;

	m_renderer = new Renderer();
	
	SetUpPhysX();
	SetUpVisualDebugger(); 
	SetUpTutorial1();

	muzzleSpeed = 10000.0f;
	shootTimer = 0.0f;
	shootTimeOut = 0.2f;

    return true;
}

void Physics::shutdown()
{
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();
	delete m_renderer;
    Gizmos::destroy();
    Application::shutdown();
}

bool Physics::update()
{
    if (Application::update() == false)
    {
        return false;
    }

    Gizmos::clear();

    float dt = (float)glfwGetTime();
    m_delta_time = dt;
    glfwSetTime(0.0);

    vec4 white(1);
    vec4 black(0, 0, 0, 1);

    for (int i = 0; i <= 20; ++i)
    {
        Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
            i == 10 ? white : black);
        Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
            i == 10 ? white : black);
    }
	Gizmos::addAABBFilled(glm::vec3(0, 0, 0), glm::vec3(500, 0, 500),glm::vec4(0.3,0.3,0.8,1));

    m_camera.update(1.0f / 60.0f);

	UpdatePhysX(m_delta_time);

	renderGizmos(g_PhysicsScene);

	shootTimer += m_delta_time;

	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS && shootTimer >= shootTimeOut)
	{
		shootTimer = 0;
		Shoot();
	}

    return true;
}

void Physics::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
    Gizmos::draw(m_camera.proj, m_camera.view);

    m_renderer->RenderAndClear(m_camera.view_proj);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void AddWidget(PxShape* shape, PxRigidActor* actor, vec4 geo_color)
{
    PxTransform full_transform = PxShapeExt::getGlobalPose(*shape, *actor);
    vec3 actor_position(full_transform.p.x, full_transform.p.y, full_transform.p.z);
    glm::quat actor_rotation(full_transform.q.w,
        full_transform.q.x,
        full_transform.q.y,
        full_transform.q.z);
    glm::mat4 rot(actor_rotation);

    mat4 rotate_matrix = glm::rotate(10.f, glm::vec3(7, 7, 7));

    PxGeometryType::Enum geo_type = shape->getGeometryType();

    switch (geo_type)
    {
    case (PxGeometryType::eBOX) :
    {
        PxBoxGeometry geo;
        shape->getBoxGeometry(geo);
        vec3 extents(geo.halfExtents.x, geo.halfExtents.y, geo.halfExtents.z);
        Gizmos::addAABBFilled(actor_position, extents, geo_color, &rot);
    } break;
    case (PxGeometryType::eCAPSULE) :
    {
        PxCapsuleGeometry geo;
        shape->getCapsuleGeometry(geo);
        Gizmos::addCapsule(actor_position, geo.halfHeight * 2, geo.radius, 16, 16, geo_color, &rot);
    } break;
    case (PxGeometryType::eSPHERE) :
    {
        PxSphereGeometry geo;
        shape->getSphereGeometry(geo);
        Gizmos::addSphereFilled(actor_position, geo.radius, 16, 16, geo_color, &rot);
    } break;
    case (PxGeometryType::ePLANE) :
    {

    } break;
    }
}

void Physics::renderGizmos(PxScene* physics_scene)
{
    PxActorTypeFlags desiredTypes = PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC;
    PxU32 actor_count = physics_scene->getNbActors(desiredTypes);
    PxActor** actor_list = new PxActor*[actor_count];
	physics_scene->getActors(desiredTypes, actor_list, actor_count);
    
    vec4 geo_color(1, 0, 0, 1);
    for (int actor_index = 0;
        actor_index < (int)actor_count;
        ++actor_index)
    {
        PxActor* curr_actor = actor_list[actor_index];
        if (curr_actor->isRigidActor())
        {
            PxRigidActor* rigid_actor = (PxRigidActor*)curr_actor;
            PxU32 shape_count = rigid_actor->getNbShapes();
            PxShape** shapes = new PxShape*[shape_count];
            rigid_actor->getShapes(shapes, shape_count);

            for (int shape_index = 0;
                shape_index < (int)shape_count;
                ++shape_index)
            {
                PxShape* curr_shape = shapes[shape_index];
                AddWidget(curr_shape, rigid_actor, geo_color);
            }

            delete[]shapes;
        }
    }

    delete[] actor_list;

    int articulation_count = physics_scene->getNbArticulations();

    for (int a = 0; a < articulation_count; ++a)
    {
        PxArticulation* articulation;
		physics_scene->getArticulations(&articulation, 1, a);

        int link_count = articulation->getNbLinks();

        PxArticulationLink** links = new PxArticulationLink*[link_count];
        articulation->getLinks(links, link_count);

        for (int l = 0; l < link_count; ++l)
        {
            PxArticulationLink* link = links[l];
            int shape_count = link->getNbShapes();

            for (int s = 0; s < shape_count; ++s)
            {
                PxShape* shape;
                link->getShapes(&shape, 1, s);
                AddWidget(shape, link, geo_color);
            }
        }
        delete[] links;
    }
}

void Physics::SetUpPhysX()
{
	PxAllocatorCallback *myCallback = new myAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback, gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, PxTolerancesScale());
	PxInitExtensions(*g_Physics);
	//create physics material
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -10.0f, 0);
	//&physx::PxDefaultSimulationFilterShader;
	sceneDesc.filterShader = &myFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);
	PxSimulationEventCallback* myCollisionCallBack = new MyCollisionCallBack;
	g_PhysicsScene->setSimulationEventCallback(myCollisionCallBack);
}

void Physics::UpdatePhysX(float a_deltaTime)
{
	if (a_deltaTime <= 0)
	{
		return;
	}
	g_PhysicsScene->simulate(a_deltaTime);
	while (g_PhysicsScene->fetchResults() == false)
	{

	}
}

void Physics::SetUpVisualDebugger()
{
	//check if PvdConnection manager is available on this platform
	if (g_Physics->getPvdConnectionManager() == NULL)
		return;
	//setup connection parameters
	const char* pvd_host_ip = "127.0.0.1";
	//IP of the PC which is running PVD
	int port = 5425;
	//TCP port to connect to, where PVD is listening
	unsigned int timeout = 100;
	//timeout in milliseconds to wait for PVD to respond, consoles and remote PCs need a higher timeout.
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	//and now try to connect PxVisualDebuggerExt
	auto theConnection = PxVisualDebuggerExt::createConnection(g_Physics->getPvdConnectionManager(), pvd_host_ip, port, timeout, connectionFlags);
}

void Physics::SetUpTutorial1()
{
	//Add a plane
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi*1.0f, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, PxPlaneGeometry(), *g_PhysicsMaterial);
	//Add it to the physx scene
	g_PhysicsScene->addActor(*plane);
	//Add a box
	float density = 5000;
	PxBoxGeometry box(5,5,5);
	PxTransform transformS(PxVec3(0, 5, 0));
	PxRigidStatic* StaticActor = PxCreateStatic(*g_Physics, transformS, box, *g_PhysicsMaterial);
	StaticActor->setName("TriggerBox");
	setupFiltering(StaticActor, FilterGroup::eGROUND, FilterGroup::ePLAYER);
	setShapeAsTrigger(StaticActor);
	//add it to the physx scene
	g_PhysicsScene->addActor(*StaticActor);
}

void Physics::Shoot()
{
	float densityS = 100;
	PxSphereGeometry sphere(1);
	PxTransform transformS(PxVec3(m_camera.world[3].x, m_camera.world[3].y, m_camera.world[3].z));
	PxRigidDynamic* dynamicActorS = PxCreateDynamic(*g_Physics, transformS, sphere, *g_PhysicsMaterial, densityS);
	dynamicActorS->setName("Bullet");
	setupFiltering(dynamicActorS, FilterGroup::ePLAYER, FilterGroup::eGROUND);
	//add it to the physx scene
	g_PhysicsScene->addActor(*dynamicActorS);
	dynamicActorS->addForce(PxVec3(-m_camera.world[2].x, -m_camera.world[2].y, -m_camera.world[2].z)*muzzleSpeed, PxForceMode::eIMPULSE, true);
}

PxArticulation* Physics::makeRagdoll(PxPhysics* g_Physics, RagdollNode** nodeArray, PxTransform worldPos, float scaleFactor, PxMaterial* ragdollMaterial)
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
		if (currentNodePtr->parentNodeIdx != -1)
		{
			//if there is a parent then we need to work out our local position for the link
			//get a pointer to the parent node
			parentNode = *(nodeArray + currentNodePtr->parentNodeIdx);
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
		//create the link in the articulation
		PxArticulationLink* link = articulation->createLink(parentLinkPtr, linkTransform);
		//add the pointer to this link into the ragdoll data so we have it for later when we want to link to it
		currentNodePtr->linkPtr = link;
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
			joint->setSwingLimit(currentNodePtr->ySwingLimit, currentNodePtr->zSwingLimit);
			joint->setSwingLimitEnabled(true);
			joint->setTwistLimit(-0.1f, 0.1f);
			joint->setTwistLimitEnabled(true);
		}
		currentNode++;
	}
	return articulation;
}

void MyCollisionCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];
		//only interested in touches found and lost
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			std::cout << "Collision Detected between: ";
			std::cout << pairHeader.actors[0]->getName();
			std::cout << " and ";
			std::cout << pairHeader.actors[1]->getName() << std::endl;
		}
	}
}

void MyCollisionCallBack::onTrigger(PxTriggerPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		PxTriggerPair* pair = pairs + i;
		PxActor* triggerActor = pair->triggerActor;
		PxActor* otherActor = pair->otherActor;
		std::cout << otherActor->getName();
		std::cout << " Entered Trigger ";
		std::cout << triggerActor->getName() << std::endl;
	}
}

PxFilterFlags myFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	//let tirggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	//generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) &&
		(filterData1.word0 & filterData0.word1))
	{
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST;
	}

	return PxFilterFlag::eDEFAULT;
}

void  setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; //word0 = own ID
	filterData.word1 = filterMask; //word1 = ID mask to filter pairs that trigger a contact callback
	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = (PxShape**)_aligned_malloc(sizeof(PxShape*)*numShapes, 16);
	actor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	_aligned_free(shapes);
}

void Physics::setShapeAsTrigger(PxRigidActor* actorIn)
{
	PxRigidStatic* staticActor = actorIn->is<PxRigidStatic>();
	assert(staticActor);

	const PxU32 numShapes = staticActor->getNbShapes();
	PxShape** shapes = (PxShape**)_aligned_malloc(sizeof(PxShape*)*numShapes, 16);
	staticActor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		shapes[i]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shapes[i]->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
}