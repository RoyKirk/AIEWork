#pragma once

#include "glm/glm.hpp"
#include <vector>



class NeuralNetwork;

class Agent
{
	float _memoryClock;
	NeuralNetwork* neuralNetwork = NULL;
	glm::vec2 _position;
	float _facingDirection;
	glm::vec2 _startingPosition;
	float _startingFacingDirection;

	float _diameter;
	float _clock;
	int _foodClock;
	int _waterClock;
	glm::vec2 _velocity;

	bool _active;
	glm::vec4 _colour;
	float _maxSpeed;
	void checkBounds();
	void fakeMemory(std::vector<glm::vec3>&);
	void initMemory(std::vector<glm::vec3>&);
	float health;
	std::vector<glm::vec3> memory;
	void addToMemory(glm::vec3);

	void resetAgent();
	void checkIfResourceFound();
	void avoidDanger();
	void AStar();

	bool resourceFound;

	glm::vec2 destination;

	float wanderTimer;

	const float ENEMY_AVOIDANCE_RADIUS = 10.0f;
	const float WANDER_TIME = 4.0f;
	const int STARTING_HEALTH = 2;
	const int MAX_HEALTH = 4;

	std::vector<glm::vec3> graphList;
	std::vector<glm::vec3> openList;
	std::vector<glm::vec3> closedList;
	std::vector<glm::vec3> pathList;
	std::vector<glm::vec3> followList;

public:
	Agent();
	~Agent();
	void setup(glm::vec2 startPos, float size, glm::vec4 colour,float facingDirection);
	void update(float delta);
	void draw();
	void addGizmo();
	void hurtAgent(float damage);
	void feedAgent(float food);
	void waterAgent(float water);
	glm::vec2 getPosition();
};