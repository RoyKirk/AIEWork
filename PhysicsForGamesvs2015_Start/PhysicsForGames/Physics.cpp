#include "Physics.h"

#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include "Gizmos.h"

#include "glm/ext.hpp"
#include "glm/gtc/quaternion.hpp"

#define Assert(val) if (val){}else{ *((char*)0) = 0;}
#define ArrayCount(val) (sizeof(val)/sizeof(val[0]))

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
    m_camera.setLookAt(vec3(0, 0, 40), vec3(0), vec3(0, 1, 0));
    m_camera.sensitivity = 3;

	m_renderer = new Renderer();

	m_physics = new PhysicsScene();
	m_physics->gravity = glm::vec3(0, -10, 0);
	m_physics->timeStep = m_timeStep;
	
	simSize = 100.0f;

	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	aspectRatio = (float)width / (float)height;

	//projectileMotionSetup();
	//rocketEngineSetup();
	collisionDetectionTuteSetup();

    return true;
}

void Physics::shutdown()
{
	delete m_renderer;
	delete m_physics;
	delete newBall;
	delete newPlane;
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
	
	m_physics->update();
	m_physics->addGizmos();

    float dt = (float)glfwGetTime();
    m_delta_time = dt;
    glfwSetTime(0.0);
	
	//rocketEngine();

	m_camera.update(1.0f / 60.0f);


    return true;
}

void Physics::draw()
{
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	//Gizmos::draw(m_camera.proj, m_camera.view);
	Gizmos::draw(glm::ortho<float>(-simSize , simSize , -simSize / aspectRatio, simSize / aspectRatio, -1.0f, 1.0f));
	Gizmos::draw2D(glm::ortho<float>(-simSize, simSize, -simSize / aspectRatio, simSize / aspectRatio, -1.0f, 1.0f));
    m_renderer->RenderAndClear(m_camera.view_proj);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void Physics::projectileMotionSetup()
{
	float muzzleSpeed = 25;
	glm::vec3 intialPos = glm::vec3(-25, 0, 0);
	float initialAngle = PI*1/4;
	for (int i = 0; i < 100; i++)
	{
		glm::vec2 tempPos;
		tempPos.x = intialPos.x + muzzleSpeed * ((float)i * m_timeStep) * cos(initialAngle);
		tempPos.y = intialPos.y + muzzleSpeed * ((float)i * m_timeStep) * sin(initialAngle) + 0.5*(-10)*((float)i*m_timeStep)*((float)i*m_timeStep);
		Gizmos::addSphereFilled(glm::vec3(tempPos,0), 1, 6, 6, glm::vec4(1, 0, 1, 1));
	}

	newBall = new Sphere(intialPos, glm::vec3(cos(initialAngle)*muzzleSpeed, sin(initialAngle)*muzzleSpeed,0) , 20.0f, 1, glm::vec4(1, 0, 0, 1));
	m_physics->addActor(newBall);
}

void Physics::rocketEngineSetup()
{
	newBall = new Sphere(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), 200.0f, 1, glm::vec4(1, 0, 0, 1));
	m_physics->addActor(newBall);
}

void Physics::rocketEngine()
{
	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		float massreduction = 10;
		if (newBall->mass > massreduction)
		{
			newBall = new Sphere(glm::vec3(newBall->position.x, newBall->position.y - 1, 0), glm::vec3(0, 0, 0), massreduction, 0.1, glm::vec4(0, 1, 0, 1));
			m_physics->addActor(newBall);
			newBall->applyForceToActor(newBall, glm::vec2(0, 10));
			newBall->applyForce(glm::vec2(0, 100));
			//newBall->mass -= massreduction;
		}
	}

	if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		float massreduction = 10;
		if (newBall->mass > massreduction)
		{
			newBall = new Sphere(glm::vec3(newBall->position.x + 1, newBall->position.y - 1, 0), glm::vec3(0, 0, 0), massreduction, 0.1, glm::vec4(0, 1, 0, 1));
			m_physics->addActor(newBall);
			newBall->applyForceToActor(newBall, glm::vec2(0, 10));
			newBall->applyForce(glm::vec2(-50, 0));
			//newBall->mass -= massreduction;
		}
	}
	if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		float massreduction = 10;
		if (newBall->mass > massreduction)
		{
			newBall = new Sphere(glm::vec3(newBall->position.x - 1, newBall->position.y - 1, 0), glm::vec3(0, 0, 0), massreduction, 0.1, glm::vec4(0, 1, 0, 1));
			m_physics->addActor(newBall);
			newBall->applyForceToActor(newBall, glm::vec2(0, 10));
			newBall->applyForce(glm::vec2(50, 0));
			//newBall->mass -= massreduction;
		}
	}

	if (m_physics->actors.size() > 10)
	{
		m_physics->actors.erase(m_physics->actors.begin() + 1);
	}
}

void Physics::collisionDetectionTuteSetup()
{
	//float muzzleSpeed = 25;
	//glm::vec3 intialPos = glm::vec3(-25, 0, 0);
	//float initialAngle = PI * 1 / 4;
	newBall = new Sphere(glm::vec3(-25, 0, 0), glm::vec3(30, 0, 0), 20.0f, 10, glm::vec4(1, 0, 0, 1));
	m_physics->addActor(newBall);
	newBall = new Sphere(glm::vec3(-50, 30, 0), glm::vec3(30, 0, 0), 20.0f,15, glm::vec4(1, 0, 0, 1));
	m_physics->addActor(newBall);
	newPlane = new Plane(glm::vec2(1, 0), simSize, glm::vec4(1, 0, 0, 1));
	m_physics->addActor(newPlane);
	newPlane = new Plane(glm::vec2(-1, 0), simSize, glm::vec4(1, 0, 0, 1));
	m_physics->addActor(newPlane);
	newPlane = new Plane(glm::vec2(0, 1), simSize/aspectRatio, glm::vec4(1, 0, 0, 1));
	m_physics->addActor(newPlane);
	newPlane = new Plane(glm::vec2(0, -1), simSize/aspectRatio, glm::vec4(1, 0, 0, 1));
	m_physics->addActor(newPlane);
}

void Physics::collisionDetectionTute()
{
	/*if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		int width = 0, height = 0;
		glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
		float aspectRatio = width / (float)height;
		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		glm::vec3 screenCoord(mouseX, mouseY, 0);
		float fSize = 50;
		float farPlane = 200;
		glm::vec4 viewPort = glm::vec4(0.f, 0.f, width, height);
		glm::vec3 worldPos = glm::unProject(screenCoord, glm::mat4(), projectionMatrix, viewPort);
		worldPos.y *= -1;
		worldPos.z = 1;
		glm::vec3 endPos = glm::vec3(whiteBall->position, 0);
		direction = worldPos - endPos;
		selectedPower = glm::length(direction);
		direction = glm::normalize(direction);
		if (selectedPower>30)
		{
			selectedPower = 30;
		}
		glm::vec3 finalEnd = endPos + direction* selectedPower;
		Gizmos::add2DLine(whiteBall->position, worldPos.xy, glm::vec4(1, 1, 1, 1));
	}*/
}