#pragma once

#include "BaseApplication.h"

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

	RakNet::RakPeerInterface* m_pPeerInterface;

	const char* IP = "127.0.0.1";
	const unsigned short PORT = 5456;
	unsigned int m_uiClientId;
	unsigned int m_uiClientObjectIndex;
	Camera*		m_camera;
	vec4 m_myColour;
	std::vector<GameObject> m_gameObjects;
};