#pragma once

#include "BaseApplication.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include "GameMessages.h"
#include "Camera.h"
#include "Gizmos.h"
#include <vector>
#include "GameObject.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace RakNet {
	class RakPeerInterface;
}

class Camera;

class BasicNetworkingApplication : public BaseApplication {
public:
	BasicNetworkingApplication();
	virtual ~BasicNetworkingApplication();

	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);

	virtual void draw();

	//Initialise the connection
	void HandleNetworkConnection();
	void InitialiseClientConnection();

	//Handle incoming packets
	void HandleNetworkMessage();

private:
	void readObjectDataFromServer(RakNet::BitStream& bsIn);
	void createGameObject();
	void moveClientObject(float deltaTime);
	void sendUpdatedObjectPositionToServer(GameObject& myClientObject);

	RakNet::RakPeerInterface* m_pPeerInterface;

	const char* IP = "127.0.0.1";
	const unsigned short PORT = 5456;
	unsigned int m_uiClientId;
	unsigned int m_uiClientObjectIndex;
	Camera*		m_camera;
	glm::vec4 m_myColour;
	glm::vec3	m_pickPosition;
	std::vector<GameObject> m_gameObjects;
};