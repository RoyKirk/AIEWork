#include "TestApplication.h"
#include "gl_core_4_4.h"

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

unsigned int m_texture, m_normalmap;
unsigned char* data;

unsigned int m_vertexAttributes;
FBXMaterial* m_material;
std::vector<FBXVertex> m_vertices;
std::vector<unsigned int> m_indices;
glm::mat4 m_localTransform;
glm::mat4 m_globalTransform;
void* m_userData;
FBXFile* m_fbx;

float m_timer;

void cleanupOpenGLBuffers(FBXFile* fbx)
{
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glDeleteVertexArrays(1, &glData[0]);
		glDeleteVertexArrays(1, &glData[1]);
		glDeleteVertexArrays(1, &glData[2]);

		delete[] glData;
	}
}

void createOpenGLBuffers(FBXFile* fbx)
{
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
		unsigned int* glData = new unsigned int[3];
		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);
		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);
		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*) + FBXVertex::PositionOffset);
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*) + FBXVertex::NormalOffset);
		glEnableVertexAttribArray(2); // tangents
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*) + FBXVertex::TangentOffset);
		glEnableVertexAttribArray(3); // texcoords
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*) + FBXVertex::TexCoord1Offset);
		glEnableVertexAttribArray(4); // weights
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*) + FBXVertex::WeightsOffset);
		glEnableVertexAttribArray(5); // indices
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*) + FBXVertex::IndicesOffset);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
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

void textureLoad()
{
	//data = stbi_load("./models/Marksman/Marksman_D.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	//data = stbi_load("./models/Enemytank/Enemytank_D.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	//data = stbi_load("./models/Medic/Medic_D.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	data = stbi_load("./models/Demolition/demolition_D.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

	data = stbi_load("./models/Demolition/demolition_N.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glGenTextures(1, &m_normalmap);
	glBindTexture(GL_TEXTURE_2D, m_normalmap);
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
	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 10000.f);
	m_camera->setLookAtFrom(vec3(1500, 2000, 1500), vec3(0, 1000, 0));

	//////////////////////////////////////////////////////////////////////////
	// YOUR STARTUP CODE HERE
	//////////////////////////////////////////////////////////////////////////
	m_pickPosition = glm::vec3(0);

	linkShader();

	textureLoad();
	m_fbx = new FBXFile();
	//m_fbx->load("./models/Medic/medic.fbx");
	//m_fbx->load("./models/Marksman/Marksman.fbx");
	//m_fbx->load("./models/Enemytank/Enemytank.fbx");
	m_fbx->load("./models/Demolition/demolition.fbx");
	//m_fbx->load("./models/Dragon.fbx");
	//m_fbx->load("./models/Lucy.fbx");
	//m_fbx->load("./models/Bunny.fbx");
	//m_fbx->load("./models/Buddha.fbx");
	createOpenGLBuffers(m_fbx);

	return true;
}

void TestApplication::shutdown() {

	//////////////////////////////////////////////////////////////////////////
	// YOUR SHUTDOWN CODE HERE
	//////////////////////////////////////////////////////////////////////////

	// delete our camera and cleanup gizmos
	delete m_camera;
	Gizmos::destroy();

	cleanupOpenGLBuffers(m_fbx);
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
	Gizmos::addTransform(glm::translate(m_pickPosition));

	m_timer += deltaTime;

	FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
	FBXAnimation* animation = m_fbx->getAnimationByIndex(0);

	skeleton->evaluate(animation, m_timer);

	for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
	{
		skeleton->m_nodes[bone_index]->updateGlobalTransform();
	}


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
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalmap);

	FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
	skeleton->updateBones();

	int bones_location = glGetUniformLocation(m_program, "bones");
	glUniformMatrix4fv(bones_location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	vec3 light(sin(glfwGetTime()), 1, cos(glfwGetTime()));

	unsigned int LightDirUniform = glGetUniformLocation(m_program, "LightDir");
	//glUniform3f(LightDirUniform, m_camera->getTransform()[3][0], m_camera->getTransform()[3][1], m_camera->getTransform()[3][2]);
	glUniform3f(LightDirUniform, light.x, light.y, light.z);

	unsigned int diffuseUniform = glGetUniformLocation(m_program, "diffuse");
	glUniform1i(diffuseUniform, 0);

	unsigned int normalUniform = glGetUniformLocation(m_program, "normal");
	glUniform1i(normalUniform, 1);


	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}