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
	
	if (followPath)
	{
		FollowPath();
	}
	else if (resourceFound)
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

void Agent::AStar()
{
	resetAgent();
	resourceFound = false;
	followPath = false;
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
	destination = closestFood;
	Node* startNode = new Node;
	//{glm::vec2(0, 0), 0, 0, 0, nullptr};
	startNode->position = _position;
	startNode->f = 0;
	startNode->g = 0;
	startNode->h = 0;
	startNode->parent = nullptr;
	Node* endNode = new Node;
	endNode->position = destination; 
	endNode->f = 0;
	endNode->g = 0;
	endNode->h = 0;
	endNode->parent = nullptr;

	openList.push_front(startNode);


	//for (int i = gScreenWidth; i < gScreenWidth - 100; i--)
	//{
	//	for (int j = gScreenWidth; j < gScreenWidth - 100; j--)
	//	{
	//		glm::vec3 testinput = glm::vec3((float)i, (float)j, 0);
	//		addToMemory(testinput);
	//		neuralNetwork->trainNetwork(memory);
	//	}
	//}


	while (!followPath)
	{
		if (!closedList.empty() && closedList.front()->position.x == endNode->position.x && closedList.front()->position.y == endNode->position.y && pathList.empty())
		{
			followPath = true;
			pathList.push_front(closedList.front());
			while (pathList.front() != startNode)
			{
				Node* currentNode = pathList.front()->parent;
				pathList.push_front(currentNode);
			}
		}
		else if (!closedList.empty() && closedList.front()->position.x == endNode->position.x && closedList.front()->position.y == endNode->position.y)
		{
			followPath = true;
		}
		else
		{
			if (!openList.empty())
			{
				openList.sort([](const Node* a, const Node* b) { return a->f < b->f; });
				Node* currentNode = openList.front();
				openList.pop_front();


				for each(auto& node in neuralNetwork->testData)
				{
					float radius = sqrt(((node.x * gScreenWidth) - _position.x) * ((node.x * gScreenWidth) - _position.x) + ((node.y * gScreenHeight) - _position.y) * ((node.y * gScreenHeight) - _position.y));
					if (radius < 100)
					{
						Node* testNode = new Node;
						testNode->position = glm::vec2(node.x*gScreenWidth, node.y*gScreenHeight);
						testNode->f = 0;
						testNode->g = 0;
						testNode->h = 0;
						testNode->parent = nullptr;
						if (node.z == 1)
						{
							continue;
						}
						bool foundClosed = (std::find(closedList.begin(), closedList.end(), testNode) != closedList.end());
						if (!foundClosed)
						{
							bool foundOpen = (std::find(openList.begin(), openList.end(), currentNode) != openList.end());
							if (!foundOpen)
							{
								//Euclidian Distance
								testNode->g = currentNode->g + sqrt((testNode->position.x - currentNode->position.x)*(testNode->position.x - currentNode->position.x)
									+ (testNode->position.y - currentNode->position.y)*(testNode->position.y - currentNode->position.y));
								testNode->h = sqrt((endNode->position.x - testNode->position.x)*(endNode->position.x - testNode->position.x)
									+ (endNode->position.y - testNode->position.y)*(endNode->position.y - testNode->position.y));
								testNode->f = testNode->g + testNode->h;

								testNode->parent = currentNode;
								openList.push_front(new Node);
								openList.front()->position = testNode->position;
								openList.front()->f = testNode->f;
								openList.front()->g = testNode->g;
								openList.front()->h = testNode->h;
								openList.front()->parent = testNode->parent;
							}
							else
							{
								for each(Node* currentCandidate in openList)
								{
									if (testNode->position.x == currentCandidate->position.x
										&& currentNode->position.y == currentCandidate->position.y
										&& currentCandidate->g > currentNode->g)
									{
										currentCandidate->parent = currentNode;

										//Euclidian Distance renew f,g,h scores
										currentCandidate->g = currentNode->g + sqrt((currentCandidate->position.x - currentNode->position.x)*(currentCandidate->position.x - currentNode->position.x)
											+ (currentCandidate->position.y - currentNode->position.y)*(currentCandidate->position.y - currentNode->position.y));
										currentCandidate->h = sqrt((endNode->position.x - currentCandidate->position.x)*(endNode->position.x - currentCandidate->position.x)
											+ (endNode->position.y - currentCandidate->position.y)*(endNode->position.y - currentCandidate->position.y));
										currentCandidate->f = currentCandidate->g + currentCandidate->h;
									}
								}
							}
						}
					}
				}
				closedList.push_front(currentNode);
			}
		}
		
	}
}

void Agent::FollowPath()
{
	if (followList.empty())
	{
		followList.assign(pathList.begin(), pathList.end());
	}
	else
	{
		Node* currentNode = followList.front();
		_velocity = currentNode->position - _position;
		float length = sqrt((_velocity.x*_velocity.x) + (_velocity.y*_velocity.y));
		_velocity.x /= length;
		_velocity.y /= length;
		followList.pop_front();
		//if (this->agent.position.vector[0] < pathList.back()->x + GRID_SIZE / 2 && this->agent.position.vector[0] > pathList.back()->x - GRID_SIZE / 2
		//	&& this->agent.position.vector[1] < pathList.back()->y + GRID_SIZE / 2 && this->agent.position.vector[1] > pathList.back()->y - GRID_SIZE / 2)
		//{
		//}
		//else if (this->agent.position.vector[0] < followList.front()->x + GRID_SIZE / 8 && this->agent.position.vector[0] > followList.front()->x - GRID_SIZE / 8
		//	&& this->agent.position.vector[1] < followList.front()->y + GRID_SIZE / 8 && this->agent.position.vector[1] > followList.front()->y - GRID_SIZE / 8)
		//{
		//	followList.pop_front();
		//}
		//else
		//{
		//}

	}

}

//
//void Agent::AStar()
//{
//	resourceFound = false;
//	int gScreenWidth = 0, gScreenHeight = 0;
//	glfwGetWindowSize(glfwGetCurrentContext(), &gScreenWidth, &gScreenHeight);
//	glm::vec2 closestFood = destination;
//	for each(auto& node in neuralNetwork->testData)
//	{
//		if (node.z == 2)
//		{
//			resourceFound = true;
//			glm::vec2 towardFood = closestFood - _position;
//			float distance = sqrt((towardFood.x*towardFood.x) + (towardFood.y*towardFood.y));
//
//			glm::vec2 newFood = glm::vec2((node.x * gScreenWidth), (node.y * gScreenHeight));
//			glm::vec2 towardNewFood = newFood - _position;
//			float distanceNewFood = sqrt((towardNewFood.x*towardNewFood.x) + (towardNewFood.y*towardNewFood.y));
//
//
//			if (distanceNewFood < distance)
//			{
//				closestFood = newFood;
//			}
//		}
//	}
//
//	glm::vec2 bestNextStep = destination;
//	for each(auto& node in neuralNetwork->testData)
//	{
//		float radius = sqrt(((node.x * gScreenWidth) - _position.x) * ((node.x * gScreenWidth) - _position.x) + ((node.y * gScreenHeight) - _position.y) * ((node.y * gScreenHeight) - _position.y));
//		if (radius < 30)
//		{
//			if (node.z == 2)
//			{
//				bestNextStep = glm::vec2((node.x * gScreenWidth), (node.y * gScreenHeight));
//				break;
//			}
//			if (node.z == 0)
//			{
//				glm::vec2 nextNodeToFood = closestFood - glm::vec2((node.x * gScreenWidth), (node.y * gScreenHeight));
//				float nextNodeToFoodDistance = sqrt((nextNodeToFood.x*nextNodeToFood.x) + (nextNodeToFood.y*nextNodeToFood.y));
//				glm::vec2 currentBestNode = closestFood - bestNextStep;
//				float currentBestNodeDistance = sqrt((currentBestNode.x*currentBestNode.x) + (currentBestNode.y*currentBestNode.y));
//				if (nextNodeToFoodDistance < currentBestNodeDistance)
//				{
//					bestNextStep = glm::vec2((node.x * gScreenWidth), (node.y * gScreenHeight));
//				}
//				break;
//			}
//			//if (node.z == 1)
//			//{
//
//			//	glm::vec2 nextNodeToFood = closestFood - glm::vec2((node.x * gScreenWidth), (node.y * gScreenHeight));
//			//	float nextNodeToFoodDistance = sqrt((nextNodeToFood.x*nextNodeToFood.x) + (nextNodeToFood.y*nextNodeToFood.y));
//			//	glm::vec2 currentBestNode = closestFood - bestNextStep;
//			//	float currentBestNodeDistance = sqrt((currentBestNode.x*currentBestNode.x) + (currentBestNode.y*currentBestNode.y));
//			//	if (nextNodeToFoodDistance < currentBestNodeDistance)
//			//	{
//			//		destination = bestNextStep;
//			//		_velocity = destination - _position;
//			//		float length = sqrt((_velocity.x*_velocity.x) + (_velocity.y*_velocity.y));
//			//		_velocity.x /= length;
//			//		_velocity.y /= length;
//			//		glm::vec2 avoidDirection = glm::vec2(_velocity.y * 30, _velocity.x * 30);
//			//		bestNextStep += avoidDirection;
//			//	}
//			//}
//		}
//	}
//
//	destination = bestNextStep;
//	_velocity = destination - _position;
//	float length = sqrt((_velocity.x*_velocity.x) + (_velocity.y*_velocity.y));
//	_velocity.x /= length;
//	_velocity.y /= length;
//
//
//}