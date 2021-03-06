#include "BasicNetworkingApplication.h"


using glm::vec3;
using glm::vec4;
using glm::mat4;

BasicNetworkingApplication::BasicNetworkingApplication() : m_camera(nullptr) {
	int colour1, colour2, colour3;

	/* initialize random seed: */
	srand(time(NULL));

	/* generate secret number between 1 and 10: */
	colour1 = rand() % 100 + 1;
	colour2 = rand() % 100 + 1;
	colour3 = rand() % 100 + 1;

	m_myColour = vec4((float)colour1 / 100, (float)colour2 / 100, (float)colour3 / 100, 1);

	m_uiClientObjectIndex = 0;
}

BasicNetworkingApplication::~BasicNetworkingApplication() {

}

bool BasicNetworkingApplication::startup() {
	// setup the basic window
	createWindow("Client Application", 1280, 720);

	HandleNetworkConnection();

	// start the gizmo system that can draw basic shapes
	Gizmos::create();

	// create a camera
	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
	m_camera->setLookAtFrom(vec3(10, 10, 10), vec3(0));

	return true;
}

void BasicNetworkingApplication::shutdown() {
	// delete our camera and cleanup gizmos
	delete m_camera;
	Gizmos::destroy();

	// destroy our window properly
	destroyWindow();
}

bool BasicNetworkingApplication::update(float deltaTime) {
	// close the application if the window closes
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	HandleNetworkMessage();

	// update the camera's movement
	m_camera->update(deltaTime);

	// clear the gizmos out for this frame
	Gizmos::clear();

	if (glfwGetMouseButton(m_window, 0) == GLFW_PRESS) {
		double x = 0, y = 0;
		glfwGetCursorPos(m_window, &x, &y);

		// plane represents the ground, with a normal of (0,1,0) and a distance of 0 from (0,0,0)
		glm::vec4 plane(0, 1, 0, 0);
		m_pickPosition = m_camera->pickAgainstPlane((float)x, (float)y, plane);
	}
	Gizmos::addTransform(glm::translate(m_pickPosition));

	for (int i = 0; i < m_gameObjects.size(); i++)
	{
		GameObject& obj = m_gameObjects[i];
		Gizmos::addSphere(glm::vec3(obj.fXPos, 2, obj.fZPos), 2, 32, 32, glm::vec4(obj.fRedColour, obj.fGreenColour, obj.fBlueColour, 1), nullptr);
	}

	moveClientObject(deltaTime);

	return true;
}

void BasicNetworkingApplication::draw() {
	// clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//////////////////////////////////////////////////////////////////////////
	// DRAW YOUR THINGS HERE
	//////////////////////////////////////////////////////////////////////////

	// display the 3D gizmos
	Gizmos::draw(m_camera->getProjectionView());

	// get a orthographic projection matrix and draw 2D gizmos
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0, 0, (float)width, (float)height);

	Gizmos::draw2D(m_camera->getProjectionView());
}


void BasicNetworkingApplication::HandleNetworkConnection()
{
	//Initialise the Raknet peer interface first
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();
	InitialiseClientConnection();
}

void BasicNetworkingApplication::InitialiseClientConnection()
{
	//Create a socket descriptor to describe this connection
	//No data needed, as we will be connecting to a server
	RakNet::SocketDescriptor sd;

	//Now call startup - max of 1 connections (to the server)
	m_pPeerInterface->Startup(1, &sd, 1);

	std::cout << "Connecting to server at: " << IP << std::endl;

	//Now call connect to attempt to connect to the given server
	RakNet::ConnectionAttemptResult res = m_pPeerInterface->Connect(IP, PORT, nullptr, 0);

	//Finally, check to see if we connected, and if not, throw an error
	if (res != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		std::cout << "Unable to start connection, Error number: " << res << std::endl;
	}
}

void BasicNetworkingApplication::HandleNetworkMessage()
{
	RakNet::Packet* packet;

	for (packet = m_pPeerInterface->Receive(); packet; m_pPeerInterface->DeallocatePacket(packet), packet = m_pPeerInterface->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			std::cout << "Another client has disconnected.\n";
			break;
		case ID_REMOTE_CONNECTION_LOST:
			std::cout << "Another client lost the connection.\n";
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			std::cout << "Another client has connected.\n";
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			std::cout << "Our connection request has been accepted.\n";
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			std::cout << "The server is full.\n";
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			std::cout << "We have disconnected.\n";
			break;
		case ID_CONNECTION_LOST:
			std::cout << "Connection lost.\n";
			break;
		case ID_SERVER_TEXT_MESSAGE:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			RakNet::RakString str;
			bsIn.Read(str);
			std::cout << str.C_String() << std::endl;
			break;
		}
		case ID_SERVER_CLIENT_ID:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(m_uiClientId);

			std::cout << "Server has given us an id of: " << m_uiClientId << std::endl;

			createGameObject();

			break;
		}
		case ID_SERVER_FULL_OBJECT_DATA:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			readObjectDataFromServer(bsIn);
			break;
		}
		default:
			std::cout << "Received a message with an unknown id: " << packet->data[0];
			break;
		}
	}
}


void BasicNetworkingApplication::readObjectDataFromServer(RakNet::BitStream& bsIn)
{
	GameObject tempGameObject;

	bsIn.Read(tempGameObject.fXPos);
	bsIn.Read(tempGameObject.fZPos);
	bsIn.Read(tempGameObject.fRedColour);
	bsIn.Read(tempGameObject.fGreenColour);
	bsIn.Read(tempGameObject.fBlueColour);
	bsIn.Read(tempGameObject.uiOwnerClientID);
	bsIn.Read(tempGameObject.uiObjectID);

	bool bFound = false;
	for (int i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i].uiObjectID == tempGameObject.uiObjectID)
		{
			bFound = true;

			GameObject& obj = m_gameObjects[i];
			obj.fXPos = tempGameObject.fXPos;
			obj.fZPos = tempGameObject.fZPos;
			obj.fRedColour = tempGameObject.fRedColour;
			obj.fGreenColour = tempGameObject.fGreenColour;
			obj.fBlueColour = tempGameObject.fBlueColour;
			obj.uiOwnerClientID = tempGameObject.uiOwnerClientID;
		}
	}

	if (!bFound)
	{
		m_gameObjects.push_back(tempGameObject);
		if (tempGameObject.uiOwnerClientID == m_uiClientId)
		{
			m_uiClientObjectIndex = m_gameObjects.size() - 1;
		}
	}

}

void BasicNetworkingApplication::createGameObject()
{
	RakNet::BitStream bsOut;

	GameObject tempGameObject;
	tempGameObject.fXPos = 0.0f;
	tempGameObject.fZPos = 0.0f;
	tempGameObject.fRedColour = m_myColour.r;
	tempGameObject.fGreenColour = m_myColour.g;
	tempGameObject.fBlueColour = m_myColour.b;
	tempGameObject.uiOwnerClientID = m_uiClientId;
	tempGameObject.uiObjectID = m_uiClientObjectIndex;

	m_gameObjects.push_back(tempGameObject);
	
	bsOut.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CREATE_OBJECT);
	bsOut.Write(tempGameObject.fXPos);
	bsOut.Write(tempGameObject.fZPos);
	bsOut.Write(tempGameObject.fRedColour);
	bsOut.Write(tempGameObject.fGreenColour);
	bsOut.Write(tempGameObject.fBlueColour);

	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void BasicNetworkingApplication::moveClientObject(float deltaTime)
{
	//We don't have a valid client ID, so we have no game object
	if (m_uiClientId == 0) { return; }

	//No game objects sent to us, so we don't know who we are yet
	if (m_gameObjects.size() == 0) { return; }

	bool bUpdateObjectPosition = false;

	GameObject& myClientObject = m_gameObjects[m_uiClientObjectIndex];

	if (glfwGetKey(m_window, GLFW_KEY_UP))
	{
		myClientObject.fZPos += 2 * deltaTime;
		bUpdateObjectPosition = true;
	}
	if (glfwGetKey(m_window, GLFW_KEY_DOWN))
	{
		myClientObject.fZPos -= 2 * deltaTime;
		bUpdateObjectPosition = true;
	}
	
	if (bUpdateObjectPosition == true)
	{
		sendUpdatedObjectPositionToServer(myClientObject);
	}
}

void  BasicNetworkingApplication::sendUpdatedObjectPositionToServer(GameObject& myClientObject)
{
	RakNet::BitStream bsOut;

	bsOut.Write((RakNet::MessageID)GameMessages::ID_CLIENT_UPDATE_OBJECT_POSITION);
	bsOut.Write(myClientObject.uiObjectID);
	bsOut.Write(myClientObject.fXPos);
	bsOut.Write(myClientObject.fZPos);
	bsOut.Write(myClientObject.fRedColour);
	bsOut.Write(myClientObject.fGreenColour);
	bsOut.Write(myClientObject.fBlueColour);


	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}