#include "TestApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Camera.h"
#include "Gizmos.h"

#include <iostream>
#include <fstream>

using glm::vec3;
using glm::vec4;
using glm::mat4;

int rowsSet = 1000;
int colsSet = 1000;
float waveTimer = 0;
float waveTimerLimit = 1;
bool waveDirection = true;

unsigned int m_VAO;
unsigned int m_VBO;
unsigned int m_IBO;

unsigned int m_programID;

struct Vertex
{
	vec4 position;
	vec4 colour;
};

void generateGrid(unsigned int rows, unsigned int cols)
{
	//if (waveDirection)
	//{
	//	waveTimer += deltaTime;
	//	if (waveTimer >= waveTimerLimit)
	//	{
	//		waveDirection = false;
	//	}
	//}
	//else if (!waveDirection)
	//{
	//	waveTimer -= deltaTime;
	//	if (waveTimer <= -waveTimerLimit)
	//	{
	//		waveDirection = true;
	//	}
	//}

	//waveTimer += deltaTime;

	Vertex* aoVertices = new Vertex[rows*cols];
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			float y = sin((float)r / 10)*cos((float)c / 10)*waveTimer;
			//float y = sin((((float)r/10+waveTimer)+3.14)* sin((float)c / 10 + waveTimer)+ 3.14);
			
			//if (y < 0){y = 0;}
			
			aoVertices[r * cols + c].position = vec4((float)c/10, y, (float)r/10, 1);
			//vec3 colour = vec3(sinf((c / (float)(cols - 1))*(r / (float)(rows - 1))));
			vec3 colour = vec3(0.5);
			aoVertices[r*cols + c].colour = vec4(colour,1);
			//aoVertices[r * cols + c].position = vec4((float)c, 0, (float)r, 1);
			//vec3 colour = vec3(sinf((c / (float)(cols - 1))*(r / (float)(rows - 1))));
			//aoVertices[r*cols + c].colour = vec4(colour, 1);
		}
	}

	unsigned int* auiIndices = new unsigned int[(rows - 1)*(cols - 1) * 6];
	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			//Tirangle 1
			auiIndices[index++] = r*cols + c;
			auiIndices[index++] = (r + 1)*cols + c;
			auiIndices[index++] = (r + 1)*cols + (c + 1);
			//Triangle 2
			auiIndices[index++] = r*cols + c;
			auiIndices[index++] = (r + 1)*cols + (c + 1);
			auiIndices[index++] = r*cols + (c + 1);
		}
	}

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	//Vertex Buffer

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (rows*cols)*sizeof(Vertex), aoVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));


	//Index Buffer

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);


	//Vertex Array Object

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	delete[] aoVertices;
	delete[] auiIndices;
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
	m_camera->setLookAtFrom(vec3(0, 20, 0), vec3(25, 0, 25));
	
	//////////////////////////////////////////////////////////////////////////
	// YOUR STARTUP CODE HERE
	//////////////////////////////////////////////////////////////////////////
	m_pickPosition = glm::vec3(0);

	linkShader();

	generateGrid(rowsSet, colsSet);

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

	unsigned int timeUniform = glGetUniformLocation(m_programID, "time");
	waveTimer += deltaTime;
	glUniform1f(timeUniform, waveTimer);

	unsigned int heightScaleUniform = glGetUniformLocation(m_programID, "heightScale");
	glUniform1f(heightScaleUniform,1);

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

	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	glUseProgram(m_programID);
	unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_camera->getProjectionView()));

	


	glBindVertexArray(m_VAO);
	unsigned int indexCount = (rowsSet - 1)*(colsSet - 1) * 6;
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}