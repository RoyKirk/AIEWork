#include "TestApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Camera.h"
#include "Gizmos.h"
#include <math.h>

#include "Planet.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

float Planet1Dis = 15;
float Planet2Dis = 2;
float Planet3Dis = 1;

vec3 SunPos = vec3(0);
vec3 Planet1Pos = vec3(0);
vec3 Planet2Pos = vec3(0);
vec3 Planet3Pos = vec3(0);

Planet* Sun = new Planet()

float Planet1OrbitRot = 0;
float Planet2OrbitRot = 0;
float Planet3OrbitRot = 0;

float Planet1RotSpeed = 0.5;
float Planet2RotSpeed = 1;
float Planet3RotSpeed = 2;

mat4 SunLocal = mat4(1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					SunPos.x, SunPos.y, SunPos.z, 1);

mat4 Planet1Local = mat4(1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						Planet1Pos.x, Planet1Pos.y, Planet1Pos.z, 1);

mat4 Planet2Local = mat4(1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						Planet2Pos.x, Planet2Pos.y, Planet2Pos.z, 1);
mat4 Planet3Local = mat4(1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						Planet3Pos.x, Planet3Pos.y, Planet3Pos.z, 1);


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
	m_camera = new Camera(glm::pi<float>() * 0.35f, 16 / 9.f, 0.01f, 1000.f);
	m_camera->setLookAtFrom(vec3(30, 20, 30), vec3(0));
	
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
	Planet2OrbitRot += deltaTime * Planet2RotSpeed;
	Planet3OrbitRot += deltaTime * Planet3RotSpeed;

	Planet1Local = glm::rotate(Planet1OrbitRot, vec3(0, 1, 0));
	Planet1Local = glm::translate(Planet1Local, vec3(Planet1Dis, 0, 0));
	Planet2Local = glm::rotate(Planet1Local, Planet2OrbitRot, vec3(0, 1, 0));
	Planet2Local = glm::translate(Planet2Local, vec3(Planet2Dis, 0, 0));
	Planet3Local = glm::rotate(Planet2Local, Planet3OrbitRot, vec3(0, 1, 0));
	Planet3Local = glm::translate(Planet3Local, vec3(Planet3Dis, 0, 0));
	
	SunPos = vec3(SunLocal[3].x, SunLocal[3].y, SunLocal[3].z);
	Planet1Pos = vec3(Planet1Local[3].x, Planet1Local[3].y, Planet1Local[3].z);
	Planet2Pos = vec3(Planet2Local[3].x, Planet2Local[3].y, Planet2Local[3].z);
	Planet3Pos = vec3(Planet3Local[3].x, Planet3Local[3].y, Planet3Local[3].z);	

	Gizmos::addSphere(SunPos, 3, 10, 10, vec4(1, 0, 1, 1));
	Gizmos::addSphere(Planet1Pos, 0.5, 10, 10, vec4(1, 0, 0, 1));
	Gizmos::addSphere(Planet2Pos, 0.2, 10, 10, vec4(0, 1, 0, 1));
	Gizmos::addSphere(Planet3Pos, 0.1, 10, 10, vec4(0, 0, 1, 1));

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