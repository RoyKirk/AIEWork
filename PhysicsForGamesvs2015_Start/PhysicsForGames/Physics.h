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



class Physics : public Application
{
public:
	virtual bool startup();
	virtual void shutdown();
    virtual bool update();
    virtual void draw();

	float const m_timeStep = 0.01f;

	float const PI = 3.14159265359f;

	float const simSize = 100.0f;

	float timer;
	float const timerLimit = 1.0f;

	float const MAX_SHOT_POWER = 20.0f;

	glm::vec2 direction;
	float selectedPower;
	bool shot;

	float aspectRatio;

	void projectileMotionSetup();

	void rocketEngineSetup();

	void rocketEngine();

	void collisionDetectionTute();
	void collisionDetectionTuteSetup();

	Sphere *newBall;
	Sphere *whiteBall;
	Plane *newPlane;


	PhysicsScene* m_physics;

    Renderer* m_renderer;
    FlyCamera m_camera;
    float m_delta_time;
};



#endif //CAM_PROJ_H_
