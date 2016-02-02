#include "TestApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Camera.h"
#include "Gizmos.h"
#include <math.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;

vec3 SunPos = vec3(0, 0, 0);
vec3 Planet1Pos = vec3(6, 0, 6);
vec3 Planet2Pos = vec3(10, 0, 10);
vec3 Planet3Pos = vec3(15, 0, 15);

vec4 SunPos4 = vec4(SunPos.x, SunPos.y, SunPos.z, 1);
vec4 Planet1Pos4 = vec4(Planet1Pos.x, Planet1Pos.y, Planet1Pos.z, 1);
vec4 Planet2Pos4 = vec4(Planet2Pos.x, Planet2Pos.y, Planet2Pos.z, 1);
vec4 Planet3Pos4 = vec4(Planet3Pos.x, Planet3Pos.y, Planet3Pos.z, 1);

float Planet1OrbitRot = 0;
float Planet2OrbitRot = 0;
float Planet3OrbitRot = 0;

float Planet1RotSpeed = 0.02;
float Planet2RotSpeed = 0.01;
float Planet3RotSpeed = 0.005;

mat4 Planet1Rot = mat4(1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 1);

mat4 Planet2Rot = mat4(cos(Planet2RotSpeed), 0, sin(Planet2RotSpeed), 0,
	0, 1, 0, 0,
	-(sin(Planet2RotSpeed)), 0, cos(Planet2RotSpeed), 0,
	0, 0, 0, 1);

mat4 Planet3Rot = mat4(cos(Planet3RotSpeed), 0, sin(Planet3RotSpeed), 0,
	0, 1, 0, 0,
	-(sin(Planet3RotSpeed)), 0, cos(Planet3RotSpeed), 0,
	0, 0, 0, 1);


TestApplication::TestApplication()
	: m_camera(nullptr) {

}

TestApplication::~TestApplication() {

}

bool TestApplication::startup() {

	// create a basic window
	createWindow("AIE OpenGL Application", 1280, 720);

	// start the gizmo system that can draw basic shapes
	Gizmos::create();

	// create a camera
	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.01f, 1000.f);
	m_camera->setLookAtFrom(vec3(10, 10, 10), vec3(0));
	
	//////////////////////////////////////////////////////////////////////////
	// YOUR STARTUP CODE HERE
	//////////////////////////////////////////////////////////////////////////
	m_pickPosition = glm::vec3(0);

	return true;
}

void TestApplication::shutdown() {

	//////////////////////////////////////////////////////////////////////////
	// YOUR SHUTDOWN CODE HERE
	//////////////////////////////////////////////////////////////////////////

	// delete our camera and cleanup gizmos
	delete m_camera;
	Gizmos::destroy();

	// destroy our window properly
	destroyWindow();
}

bool TestApplication::update(float deltaTime) {
	
	// close the application if the window closes
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	// update the camera's movement
	m_camera->update(deltaTime);

	// clear the gizmos out for this frame
	Gizmos::clear();

	//////////////////////////////////////////////////////////////////////////
	// YOUR UPDATE CODE HERE
	//////////////////////////////////////////////////////////////////////////

	// an example of mouse picking
	if (glfwGetMouseButton(m_window, 0) == GLFW_PRESS) {
		double x = 0, y = 0;
		glfwGetCursorPos(m_window, &x, &y);

		// plane represents the ground, with a normal of (0,1,0) and a distance of 0 from (0,0,0)
		glm::vec4 plane(0, 1, 0, 0);
		m_pickPosition = m_camera->pickAgainstPlane((float)x, (float)y, plane);
	}
	Gizmos::addTransform(glm::translate(m_pickPosition));

	// ...for now let's add a grid to the gizmos
	for (int i = 0; i < 51; ++i) {
		Gizmos::addLine(vec3(-25 + i, 0, 25), vec3(-25 + i, 0, -25),
			i == 25 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));

		Gizmos::addLine(vec3(25, 0, -25 + i), vec3(-25, 0, -25 + i),
			i == 25 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));
	}
	
	Planet1OrbitRot += deltaTime * Planet1RotSpeed;

	Planet1Rot = glm::rotate(Planet1OrbitRot, vec3(0, 1, 0));
	//Planet1Rot = glm::translate(Planet1Rot, vec3(1, 0, 0));
	



	Planet1Pos4 = Planet1Rot * Planet1Pos4;
	//Planet2Pos4 = (Planet2Rot * Planet1Rot) * Planet2Pos4;
	//Planet2Pos4 = Planet1Rot * Planet2Pos4;
	//Planet3Pos4 = Planet3Rot * Planet3Pos4;
	
	SunPos = vec3(SunPos4.x, SunPos4.y, SunPos4.z);
	Planet1Pos = vec3(Planet1Pos4.x, Planet1Pos4.y, Planet1Pos4.z);
	Planet2Pos = vec3(Planet2Pos4.x, Planet2Pos4.y, Planet2Pos4.z);
	Planet3Pos = vec3(Planet3Pos4.x, Planet3Pos4.y, Planet3Pos4.z);	

	Gizmos::addSphere(SunPos, 3, 10, 10, vec4(1, 0, 1, 1));
	Gizmos::addSphere(Planet1Pos, 0.5, 10, 10, vec4(1, 0, 0, 1));
	Gizmos::addSphere(Planet2Pos, 1, 10, 10, vec4(0, 1, 0, 1));
	Gizmos::addSphere(Planet3Pos, 1.5, 10, 10, vec4(0, 0, 1, 1));

	// return true, else the application closes
	return true;
}

void TestApplication::draw() {

	// clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//////////////////////////////////////////////////////////////////////////
	// DRAW YOUR THINGS HERE
	//////////////////////////////////////////////////////////////////////////

	// display the 3D gizmos
	Gizmos::draw(m_camera->getProjectionView());

	// get a orthographic projection matrix and draw 2D gizmos
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0, 0, (float)width, (float)height);

	Gizmos::draw2D(m_camera->getProjectionView());
}