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

float SunX = 0;
float SunY = 0;
float SunZ = 0;
float Planet1X = 6;
float Planet2X = 10;
float Planet3X = 15;
float Planet1Y = 0;
float Planet2Y = 0;
float Planet3Y = 0;
float Planet1Z = 6;
float Planet2Z = 10;
float Planet3Z = 15;
vec3 SunPos = vec3(SunX, SunY, SunZ);
vec3 Planet1Pos = vec3(Planet1X, Planet1Y, Planet1Z);
vec3 Planet2Pos = vec3(Planet2X, Planet2Y, Planet2Z);
vec3 Planet3Pos = vec3(Planet3X, Planet3Y, Planet3Z);

mat4 Planet1Rot = mat4(cos(5.0), 0, sin(5.0), 0, 0, 1, 0, 0, -(sin(5.0)), 0, cos(5.0), 0, 0, 0, 0, 0);


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
	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
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
	
	vec4 SunPos4 = vec4(SunPos.x, SunPos.y, SunPos.z,0);
	vec4 Planet1Pos4 = vec4(Planet1Pos.x, Planet1Pos.y, Planet1Pos.z,0);
	vec4 Planet2Pos4 = vec4(Planet2Pos.x, Planet2Pos.y, Planet2Pos.z,0);
	vec4 Planet3Pos4 = vec4(Planet3Pos.x, Planet3Pos.y, Planet3Pos.z,0);

	//Planet1Pos4 = Planet1Pos4 * Planet1Rot;
	
	vec3 SunPos = vec3(SunPos4.x, SunPos4.y, SunPos4.z);
	vec3 Planet1Pos = vec3(Planet1Pos4.x, Planet1Pos4.y, Planet1Pos4.z);
	vec3 Planet2Pos = vec3(Planet2Pos4.x, Planet2Pos4.y, Planet2Pos4.z);
	vec3 Planet3Pos = vec3(Planet3Pos4.x, Planet3Pos4.y, Planet3Pos4.z);

	Gizmos::addSphere(SunPos, 3, 10, 10, vec4(1, 0, 1, 1));
	Gizmos::addSphere(Planet1Pos, 0.5, 10, 10, vec4(1, 0, 0, 1));
	Gizmos::addSphere(Planet2Pos, 1, 10, 10, vec4(0, 1, 0, 1));
	Gizmos::addSphere(Planet3Pos, 1.5, 10, 10, vec4(0, 0, 1, 1));

	//SunPos = SunPos;
	//Planet1Pos = Planet1Pos;
	//Planet2Pos = Planet2Pos;
	//Planet3Pos = Planet3Pos;

	//Planet1X = SunX + ((cos(4.0f)) * (Planet1X - SunX) + (sin(4.0f)) * (Planet1Z - SunZ));
	//Planet1Z = SunZ + ((sin(4.0f)) * (Planet1X - SunX) + (cos(4.0f)) * (Planet1Z - SunZ));

	//Planet2X = SunX + ((cos(4.0f)) * (Planet2X - SunX) + (sin(4.0f)) * (Planet2Z - SunZ));
	//Planet2Z = SunZ + ((sin(4.0f)) * (Planet2X - SunX) + (cos(4.0f)) * (Planet2Z - SunZ));

	//Planet3X = SunX + ((cos(4.0f)) * (Planet3X - SunX) + (sin(4.0f)) * (Planet3Z - SunZ));
	//Planet3Z = SunZ + ((sin(4.0f)) * (Planet3X - SunX) + (cos(4.0f)) * (Planet3Z - SunZ));

	//Planet1X = SunX + (Planet1X - SunX)*cos(1.0f) - (Planet1Z - SunZ)*sin(1.0f);
	//Planet1Z = SunZ + (Planet1X - SunX)*sin(1.0f) + (Planet1Z - SunZ)*cos(1.0f);



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