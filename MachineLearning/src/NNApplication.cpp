#include "NNApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Gizmos.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

NNApplication::NNApplication() {

}

NNApplication::~NNApplication() {

}

bool NNApplication::startup() {

	// create a basic window
	createWindow("AIE OpenGL Application", 1280, 720);

	// start the gizmo system that can draw basic shapes
	Gizmos::create(0xffff, 0xffff, 0xffff, 0xffff);
	
	setUpSimulation();

	return true;
}

void NNApplication::shutdown() {

	Gizmos::destroy();

	// destroy our window properly
	destroyWindow();
}

bool NNApplication::update(float deltaTime) {
	
	// close the application if the window closes
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;
	
	// clear the gizmos out for this frame
	Gizmos::clear();

	updateAgents(deltaTime);
	linearZone.addGizmo();
	addAgentGizmos();
	addTurretWidgets();
	addFoodWidgets();
	addWaterWidgets();

	// return true, else the application closes
	return true;
}

void NNApplication::draw() {

	// clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get a orthographic projection matrix and draw 2D gizmos
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0, (float)width, 0, (float)height);

	Gizmos::draw2D(guiMatrix);
}

void NNApplication::setUpSimulation() {
	setUpAgents();
	//setUpSimpleLinearZone();
	setUpTurrets();
	setUpFood();
	setUpWater();
}

void NNApplication::setUpSimpleLinearZone() {
	glm::vec2 centre(600, 400);
	float rotation = 1.5;
	linearZone = LinearZone(centre, rotation);
}

void NNApplication::setUpAgents() {

	int gScreenWidth = 0, gScreenHeight = 0;
	glfwGetWindowSize(m_window, &gScreenWidth, &gScreenHeight);

	srand((unsigned int)time(NULL));
	for (int index = 0; index < MAX_AGENTS; index++) {
		glm::vec2 startPos;

		startPos.x = (float)gScreenWidth;
		startPos.y = (float)gScreenHeight;
		float size = 30;
		float facing = 44 / 7.0f * ((rand() % 1000) / 1000.0f);
		agents[index].setup(startPos, size, glm::vec4(1, 1, 1, 1), facing);
	}
}

void NNApplication::updateAgents(float delta) {
	for (int index = 0; index < MAX_AGENTS; index++) {
		agents[index].update(delta);
		checkAgentDamage(&agents[index]);
		checkAgentFood(&agents[index]);
		checkAgentWater(&agents[index]);
	}
}

void NNApplication::checkAgentDamage(Agent* agent) {
	float damage = 0;
	damage += linearZone.checkRange(agent->getPosition());
	for (auto& turret : turrets) {
		damage += turret.checkRange(agent->getPosition());
	}
	agent->hurtAgent(damage);
}

void NNApplication::checkAgentFood(Agent* agent) {
	float foodFound = 0;
	for (auto& food : foods) {
		foodFound += food.checkRange(agent->getPosition());
	}
	agent->feedAgent(foodFound);
}

void NNApplication::checkAgentWater(Agent* agent) {
	float waterFound = 0;
	for (auto& water : waters) {
		waterFound += water.checkRange(agent->getPosition());
	}
	agent->waterAgent(waterFound);
}

void NNApplication::setUpTurrets() {
	int gScreenWidth = 0, gScreenHeight = 0;
	glfwGetWindowSize(m_window, &gScreenWidth, &gScreenHeight);

	turrets[0] = Turret(glm::vec2(gScreenWidth/2, gScreenHeight/2), 100);
}

void NNApplication::setUpFood() {
	int gScreenWidth = 0, gScreenHeight = 0;
	glfwGetWindowSize(m_window, &gScreenWidth, &gScreenHeight);
	foods[0] = Food(glm::vec2(200, 200), 150);
}

void NNApplication::setUpWater() {
	//waters[0] = Water(glm::vec2(100, 100), 75);
}

float NNApplication::simulateTurret(glm::vec2& centre, float range, Agent* agent) {
	glm::vec2 displacment = agent->getPosition() - centre;
	float distance = glm::length(displacment);
	if (distance<range) {
		return true;
	}
	return false;
}

void NNApplication::drawAgents() {
	for (int index = 0; index < MAX_AGENTS; index++) {
		agents[index].draw();
	}
}

void NNApplication::addAgentGizmos() {
	for (auto& agent : agents) {
		agent.addGizmo();
	}
}

void NNApplication::addTurretWidgets() {
	for (auto& turret : turrets) {
		turret.addGizmo();
	}
}

void NNApplication::addFoodWidgets() {
	for (auto& food : foods) {
		food.addGizmo();
	}
}

void NNApplication::addWaterWidgets() {
	for (auto& water : waters) {
		water.addGizmo();
	}
}