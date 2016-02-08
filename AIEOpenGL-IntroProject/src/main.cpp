#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

int main() {
	if (glfwInit() == false)
		return -1;


	GLFWwindow* m_window = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);

	if (m_window == nullptr)
	{
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(m_window);
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return -3;
	}

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);

	glfwDestroyWindow(m_window);
	glfwTerminate();
	return 0;
}