#pragma once
#include "gl_core_4_4.h"

#define GLM_SWIZZLE

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <stb_image.h>
#include "AntTweakBar.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
class Terrain
{
public:
	Terrain(unsigned int dimensionIn,float heightIn);
	~Terrain();
	void Draw(unsigned int &m_program);
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int dimension;
	float* perlin_data;
	float height;
	vec3 light;
	vec3 lightColour;
	float textureBlend;
	unsigned int m_texture, m_normalmap, m_specularmap, m_rockD, m_rockN, m_snowD, m_snowN;
	unsigned char* data;
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	struct Vertex
	{
		vec4 position;
		vec2 texCoord;
		vec4 normal;
	};
	void TextureLoad();
};

