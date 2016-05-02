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
		//AStar();
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
	_maxSpeed = 400;
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
		int numberHiddenNeurons = 40;  //the tutorial notes claim that there are three neurons in hte hidden layer but performance is rather poor with only three and four works better. Try three as an experiment
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
	//int randSelector = rand() % 4;
	//if (randSelector == 0)
	//{
	//	_startingPosition.x = 20;
	//	_startingPosition.y = 20;
	//}
	//else if (randSelector == 1)
	//{
	//	_startingPosition.x = 20;
	//	_startingPosition.y = gScreenHeight-20;
	//}
	//else if (randSelector == 2)
	//{
	//	_startingPosition.x = gScreenWidth-20;
	//	_startingPosition.y = 20;
	//}
	//else
	//{
	//	_startingPosition.x = gScreenWidth-20;
	//	_startingPosition.y = gScreenHeight-20;
	//}
	
	_startingPosition.x = gScreenWidth;
	_startingPosition.y = gScreenHeight;

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
				//direction.x /= distance;
				//direction.y /= distance;

				_velocity -= glm::vec2(direction.x*(1 / distance), direction.y*(1 / distance));
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
				break;
			}
		}

	}
}

//void Agent::AStar()
//{
//	glm::vec3 endNode = memory.front();
//	if (!closedList.empty() && closedList.front().x == endNode.x && closedList.front().y == endNode.y && pathList.empty())
//	{
//		pathList.push_front(closedList.front());
//		while (pathList.front() != pGraph.GetNode(START))
//		{
//			Graph::Node* currentNode = pathList.front().parent;
//			pathList.push_front(currentNode);
//		}
//	}
//	else if (!closedList.empty() && closedList.front().x == endNode.x && closedList.front().y == endNode.y)
//	{
//	}
//	else
//	{
//		if (!openList.empty())
//		{
//			openList.sort([](const Graph::Node* a, const Graph::Node* b) { return a.f < b.f; });
//			Graph::Node* currentNode = openList.front();
//			openList.pop_front();
//
//			for each (Graph::Edge c in currentNode.connections)
//			{
//				if (c.connection.wall)
//				{
//					continue;
//				}
//				bool foundClosed = (std::find(closedList.begin(), closedList.end(), c.connection) != closedList.end());
//				if (!foundClosed)
//				{
//					bool foundOpen = (std::find(openList.begin(), openList.end(), c.connection) != openList.end());
//					if (!foundOpen)
//					{
//						//Manhattan Distance
//						//c.connection.g = currentNode.g + abs(c.connection.x - currentNode.x) + abs(c.connection.y - currentNode.y);
//						//c.connection.h = abs(endNode.x - c.connection.x) + abs(endNode.y - c.connection.y);
//						//c.connection.f = c.connection.g + c.connection.h;
//
//						//Euclidian Distance
//						c.connection.g = currentNode.g + sqrt((c.connection.x - currentNode.x)*(c.connection.x - currentNode.x)
//							+ (c.connection.y - currentNode.y)*(c.connection.y - currentNode.y));
//						c.connection.h = sqrt((endNode.x - c.connection.x)*(endNode.x - c.connection.x)
//							+ (endNode.y - c.connection.y)*(endNode.y - c.connection.y));
//						c.connection.f = c.connection.g + c.connection.h;
//
//						c.connection.parent = currentNode;
//						openList.push_front(c.connection);
//					}
//					else
//					{
//						for each(Graph::Node* currentCandidate in openList)
//						{
//							if (c.connection.x == currentCandidate.x
//								&& c.connection.y == currentCandidate.y
//								&& currentCandidate.g > c.connection.g)
//							{
//								currentCandidate.parent = currentNode;
//
//								//Manhattan Distance
//								//currentCandidate.g = currentNode.g + abs(currentCandidate.x - currentNode.x) + abs(currentCandidate.y - currentNode.y);
//								//currentCandidate.h = abs(endNode.x - currentCandidate.x) + abs(endNode.y - currentCandidate.y);
//								//currentCandidate.f = currentCandidate.g + currentCandidate.h;
//
//
//								//Euclidian Distance renew f,g,h scores
//								currentCandidate.g = currentNode.g + sqrt((currentCandidate.x - currentNode.x)*(currentCandidate.x - currentNode.x)
//									+ (currentCandidate.y - currentNode.y)*(currentCandidate.y - currentNode.y));
//								currentCandidate.h = sqrt((endNode.x - currentCandidate.x)*(endNode.x - currentCandidate.x)
//									+ (endNode.y - currentCandidate.y)*(endNode.y - currentCandidate.y));
//								currentCandidate.f = currentCandidate.g + currentCandidate.h;
//							}
//						}
//					}
//				}
//			}
//			closedList.push_front(currentNode);
//		}
//	}
//}

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