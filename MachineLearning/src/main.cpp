#include "TestApplication.h"
#include "NNApplication.h"
#include <GLFW/glfw3.h>

int main() {
	//NNApplication* machineLearningApp = new NNApplication();
	BaseApplication* app = new NNApplication();
	//BaseApplication* app = new TestApplication();
	if (app->startup())
		app->run();
	app->shutdown();

	return 0;
}