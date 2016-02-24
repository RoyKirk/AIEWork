#include "TestApplication.h"
#include "gl_core_4_4.h"

#define GLM_SWIZZLE

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "FBXFile.h"

#include <stb_image.h>

#include "Camera.h"
#include "Gizmos.h"
#include <iostream>
#include <fstream>
#include <vector>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

unsigned int m_program;

int imageWidth = 0, imageHeight = 0, imageFormat = 0;

unsigned int m_texture, m_normalmap, m_specularmap;
unsigned char* data;

unsigned int dimension = 64;

unsigned int m_VAO;
unsigned int m_VBO;
unsigned int m_IBO;

float* perlin_data = new float[dimension * dimension];

float height = 50;

float m_timer;

struct Vertex
{
	vec4 position;
	vec2 texCoord;
	vec4 normal;
};


void createOpenGLBuffers(unsigned int dimension)
{
		Vertex* aoVertices = new Vertex[dimension*dimension];
		for (unsigned int r = 0; r < dimension; ++r)
		{
			for (unsigned int c = 0; c < dimension; ++c)
			{
				//float hp = perlin_data[r*dimension + c] * height;
				aoVertices[r * dimension + c].position = vec4((float)c, 0, (float)r, 1);
				aoVertices[r*dimension + c].texCoord = vec2((float)r / dimension, (float)c / dimension);
				aoVertices[r * dimension + c].normal = vec4(normalize(vec3(1)), 0);
				//	aoVertices[r * dimension + c].normal.w = 1;
				//if (c == 0 || r == 0 || c == dimension || r == dimension)
				//{
				//	aoVertices[r * dimension + c].normal = vec4(0, 0, 0, 0);
				//}
				//else
				//{
				//	vec3 c1 = aoVertices[r* dimension + c].position.xyz - aoVertices[(r - 1)* dimension + c].position.xyz;
				//	vec3 c2 = aoVertices[r* dimension + c].position.xyz - aoVertices[r * dimension + c - 1].position.xyz;
				//	vec3 d1 = glm::cross(c1, c2);
				//	aoVertices[r * dimension + c].normal.xyz = normalize(d1);
				//	aoVertices[r * dimension + c].normal.w = 1;
				//}
			}
		}

		unsigned int* auiIndices = new unsigned int[(dimension - 1)*(dimension - 1) * 6];
		unsigned int index = 0;
		for (unsigned int r = 0; r < (dimension - 1); ++r)
		{
			for (unsigned int c = 0; c < (dimension - 1); ++c)
			{
				//Tirangle 1
				auiIndices[index++] = r*dimension + c;
				auiIndices[index++] = (r + 1)*dimension + c;
				auiIndices[index++] = (r + 1)*dimension + (c + 1);
				//Triangle 2
				auiIndices[index++] = r*dimension + c;
				auiIndices[index++] = (r + 1)*dimension + (c + 1);
				auiIndices[index++] = r*dimension + (c + 1);
			}
		}

		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_IBO);

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);
		//Vertex Buffer

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, (dimension*dimension)*sizeof(Vertex), aoVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(sizeof(vec4)+sizeof(vec2)));


		//Index Buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (dimension - 1) * (dimension - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);


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

	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	glLinkProgram(m_program);

	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_program, infoLogLength, 0, infoLog);
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
void textureLoad()
{
	float scale = (1.0f / dimension) * 3;
	int octaves = 8;
	for (int x = 0; x < dimension; ++x)
	{
		for (int y = 0; y < dimension; ++y)
		{
			float amplitude = 1.0f;
			float persistence = 0.3f;
			perlin_data[y*dimension + x] = 0;
			for (int o = 0; o < octaves; ++o)
			{
				float freq = powf(2, (float)o);
				float perlin_sample = glm::perlin(vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;
				perlin_data[y*dimension + x] += perlin_sample*amplitude;
				amplitude *= persistence;
			}
		}
	}
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dimension, dimension, 0, GL_RED, GL_FLOAT, perlin_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//stbi_image_free(perlin_data);
}
bool TestApplication::startup() {

	// create a basic window
	createWindow("AIE OpenGL Application", 1280, 720);

	// start the gizmo system that can draw basic shapes
	Gizmos::create();

	// create a camera
	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
	m_camera->setLookAtFrom(vec3(-10, 100, -10), vec3(50, 0, 50));

	//////////////////////////////////////////////////////////////////////////
	// YOUR STARTUP CODE HERE
	//////////////////////////////////////////////////////////////////////////
	m_pickPosition = glm::vec3(0);
	
	linkShader();
	textureLoad();

	createOpenGLBuffers(dimension);

	return true;
}

void TestApplication::shutdown() {

	//////////////////////////////////////////////////////////////////////////
	// YOUR SHUTDOWN CODE HERE
	//////////////////////////////////////////////////////////////////////////

	// delete our camera and cleanup gizmos
	delete m_camera;
	Gizmos::destroy();

	glDeleteProgram(m_program);
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

	if (glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS)
	{
		linkShader();
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

	glUseProgram(m_program);
	unsigned int projectionViewUniform = glGetUniformLocation(m_program, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, glm::value_ptr(m_camera->getProjectionView()));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	vec3 light(sin(glfwGetTime()), 1, cos(glfwGetTime()));

	unsigned int LightDirUniform = glGetUniformLocation(m_program, "LightDir");
	glUniform3f(LightDirUniform, m_camera->getTransform()[3][0], m_camera->getTransform()[3][1], m_camera->getTransform()[3][2]);
	//glUniform3f(LightDirUniform, light.x, light.y, light.z);
	//glUniform3f(LightDirUniform, 0, 1, 0);

	unsigned int LightColourUniform = glGetUniformLocation(m_program, "LightColour");
	glUniform3f(LightColourUniform, 0.8, 0.8, 0.8);

	unsigned int diffuseUniform = glGetUniformLocation(m_program, "diffuse");
	glUniform1i(diffuseUniform, 0);
	
	unsigned int dimensionUniform = glGetUniformLocation(m_program, "dimension");
	glUniform1ui(dimensionUniform, dimension);

	glBindVertexArray(m_VAO);
	unsigned int indexCount = (dimension - 1)*(dimension - 1) * 6;
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}