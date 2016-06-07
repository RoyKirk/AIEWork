#ifndef SOUND_PROGRAMMING_H_
#define SOUND_PROGRAMMING_H_

#include "Application.h"
#include "Camera.h"
#include "Render.h"

#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "RigidBody.h"
#include "Plane.h"
#include "Box.h"
#include "Sphere.h"


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

	float m_timeStep = 0.01f;

	float const PI = 3.14159265359f;

	void projectileMotionSetup();

	void rocketEngineSetup();

	void rocketEngine();

	void mousePush();

	//void renderGizmos(PhysicsScene* physics_scene);

	Sphere *newBall;
	

	PhysicsScene* m_physics;

    Renderer* m_renderer;
    FlyCamera m_camera;
    float m_delta_time;
};



#endif //CAM_PROJ_H_
