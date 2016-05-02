#include "Agent.h"
#include <GLFW/glfw3.h>
#include "NeuralNetwork.h"
#include "Gizmos.h"

Agent::Agent()
{
}

Agent::~Agent()
{
}
void Agent::update(float delta)
{
	glm::vec3 testinput = glm::vec3(_position.x, _position.y, 0);
	_clock += delta;
	_memoryClock+= delta;
	float memoryFrequency = 1;
	if (_memoryClock > memoryFrequency)
	{
		_memoryClock -= memoryFrequency;
		glm::vec3 testinput = glm::vec3(_position.x, _position.y,0);
		addToMemory(testinput);
		neuralNetwork->trainNetwork(memory); 
	}



	neuralNetwork->renderDebug(glm::vec2(30, 30), 200, memory);
	_foodClock--;
	_waterClock--;
	if (resourceFound)
	{
		_velocity = destination - _position;
		float length = sqrt((_velocity.x*_velocity.x) + (_velocity.y*_velocity.y));
		_velocity.x /= length;
		_velocity.y /= length;
	}
	else
	{
		wanderTimer += delta;
		if (wanderTimer > WANDER_TIME)
		{
			int gScreenWidth = 0, gScreenHeight = 0;
			glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);
			destination = glm::vec2((float)(rand() % gScreenWidth), (float)(rand() % gScreenHeight));
			wanderTimer = 0;
			_velocity = destination - _position;
			float length = sqrt((_velocity.x*_velocity.x) + (_velocity.y*_velocity.y));
			_velocity.x /= length;
			_velocity.y /= length;
		}

		checkIfResourceFound();
	}
	avoidDanger();
	checkBounds();
	_position += _velocity * _maxSpeed * delta;
}

void Agent::checkIfResourceFound()
{
	int gScreenWidth = 0, gScreenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);
	resourceFound = false;
	for each(auto& node in memory)
	{
		if (node.z == 2 || node.z == 3)
		{

			destination = glm::vec2(node.x*gScreenWidth, node.y*gScreenHeight);
			resourceFound = true;
			//glm::vec2 direction = glm::vec2(node.x, node.y) - _position;
			//float distanceToNew = sqrt((direction.x*direction.x) + (direction.y*direction.y));
			//direction = destination - _position;
			//float distanceToDestination = sqrt((direction.x*direction.x) + (direction.y*direction.y));
			//if (distanceToNew < distanceToDestination)
			//{
			//	destination = glm::vec2(node.x, node.y);
			//}	
		}
	}
}

void Agent::checkBounds()
{
	int gScreenWidth = 0, gScreenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);

	if ((_position.x < 0) || (_position.x >gScreenWidth) )
	{ 
		_velocity.x *= -1;
	}
	if ((_position.y < 0) || (_position.y >gScreenHeight))
	{
		_velocity.y *= -1;
	}
}

void Agent::draw()
{
}

void Agent::setup(glm::vec2 startPos, float size,glm::vec4 colour,float facingDirection)
{
	_position = startPos;
	_startingPosition = startPos;
	//_facingDirection = facingDirection;
	_startingFacingDirection = facingDirection;
	_diameter = size;
	_clock = 0;
	_foodClock = 0;
	_waterClock = 0;
	_memoryClock = 0;
	_colour = colour;
	_maxSpeed = 200;
	_velocity = destination - _position;
	float length = sqrt((_velocity.x*_velocity.x) + (_velocity.y*_velocity.y));
	_velocity.x /= length;
	_velocity.y /= length;
	//_velocity.x = _maxSpeed * sin(_facingDirection);
	//_velocity.y = _maxSpeed * cos(_facingDirection);
	health = STARTING_HEALTH;
	resourceFound = false;
	int gScreenWidth = 0, gScreenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);
	destination = glm::vec2((float)(rand() % gScreenWidth), (float)(rand() % gScreenHeight));
	wanderTimer = 0;
	if (neuralNetwork == NULL)
	{
		int numberInputs = 2;
		int numberHiddenNeurons = 4;  //the tutorial notes claim that there are three neurons in hte hidden layer but performance is rather poor with only three and four works better. Try three as an experiment
		int numberOutputs = 4; 
		neuralNetwork = new NeuralNetwork(numberInputs, numberHiddenNeurons, numberOutputs);
		initMemory(memory);
		neuralNetwork->trainNetwork(memory);  //mock up for testing purposes
		neuralNetwork->setUpDebugRender(30); //setup our debug 
	}
}

void Agent::resetAgent()
{
	int gScreenWidth = 0, gScreenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);
	addToMemory(glm::vec3(_position.x, _position.y, 1));
	neuralNetwork->trainNetwork(memory);
	int randSelector = rand() % 4;
	if (randSelector == 0)
	{
		_startingPosition.x = 20;
		_startingPosition.y = 20;
	}
	else if (randSelector == 1)
	{
		_startingPosition.x = 20;
		_startingPosition.y = gScreenHeight-20;
	}
	else if (randSelector == 2)
	{
		_startingPosition.x = gScreenWidth-20;
		_startingPosition.y = 20;
	}
	else
	{
		_startingPosition.x = gScreenWidth-20;
		_startingPosition.y = gScreenHeight-20;
	}
	
	//_startingPosition.x = gScreenWidth;
	//_startingPosition.y = gScreenHeight;

	//if (resourceFound)
	//{
	//	_startingPosition.x = gScreenWidth;
	//	_startingPosition.y = gScreenHeight;
	//}
	//else
	//{
	//	_startingPosition.x = (float)(rand() % gScreenWidth);
	//	_startingPosition.y = (float)(rand() % 2 * gScreenHeight);
	//}	
	health = STARTING_HEALTH;
	float size = 20;
	float facing = 44 / 7.0f * ((rand() % 1000) / 1000.0f);
	setup(_startingPosition, _diameter, _colour, facing);
}

void Agent::initMemory(std::vector<glm::vec3>& memory)
{
	memory.push_back(glm::vec3(_position.x, _position.y, 0));
}

void Agent::addToMemory(glm::vec3 newMemory)
{
	int gScreenWidth = 0, gScreenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);
	newMemory.x /= (float)gScreenWidth;
	newMemory.y /= (float)gScreenHeight;
	memory.push_back(newMemory);
}

void Agent::addGizmo()
{
	int segments = 20;
	Gizmos::add2DCircle(_position, _diameter*1.1f / 2, segments, glm::vec4(0, 0, 0, 1));
	Gizmos::add2DCircle(_position, _diameter*.9f / 2, segments, _colour);


}

void Agent::avoidDanger()
{
	for each(auto& node in memory)
	{
		if (node.z == 1)
		{
			int gScreenWidth = 0, gScreenHeight = 0;
			glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);
			glm::vec2 direction = glm::vec2(node.x*gScreenWidth, node.y*gScreenHeight) - _position;
			float distance = sqrt((direction.x*direction.x) + (direction.y*direction.y));
			if (distance < ENEMY_AVOIDANCE_RADIUS)
			{
				direction.x /= distance;
				direction.y /= distance;
				//repulsing vector orthogonal to direction to enemy
				int randDir = rand() % 1;
				if (randDir == 0)
				{
					_velocity += glm::vec2(direction.y*-1*(1 / distance), direction.x*-1*(1 / distance));
				}
				else
				{
					_velocity += glm::vec2(direction.y*(1 / distance), direction.x*(1 / distance));
				}
				float length = sqrt((_velocity.x*_velocity.x) + (_velocity.y*_velocity.y));
				_velocity.x /= length;
				_velocity.y /= length;
			}
		}
	}
}

void Agent::hurtAgent(float damage)
{
	health -= damage;
	if (health < 0)
	{
		checkIfResourceFound();
		resetAgent();
	}
}

void Agent::feedAgent(float foodFound)
{
	if (foodFound > 0)
	{
		health += foodFound;
		if (_foodClock <= 0) //simple mechanism to stop too many food discoveries getting added to memory
		{
			addToMemory(glm::vec3(_position.x, _position.y, 2));
			neuralNetwork->trainNetwork(memory);
			_foodClock = 20;
		}
	}
	if (health > MAX_HEALTH)
	{
		checkIfResourceFound();
		resetAgent();
	}
}

void Agent::waterAgent(float waterFound)
{
	if (waterFound > 0)
	{
		health += waterFound;
		if (_waterClock <= 0) //simple mechanism to stop too many water discoveries getting added to memory
		{
			addToMemory(glm::vec3(_position.x, _position.y, 3));
			neuralNetwork->trainNetwork(memory);
			_waterClock = 20;
		}
	}
	if (health > MAX_HEALTH)
	{
		checkIfResourceFound();
		resetAgent();
	}

}


glm::vec2 Agent::getPosition()
{
	return _position;
}