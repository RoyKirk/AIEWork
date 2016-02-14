#include "TestApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Camera.h"
#include "Gizmos.h"

#include <iostream>
#include <fstream>

using glm::vec3;
using glm::vec4;
using glm::mat4;


unsigned int m_VAO;
unsigned int m_VBO;
unsigned int m_IBO;

unsigned int m_programID;

int imageWidth = 0, imageHeight = 0, imageFormat = 0;

unsigned char* data = stbi_load("./textures/crate.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);

unsigned int m_texture;

struct Vertex
{
	vec4 position;
	vec4 colour;
};

void generateGrid()
{
	float vertexData[] = {
		-5,0,5,1,0,1,
		5,0,5,1,1,1,
		5,0,-5,1,1,0,
		-5,0,-5,1,0,0
	};

	unsigned int indexData[] = {
		0,1,2,
		0,2,3
	};

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	//Vertex Buffer

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 *4, vertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0)+16);


	//Index Buffer

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indexData, GL_STATIC_DRAW);


	//Vertex Array Object

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}


char* loadShader(char* filename)
{


	std::ifstream file;
	file.open(filename, std::ios::in); // opens as ASCII!
	assert(file);
	

	unsigned long pos = file.tellg();
	file.seekg(0, std::ios::end);
	unsigned long len = file.tellg();
	file.seekg(std::ios::beg);

	assert(len != 0);

	char* shaderSource = new char[len + 1];

	assert(shaderSource != 0);

	shaderSource[len] = 0;

	unsigned int i = 0;
	while (file.good())
	{
		shaderSource[i] = file.get();
		if (!file.eof())
			i++;
	}

	shaderSource[i] = 0;  // 0-terminate it at the correct position

	file.close();

	return shaderSource;
}

void linkShader()
{
	const char* vsSource = loadShader("./src/Shader.vert");
	const char* fsSource = loadShader("./src/Shader.frag");
	
	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void textureLoad()
{
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);
}

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
	m_camera->setLookAtFrom(vec3(0, 2, 0), vec3(1, 1, 1));
	
	//////////////////////////////////////////////////////////////////////////
	// YOUR STARTUP CODE HERE
	//////////////////////////////////////////////////////////////////////////
	m_pickPosition = glm::vec3(0);

	linkShader();

	generateGrid();

	textureLoad();

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

	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	
	glUseProgram(m_programID);
	unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, &(m_camera->getProjectionView()[0][0]));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	projectionViewUniform = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(projectionViewUniform, 0);
	


	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}