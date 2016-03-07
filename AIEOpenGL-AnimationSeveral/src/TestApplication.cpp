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
#include "BoundingSphere.h"


using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

unsigned int m_program;

int imageWidth = 0, imageHeight = 0, imageFormat = 0;

unsigned int m_texture1, m_normalmap1, m_specularmap1;
unsigned int m_texture2, m_normalmap2, m_specularmap2;
unsigned char* data;

unsigned int m_vertexAttributes;
FBXMaterial* m_material;
std::vector<FBXVertex> m_vertices;
std::vector<unsigned int> m_indices;
glm::mat4 m_localTransform;
glm::mat4 m_globalTransform;
void* m_userData;
FBXFile* m_fbx1;
FBXFile* m_fbx2;

bool fbx1Cull = false;
bool fbx2Cull = false;

std::vector<vec3> posData1;
std::vector<vec3> posData2;
float m_timer;
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
		planes[i].x /= glm::length(glm::vec3(planes[i].xyz));
		planes[i].y /= glm::length(glm::vec3(planes[i].xyz));
		planes[i].z /= glm::length(glm::vec3(planes[i].xyz));
	}
}

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

void createOpenGLBuffers(FBXFile* fbx, std::vector<vec3> &posData)
{
for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
		unsigned int* glData = new unsigned int [3];
		for (int i = 0; i < mesh->m_vertices.size(); i++)
		{
			posData.push_back(vec3(mesh->m_vertices.data()[i].position.x, mesh->m_vertices.data()[i].position.y, mesh->m_vertices.data()[i].position.z));
		}
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

void linkShader(char* vertfilename, char* fragfilename)
{
	const char* vsSource = loadShader(vertfilename);
	const char* fsSource = loadShader(fragfilename);

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
	data = stbi_load("./models/Demolition/demolition_D.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glGenTextures(1, &m_texture1);
	glBindTexture(GL_TEXTURE_2D, m_texture1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

	data = stbi_load("./models/Demolition/demolition_N.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glGenTextures(1, &m_normalmap1);
	glBindTexture(GL_TEXTURE_2D, m_normalmap1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

	data = stbi_load("./models/Demolition/demolition_S.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glGenTextures(1, &m_specularmap1);
	glBindTexture(GL_TEXTURE_2D, m_specularmap1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

	data = stbi_load("./models/Marksman/Marksman_D.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glGenTextures(1, &m_texture2);
	glBindTexture(GL_TEXTURE_2D, m_texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

	data = stbi_load("./models/Marksman/Marksman_N.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glGenTextures(1, &m_normalmap2);
	glBindTexture(GL_TEXTURE_2D, m_normalmap2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

	data = stbi_load("./models/Marksman/Marksman_S.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	glGenTextures(1, &m_specularmap2);
	glBindTexture(GL_TEXTURE_2D, m_specularmap2);
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
	m_camera = new Camera(glm::pi<float>() * 0.15f, 16 / 9.f, 0.1f, 5000.f);
	m_camera->setLookAtFrom(vec3(0, 0, 0), vec3(1, 0, 1));

	//////////////////////////////////////////////////////////////////////////
	// YOUR STARTUP CODE HERE
	//////////////////////////////////////////////////////////////////////////
	m_pickPosition = glm::vec3(0);

	linkShader("./src/Shader.vert", "./src/Shader.frag");

	textureLoad();
	m_fbx1 = new FBXFile();
	m_fbx2 = new FBXFile();
	//m_fbx->load("./models/enemynormal/EnemyNormal.fbx");
	//m_fbx->load("./models/Medic/medic.fbx");
	m_fbx2->load("./models/Marksman/Marksman.fbx");
	//m_fbx->load("./models/Enemytank/Enemytank.fbx");
	m_fbx1->load("./models/Demolition/demolition.fbx");
	//m_fbx2->load("./models/Enemytank/Enemytank.fbx");
	//m_fbx->load("./models/Dragon.fbx");
	//m_fbx->load("./models/Lucy.fbx");
	//m_fbx->load("./models/Bunny.fbx");
	//m_fbx->load("./models/Buddha.fbx");
	createOpenGLBuffers(m_fbx1,posData1);
	createOpenGLBuffers(m_fbx2,posData2);

	return true;
}

void TestApplication::shutdown() {

	//////////////////////////////////////////////////////////////////////////
	// YOUR SHUTDOWN CODE HERE
	//////////////////////////////////////////////////////////////////////////

	// delete our camera and cleanup gizmos
	delete m_camera;
	Gizmos::destroy();

	cleanupOpenGLBuffers(m_fbx1);
	cleanupOpenGLBuffers(m_fbx2);
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

	FBXSkeleton* skeleton1 = m_fbx1->getSkeletonByIndex(0);
	FBXAnimation* animation1 = m_fbx1->getAnimationByIndex(0);

	skeleton1->evaluate(animation1, m_timer);

	for (unsigned int bone_index = 0; bone_index < skeleton1->m_boneCount; ++bone_index)
	{
		skeleton1->m_nodes[bone_index]->updateGlobalTransform();
	}

	FBXSkeleton* skeleton2 = m_fbx2->getSkeletonByIndex(0);
	FBXAnimation* animation2 = m_fbx2->getAnimationByIndex(0);

	skeleton2->evaluate(animation2, m_timer);

	for (unsigned int bone_index = 0; bone_index < skeleton2->m_boneCount; ++bone_index)
	{
		skeleton2->m_nodes[bone_index]->updateGlobalTransform();
	}

	vec4 planes[6];
	getFrustumPlanes(m_camera->getProjectionView(), planes);

	BoundingSphere sphere1;
	sphere1.fit(posData1);

	for (int i = 0; i < 6; i++)
	{
		
		float d = glm::dot(vec3(planes[i]), sphere1.centre) + planes[i].w;
		if (d < -sphere1.radius)
		{
			if (fbx1Cull)
			{
				printf("1Behind, don't render it!\n");
			}
			fbx1Cull = false;
			break;
		}
		else if (d < sphere1.radius)
		{
			printf("1Touching, we still need to render it!\n");
			fbx1Cull = true;
		}
		else
		{	
			printf("1Front, fully visible so render it!\n");
			fbx1Cull = true;
		}
	}


	BoundingSphere sphere2;
	sphere2.fit(posData2);
	sphere2.centre.z -= 1000;

	for (int i = 0; i < 6; i++)
	{

		float d = glm::dot(vec3(planes[i]), sphere2.centre) + planes[i].w;
		if (d < -sphere2.radius)
		{
			if (fbx2Cull)
			{
				printf("2Behind, don't render it!\n");
			}
			fbx2Cull = false;
			break;
		}
		else if (d < sphere2.radius)
		{
			printf("2Touching, we still need to render it!\n");
			
			fbx2Cull = true;
		}
		else
		{
			printf("2Front, fully visible so render it!\n");
			fbx2Cull = true;
		}
	}

	if (fbx1Cull)
	{
		Gizmos::addSphere(sphere1.centre, sphere1.radius, 10, 10, vec4(1, 0, 1, 1));
	}
	if (fbx2Cull)
	{
		Gizmos::addSphere(sphere2.centre, sphere2.radius, 10, 10, vec4(1, 0, 1, 1));
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
	

	// get a orthographic projection matrix and draw 2D gizmos
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0, 0, (float)width, (float)height);

	Gizmos::draw2D(m_camera->getProjectionView());


	glUseProgram(m_program);
	unsigned int projectionViewUniform = glGetUniformLocation(m_program, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, glm::value_ptr(m_camera->getProjectionView()));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalmap1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_specularmap1);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_texture2);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_normalmap2);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_specularmap2);

	vec3 light(sin(glfwGetTime()), 1, cos(glfwGetTime()));

	unsigned int LightDirUniform = glGetUniformLocation(m_program, "LightDir");
	//glUniform3f(LightDirUniform, m_camera->getTransform()[3][0], m_camera->getTransform()[3][1], m_camera->getTransform()[3][2]);
	//glUniform3f(LightDirUniform, light.x, light.y, light.z);
	glUniform3f(LightDirUniform, 0, 1, 0);

	unsigned int CameraPosUniform = glGetUniformLocation(m_program, "CameraPos");
	glUniform3f(CameraPosUniform, m_camera->getTransform()[3][0], m_camera->getTransform()[3][1], m_camera->getTransform()[3][2]);

	unsigned int SpecPowUniform = glGetUniformLocation(m_program, "SpecPow");
	glUniform1f(SpecPowUniform, 10);

	if (fbx1Cull)
	{
		for (unsigned int i = 0; i < m_fbx1->getMeshCount(); ++i)
		{
			unsigned int diffuseUniform = glGetUniformLocation(m_program, "diffuse");
			glUniform1i(diffuseUniform, 0);
			unsigned int normalUniform = glGetUniformLocation(m_program, "normal");
			glUniform1i(normalUniform, 1);
			unsigned int specularUniform = glGetUniformLocation(m_program, "specular");
			glUniform1i(specularUniform, 2);

			unsigned int offsetUniform = glGetUniformLocation(m_program, "offset");
			glUniform1f(offsetUniform, 0);

			FBXSkeleton* skeleton = m_fbx1->getSkeletonByIndex(0);
			skeleton->updateBones();
			int bones_location = glGetUniformLocation(m_program, "bones");
			glUniformMatrix4fv(bones_location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

			FBXMeshNode* mesh = m_fbx1->getMeshByIndex(i);
			unsigned int* glData = (unsigned int*)mesh->m_userData;
			glBindVertexArray(glData[0]);
			glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		}
	}
	if (fbx2Cull)
	{
		for (unsigned int i = 0; i < m_fbx2->getMeshCount(); ++i)
		{
			unsigned int diffuseUniform = glGetUniformLocation(m_program, "diffuse");
			glUniform1i(diffuseUniform, 3);
			unsigned int normalUniform = glGetUniformLocation(m_program, "normal");
			glUniform1i(normalUniform, 4);
			unsigned int specularUniform = glGetUniformLocation(m_program, "specular");
			glUniform1i(specularUniform, 5);

			unsigned int offsetUniform = glGetUniformLocation(m_program, "offset");
			glUniform1f(offsetUniform, -1000);

			FBXSkeleton* skeleton = m_fbx2->getSkeletonByIndex(0);
			skeleton->updateBones();
			int bones_location = glGetUniformLocation(m_program, "bones");
			glUniformMatrix4fv(bones_location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

			FBXMeshNode* mesh = m_fbx2->getMeshByIndex(i);
			unsigned int* glData = (unsigned int*)mesh->m_userData;
			glBindVertexArray(glData[0]);
			glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Gizmos::draw(m_camera->getProjectionView());
}