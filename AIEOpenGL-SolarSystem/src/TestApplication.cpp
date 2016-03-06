#include "TestApplication.h"
#include "gl_core_4_4.h"

#define GLM_SWIZZLE

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

#include "Camera.h"
#include "Gizmos.h"
#include <math.h>

#include "Planet.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

int celestBodNum = 5;
float celestBodSize = 2;
float celestBodDist = 30;
float celestBodSpeed = 0.5;

//Planet* Sun = new Planet(nullptr, 0, 0);
//Planet* Planet1 = new Planet(Sun, 15, 0.5);
//Planet* Planet2 = new Planet(Planet1, 2, 1);
//Planet* Planet3 = new Planet(Planet2, 1, 2);

Planet** celestBod = new Planet*[celestBodNum];

class BoundingSphere
{
public:
	BoundingSphere() : centre(0), radius(0) {}
	~BoundingSphere() {}

	void fit(const std::vector<glm::vec3>& points) {

		glm::vec3 min(1e37f), max(-1e37f);

		for (auto& p : points)
		{
			if (p.x < min.x) { min.x = p.x; }
			if (p.y < min.y) { min.y = p.y; }
			if (p.z < min.z) { min.z = p.z; }
			if (p.x > max.x) { max.x = p.x; }
			if (p.y > max.y) { max.y = p.y; }
			if (p.z > max.z) { max.z = p.z; }
		}

		centre = (min + max)*0.5f;
		radius = glm::distance(min, centre);
	}
	glm::vec3 centre;
	float radius;
};

void getFrustumPlanes(const glm::mat4& transform, glm::vec4* planes)
{
	//right side
	planes[0] = vec4(transform[0][3] - transform[0][0],
		transform[1][3] - transform[1][0],
		transform[2][3] - transform[2][0],
		transform[3][3] - transform[3][0]);
	//left side
	planes[1] = vec4(transform[0][3] + transform[0][0],
		transform[1][3] + transform[1][0],
		transform[2][3] + transform[2][0],
		transform[3][3] + transform[3][0]);

	//top
	planes[2] = vec4(transform[0][3] - transform[0][1],
		transform[1][3] - transform[1][1],
		transform[2][3] - transform[2][1],
		transform[3][3] - transform[3][1]);
	//bottom
	planes[3] = vec4(transform[0][3] + transform[0][1],
		transform[1][3] + transform[1][1],
		transform[2][3] + transform[2][1],
		transform[3][3] + transform[3][1]);
	//far
	planes[4] = vec4(transform[0][3] - transform[0][2],
		transform[1][3] - transform[1][2],
		transform[2][3] - transform[2][2],
		transform[3][3] - transform[3][2]);
	//near
	planes[5] = vec4(transform[0][3] + transform[0][2],
		transform[1][3] + transform[1][2],
		transform[2][3] + transform[2][2],
		transform[3][3] + transform[3][2]);

	for (int i = 0; i < 6; i++)
	{
		planes[i] = glm::normalize(planes[i]);
	}
}


TestApplication::TestApplication()
	: m_camera(nullptr) {

}

TestApplication::~TestApplication() {

}

bool TestApplication::startup() {
	
	
	//Solar System list build
	for (int i = 0; i <= celestBodNum; i++)
	{
		if (i == 0)
		{
			celestBod[i] = new Planet(nullptr, 0, 0, celestBodSize*1.5);
		}
		else
		{
			celestBod[i] = new Planet(celestBod[i - 1], celestBodDist / (2*i), celestBodSpeed * i, celestBodSize / i);
		}
	}


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
	
	for (int i = 0; i <= celestBodNum; i++)
	{
		if (celestBod[i]->m_parent == nullptr)
		{
			celestBod[i]->m_local = glm::rotate(celestBod[i]->m_rotation, vec3(0, 1, 0));
			celestBod[i]->m_local = glm::translate(celestBod[i]->m_local, vec3(celestBod[i]->m_distance, 0, 0));
			
		}
		else
		{
			celestBod[i]->m_rotation += deltaTime * celestBod[i]->m_orbitSpeed;
			celestBod[i]->m_local = glm::rotate(celestBod[i]->m_parent->m_local, celestBod[i]->m_rotation, vec3(0, 1, 0));
			celestBod[i]->m_local = glm::translate(celestBod[i]->m_local, vec3(celestBod[i]->m_distance, 0, 0));
		}
		BoundingSphere sphere;
		sphere.centre = celestBod[i]->m_local[3].xyz();
		sphere.radius = celestBod[i]->m_size;

		vec4 planes[6];
		getFrustumPlanes(m_camera->getProjectionView(), planes);

		for (int i = 0; i < 6; i++)
		{
			float d = glm::dot(vec3(planes[i]), sphere.centre) + planes[i].w;
		}

		for (int i = 0; i < 6; i++)
		{
			float d = glm::dot(vec3(planes[i]), sphere.centre) + planes[i].w;
			if (d < -sphere.radius)
			{
				printf("Behind, don't render it!\n");
				break;
			}
			else if (d < sphere.radius)
			{
				printf("Touching, we still need to render it!\n");
				Gizmos::addSphere(sphere.centre, sphere.radius, 10, 10, vec4(1, 0, 1, 1));
				continue;
			}
			else
			{
				printf("Front, fully visible so render it!\n");
				Gizmos::addSphere(sphere.centre, sphere.radius, 10, 10, vec4(1, 0, 1, 1));
				continue;
			}
		}
	}



	//Planet1->m_rotation += deltaTime * Planet1->m_orbitSpeed;
	//Planet2->m_rotation += deltaTime * Planet2->m_orbitSpeed;
	//Planet3->m_rotation += deltaTime * Planet3->m_orbitSpeed;

	//Sun->m_local = glm::rotate(Sun->m_rotation, vec3(0, 1, 0));
	//Sun->m_local = glm::translate(Sun->m_local, vec3(Sun->m_distance, 0, 0));

	//Planet1->m_local = glm::rotate(Planet1->m_parent->m_local, Planet1->m_rotation, vec3(0, 1, 0));
	//Planet1->m_local = glm::translate(Planet1->m_local, vec3(Planet1->m_distance, 0, 0));

	//Planet2->m_local = glm::rotate(Planet1->m_local, Planet2->m_rotation, vec3(0, 1, 0));
	//Planet2->m_local = glm::translate(Planet2->m_local, vec3(Planet2->m_distance, 0, 0));

	//Planet3->m_local = glm::rotate(Planet2->m_local, Planet3->m_rotation, vec3(0, 1, 0));
	//Planet3->m_local = glm::translate(Planet3->m_local, vec3(Planet3->m_distance, 0, 0));

	//Gizmos::addSphere(Sun->m_local[3].xyz(), 3, 10, 10, vec4(1, 0, 1, 1));
	//Gizmos::addSphere(Planet1->m_local[3].xyz(), 0.5, 10, 10, vec4(1, 0, 0, 1));
	//Gizmos::addSphere(Planet2->m_local[3].xyz(), 0.2, 10, 10, vec4(0, 1, 0, 1));
	//Gizmos::addSphere(Planet3->m_local[3].xyz(), 0.1, 10, 10, vec4(0, 0, 1, 1));

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