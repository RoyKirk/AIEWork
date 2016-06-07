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
	m_physics->timeStep = 0.01f;

	newBall = new Sphere(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), 2000.0f, 1, glm::vec4(1, 0, 0, 1));
	m_physics->addActor(newBall);


    return true;
}

void Physics::shutdown()
{
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
	
	m_physics->update();
	m_physics->addGizmos();

    float dt = (float)glfwGetTime();
    m_delta_time = dt;
    glfwSetTime(0.0);

	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		float massreduction = 10;
		if (newBall->mass > massreduction)
		{			
			Sphere *newBall2;
			newBall2 = new Sphere(glm::vec3(newBall->position.x, newBall->position.y - 1, 0), glm::vec3(0, 0, 0), massreduction, 0.1, glm::vec4(0, 1, 0, 1));
			m_physics->addActor(newBall2);
			newBall->applyForceToActor(newBall2, glm::vec2(0, 10));
			newBall->applyForce(glm::vec2(0, 100));
			newBall->mass -= massreduction;
		}
	}

	//if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
	//{
	//	newBall->applyForceToActor(newBall2, glm::vec2(-10, 0));
	//}
	//if (glfwGetKey(m_window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
	//{
	//	newBall->applyForceToActor(newBall2, glm::vec2(10, 0));
	//}

    //vec4 white(1);
    //vec4 black(0, 0, 0, 1);

    //for (int i = 0; i <= 20; ++i)
    //{
    //    Gizmos::addLine(vec3(-10 + i, -0.01, -10), vec3(-10 + i, -0.01, 10),
    //        i == 10 ? white : black);
    //    Gizmos::addLine(vec3(-10, -0.01, -10 + i), vec3(10, -0.01, -10 + i),
    //        i == 10 ? white : black);
    //}

    m_camera.update(1.0f / 60.0f);


    return true;
}

void Physics::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	Gizmos::draw(m_camera.proj, m_camera.view);
	//Gizmos::draw(glm::ortho<float>(-100, 100,-100 / (16/9), 100 / (16 / 9),-1.0f, 1.0f));
    m_renderer->RenderAndClear(m_camera.view_proj);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
