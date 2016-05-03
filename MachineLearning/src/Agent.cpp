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
		AStar();
		//_velocity = destination - _position;
		//float length = sqrt((_velocity.x*_velocity.x) + (_velocity.y*_velocity.y));
		//_velocity.x /= length;
		//_velocity.y /= length;
		//avoidDanger();
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
		avoidDanger();
	}

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
		}
	}
}

void Agent::checkBounds()
{
	int gScreenWidth = 0, gScreenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);

	if ((_position.x < 0) || (_position.x >gScreenWidth) || (_position.y < 0) || (_position.y >gScreenHeight))
	{ 
		_position.x = (float)((int)_position.x % gScreenWidth);
		_position.y = (float)((int)_position.y % gScreenHeight);
		_velocity = glm::vec2(gScreenWidth/2, gScreenHeight/2) - _position;
		float length = sqrt((_velocity.x*_velocity.x) + (_velocity.y*_velocity.y));
		_velocity.x /= length;
		_velocity.y /= length;
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
	_maxSpeed = 400;
	_velocity = destination - _position;
	float length = sqrt((_velocity.x*_velocity.x) + (_velocity.y*_velocity.y));
	_velocity.x /= length;
	_velocity.y /= length;
	health = (float)STARTING_HEALTH;
	resourceFound = false;
	int gScreenWidth = 0, gScreenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);
	destination = glm::vec2((float)(rand() % gScreenWidth), (float)(rand() % gScreenHeight));
	wanderTimer = 0;
	if (neuralNetwork == NULL)
	{
		int numberInputs = 2;
		int numberHiddenNeurons = 30;  //the tutorial notes claim that there are three neurons in hte hidden layer but performance is rather poor with only three and four works better. Try three as an experiment
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
	
	_startingPosition.x = (float)gScreenWidth;
	_startingPosition.y = (float)gScreenHeight;

	health = (float)STARTING_HEALTH;
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
	
	//for each(auto& node in memory)
	for each(auto& node in neuralNetwork->testData)
	{
		if (node.z == 1)
		{
			int gScreenWidth = 0, gScreenHeight = 0;
			glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);
			glm::vec2 direction = glm::vec2(node.x*gScreenWidth, node.y*gScreenHeight) - _position;
			float distance = sqrt((direction.x*direction.x) + (direction.y*direction.y));
			if (distance < ENEMY_AVOIDANCE_RADIUS)
			{
				//direction.x /= distance;
				//direction.y /= distance;

				//_velocity -= glm::vec2(direction.x*(1 / distance), direction.y*(1 / distance));
				//_velocity *= -1;
				_velocity = glm::vec2(_velocity.y*-1, _velocity.x*-1)+(glm::vec2(_velocity.x*-1, _velocity.y*-1));
				//_velocity -= glm::vec2(direction.x*(1 / distance), direction.y*(1 / distance));
				//_velocity -= glm::vec2(direction.y*(1 / distance), direction.x*(1 / distance));
				//_velocity -= glm::vec2(_velocity.x*(1 / distance), _velocity.y*(1 / distance));
				////repulsing vector orthogonal to direction to enemy
				//int randDir = rand() % 1;
				//if (randDir == 0)
				//{
				//	_velocity += glm::vec2(direction.y*-1*(1 / distance), direction.x*-1*(1 / distance));
				//}
				//else
				//{
				//	_velocity += glm::vec2(direction.y*(1 / distance), direction.x*(1 / distance));
				//}
				float length = sqrt((_velocity.x*_velocity.x) + (_velocity.y*_velocity.y));
				_velocity.x /= length;
				_velocity.y /= length;
				//break;
			}
		}

	}
}



void Agent::hurtAgent(float damage)
{
	health -= damage;
	if (health < 0)
	{
		addToMemory(glm::vec3(_position.x, _position.y, 1));
		neuralNetwork->trainNetwork(memory);
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

void Agent::FollowPath()
{

}


void Agent::AStar()
{
	resourceFound = false;
	int gScreenWidth = 0, gScreenHeight = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);
	glm::vec2 closestFood = destination;
	for each(auto& node in neuralNetwork->testData)
	{
		if (node.z == 2)
		{
			resourceFound = true;
			glm::vec2 towardFood = closestFood - _position;
			float distance = sqrt((towardFood.x*towardFood.x) + (towardFood.y*towardFood.y));

			glm::vec2 newFood = glm::vec2((node.x * gScreenWidth), (node.y * gScreenHeight));
			glm::vec2 towardNewFood = newFood - _position;
			float distanceNewFood = sqrt((towardNewFood.x*towardNewFood.x) + (towardNewFood.y*towardNewFood.y));


			if (distanceNewFood < distance)
			{
				closestFood = newFood;
			}
		}
	}

	glm::vec2 bestNextStep = destination;
	for each(auto& node in neuralNetwork->testData)
	{
		float radius = sqrt(((node.x * gScreenWidth) - _position.x) * ((node.x * gScreenWidth) - _position.x) + ((node.y * gScreenHeight) - _position.y) * ((node.y * gScreenHeight) - _position.y));
		if (radius < 30)
		{
			if (node.z == 2)
			{
				bestNextStep = glm::vec2((node.x * gScreenWidth), (node.y * gScreenHeight));
				break;
			}
			if (node.z == 0)
			{
				glm::vec2 nextNodeToFood = closestFood - glm::vec2((node.x * gScreenWidth), (node.y * gScreenHeight));
				float nextNodeToFoodDistance = sqrt((nextNodeToFood.x*nextNodeToFood.x) + (nextNodeToFood.y*nextNodeToFood.y));
				glm::vec2 currentBestNode = closestFood - bestNextStep;
				float currentBestNodeDistance = sqrt((currentBestNode.x*currentBestNode.x) + (currentBestNode.y*currentBestNode.y));
				if (nextNodeToFoodDistance < currentBestNodeDistance)
				{
					bestNextStep = glm::vec2((node.x * gScreenWidth), (node.y * gScreenHeight));
				}
				break;
			}
			//if (node.z == 1)
			//{

			//	glm::vec2 nextNodeToFood = closestFood - glm::vec2((node.x * gScreenWidth), (node.y * gScreenHeight));
			//	float nextNodeToFoodDistance = sqrt((nextNodeToFood.x*nextNodeToFood.x) + (nextNodeToFood.y*nextNodeToFood.y));
			//	glm::vec2 currentBestNode = closestFood - bestNextStep;
			//	float currentBestNodeDistance = sqrt((currentBestNode.x*currentBestNode.x) + (currentBestNode.y*currentBestNode.y));
			//	if (nextNodeToFoodDistance < currentBestNodeDistance)
			//	{
			//		destination = bestNextStep;
			//		_velocity = destination - _position;
			//		float length = sqrt((_velocity.x*_velocity.x) + (_velocity.y*_velocity.y));
			//		_velocity.x /= length;
			//		_velocity.y /= length;
			//		glm::vec2 avoidDirection = glm::vec2(_velocity.y * 30, _velocity.x * 30);
			//		bestNextStep += avoidDirection;
			//	}
			//}
		}
	}

	destination = bestNextStep;
	_velocity = destination - _position;
	float length = sqrt((_velocity.x*_velocity.x) + (_velocity.y*_velocity.y));
	_velocity.x /= length;
	_velocity.y /= length;


}