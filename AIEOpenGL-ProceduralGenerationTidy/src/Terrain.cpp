#include "Terrain.h"





void Terrain::TextureLoad()
{
	float scale = (1.0f / this->dimension) * 3;
	int octaves = 6;
	for (unsigned int x = 0; x < this->dimension; ++x)
	{
		for (unsigned int y = 0; y < this->dimension; ++y)
		{
			float amplitude = 1.0f;
			float persistence = 0.4f;
			this->perlin_data[x*this->dimension + y] = 0;
			for (int o = 0; o < octaves; ++o)
			{
				float freq = powf(2, (float)o);
				float perlin_sample = glm::perlin(vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;
				this->perlin_data[x*this->dimension + y] += perlin_sample*amplitude;
				amplitude *= persistence;
			}
		}
	}
	glGenTextures(1, &this->m_texture);
	glBindTexture(GL_TEXTURE_2D, this->m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, this->dimension, this->dimension, 0, GL_RED, GL_FLOAT, this->perlin_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	this->data = stbi_load("./textures/grass_d.jpg", &this->imageWidth, &this->imageHeight, &this->imageFormat, STBI_default);
	glGenTextures(1, &this->m_rockD);
	glBindTexture(GL_TEXTURE_2D, this->m_rockD);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->imageWidth, this->imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, this->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(this->data);
	this->data = stbi_load("./textures/grass_n.jpg", &this->imageWidth, &this->imageHeight, &this->imageFormat, STBI_default);
	glGenTextures(1, &this->m_rockN);
	glBindTexture(GL_TEXTURE_2D, this->m_rockN);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->imageWidth, this->imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, this->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(this->data);
	this->data = stbi_load("./textures/snow_d.jpg", &this->imageWidth, &this->imageHeight, &this->imageFormat, STBI_default);
	glGenTextures(1, &this->m_snowD);
	glBindTexture(GL_TEXTURE_2D, this->m_snowD);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->imageWidth, this->imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, this->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(this->data);
	this->data = stbi_load("./textures/snow_n.jpg", &this->imageWidth, &this->imageHeight, &this->imageFormat, STBI_default);
	glGenTextures(1, &this->m_snowN);
	glBindTexture(GL_TEXTURE_2D, this->m_snowN);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->imageWidth, this->imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, this->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(this->data);
}

Terrain::Terrain(unsigned int dimensionIn, float heightIn)
{
	this->dimension = dimensionIn;
	this->perlin_data = new float[this->dimension * this->dimension];
	this->height = heightIn;

	TextureLoad();

	Vertex* aoVertices = new Vertex[this->dimension*this->dimension];
	for (unsigned int r = 0; r < this->dimension; ++r)
	{
		for (unsigned int c = 0; c < this->dimension; ++c)
		{
			float hp = perlin_data[r*this->dimension + c] * height;
			aoVertices[r * this->dimension + c].position = vec4((float)c, hp, (float)r, 1);
			aoVertices[r*this->dimension + c].texCoord = vec2((float)r / this->dimension, (float)c / this->dimension);
			aoVertices[r * this->dimension + c].normal = vec4(normalize(vec3(1)), 0);
			aoVertices[r * this->dimension + c].normal.w = 1;
			if (c == 0 || r == 0 || c == this->dimension || r == this->dimension)
			{
				aoVertices[r * this->dimension + c].normal = vec4(0, 0, 0, 0);
			}
			else
			{
				vec3 c1 = aoVertices[r* this->dimension + c].position.xyz - aoVertices[(r - 1)* this->dimension + c].position.xyz;
				vec3 c2 = aoVertices[r* this->dimension + c].position.xyz - aoVertices[r * this->dimension + c - 1].position.xyz;
				vec3 d1 = glm::cross(c2, c1);
				aoVertices[r * this->dimension + c].normal.xyz = normalize(d1);
				aoVertices[r * this->dimension + c].normal.w = 1;
			}
		}
	}

	unsigned int* auiIndices = new unsigned int[(this->dimension - 1)*(this->dimension - 1) * 6];
	unsigned int index = 0;
	for (unsigned int r = 0; r < (this->dimension - 1); ++r)
	{
		for (unsigned int c = 0; c < (this->dimension - 1); ++c)
		{
			//Tirangle 1
			auiIndices[index++] = r*this->dimension + c;
			auiIndices[index++] = (r + 1)*this->dimension + c;
			auiIndices[index++] = (r + 1)*this->dimension + (c + 1);
			//Triangle 2
			auiIndices[index++] = r*this->dimension + c;
			auiIndices[index++] = (r + 1)*this->dimension + (c + 1);
			auiIndices[index++] = r*this->dimension + (c + 1);
		}
	}

	glGenBuffers(1, &this->m_VBO);
	glGenBuffers(1, &this->m_IBO);

	glGenVertexArrays(1, &this->m_VAO);
	glBindVertexArray(this->m_VAO);
	//Vertex Buffer

	glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (this->dimension*this->dimension)*sizeof(Vertex), aoVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(sizeof(vec4) + sizeof(vec2)));


	//Index Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (this->dimension - 1) * (this->dimension - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);


	//Vertex Array Object
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] aoVertices;
	delete[] auiIndices;
}


Terrain::~Terrain()
{
	delete[] this->perlin_data;
}

void Terrain::Draw(unsigned int &m_program)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->m_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->m_rockD);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, this->m_rockN);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, this->m_snowD);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, this->m_snowN);
	
	unsigned int LightDirUniform = glGetUniformLocation(m_program, "LightDir");
	//glUniform3f(LightDirUniform, m_camera->getTransform()[3][0], m_camera->getTransform()[3][1], m_camera->getTransform()[3][2]);
	glUniform3f(LightDirUniform, light.x, light.y, light.z);
	//glUniform3f(LightDirUniform, 1, 1, 1);

	unsigned int LightColourUniform = glGetUniformLocation(m_program, "LightColour");
	glUniform3f(LightColourUniform, lightColour.x, lightColour.y, lightColour.z);

	unsigned int textureBlendUniform = glGetUniformLocation(m_program, "textureBlend");
	glUniform1f(textureBlendUniform, textureBlend);

	unsigned int diffuseUniform = glGetUniformLocation(m_program, "diffuse");
	glUniform1i(diffuseUniform, 0);

	diffuseUniform = glGetUniformLocation(m_program, "ground_textureD");
	glUniform1i(diffuseUniform, 1);

	unsigned int normalUniform = glGetUniformLocation(m_program, "ground_textureN");
	glUniform1i(normalUniform, 2);

	diffuseUniform = glGetUniformLocation(m_program, "snow_textureD");
	glUniform1i(diffuseUniform, 3);

	normalUniform = glGetUniformLocation(m_program, "snow_textureN");
	glUniform1i(normalUniform, 4);

	unsigned int dimensionUniform = glGetUniformLocation(m_program, "dimension");
	glUniform1ui(dimensionUniform, this->dimension);

	glBindVertexArray(m_VAO);
	unsigned int indexCount = (dimension - 1)*(dimension - 1) * 6;
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}