#ifndef _SHADERLOADER_H_
#define _SHADERLOADER_H_

#include <assert.h> 
#include <iostream>
#include <fstream>
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>

class ShaderLoader
{
public:
	ShaderLoader();
	~ShaderLoader();
	void Loader(unsigned int &program, char* vertShader, char* fragShader);
private:
	char* Reader(char* filename);
};

#endif