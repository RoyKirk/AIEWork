#include "Server.h"

Server::Server() {
	// initialize the Raknet peer interface first
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();

	m_uiConnectionCounter = 1;
	m_uiObjectCounter = 1;
}

Server::~Server() {

}

void Server::run() {

	// startup the server, and start it listening to clients
	std::cout << "Starting up the server..." << std::endl;

	// create a socket descriptor to describe this connection
	RakNet::SocketDescriptor sd(PORT, 0);

	// now call startup - max of 32 connections, on the assigned port
	m_pPeerInterface->Startup(32, &sd, 1);
	m_pPeerInterface->SetMaximumIncomingConnections(32);

	handleNetworkMessages();
}

void Server::handleNetworkMessages() {
	RakNet::Packet* packet = nullptr;

	while (true) {
		for (	packet = m_pPeerInterface->Receive();
				packet;
				m_pPeerInterface->DeallocatePacket(packet), packet = m_pPeerInterface->Receive()) {

			switch (packet->data[0]) {
			case ID_NEW_INCOMING_CONNECTION: 
			{
				addNewConnection(packet->systemAddress);
				std::cout << "A connection is incoming.\n";
				break;
			}
			case ID_DISCONNECTION_NOTIFICATION:
			{
				std::cout << "A client has disconnected.\n";
				removeConnection(packet->systemAddress);
				break;
			}
			case ID_CONNECTION_LOST:
			{
				std::cout << "A client lost the connection.\n";
				removeConnection(packet->systemAddress);
				break;
			}
			case ID_CLIENT_CREATE_OBJECT:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				createNewObject(bsIn, packet->systemAddress);
				break;
			}
			case ID_CLIENT_UPDATE_OBJECT_POSITION:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				updateObject(bsIn, packet->systemAddress);
				break;
			}
			default:
				std::cout << "Received a message with a unknown id: " << packet->data[0];
				break;
			}
		}
	}
}

void Server::addNewConnection(RakNet::SystemAddress systemAddress) {
	ConnectionInfo info;
	info.sysAddress = systemAddress;
	info.uiConnectionID = m_uiConnectionCounter++;
	m_connectedClients[info.uiConnectionID] = info;

	sendClientIDToClient(info.uiConnectionID);
}

void Server::removeConnection(RakNet::SystemAddress systemAddress) {
	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++) {
		if (it->second.sysAddress == systemAddress) {
			m_connectedClients.erase(it);
			break;
		}
	}
}

unsigned int Server::systemAddressToClientID(RakNet::SystemAddress& systemAddress) {
	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++) {
		if (it->second.sysAddress == systemAddress) {
			return it->first;
		}
	}

	return 0;
}


void Server::sendClientIDToClient(unsigned int uiClientID) {
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_CLIENT_ID);
	bs.Write(uiClientID);

	m_pPeerInterface->Send(&bs, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, m_connectedClients[uiClientID].sysAddress, false);
}

void Server::createNewObject(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress)
{
	GameObject newGameObject;

	//Read in the information from the packet
	bsIn.Read(newGameObject.fXPos);
	bsIn.Read(newGameObject.fZPos);
	bsIn.Read(newGameObject.fRedColour);
	bsIn.Read(newGameObject.fGreenColour);
	bsIn.Read(newGameObject.fBlueColour);

	newGameObject.uiOwnerClientID = systemAddressToClientID(ownerSysAddress);
	newGameObject.uiObjectID = m_uiObjectCounter++;

	m_gameObjects.push_back(newGameObject);
	//sendGameObjectToAllClients(newGameObject, ownerSysAddress);
}

void Server::updateObject(RakNet::BitStream& bsIn, RakNet::SystemAddress& ownerSysAddress)
{
	unsigned int tempuiObjectID;
	bsIn.Read(tempuiObjectID);
	tempuiObjectID -= 1;
	if (m_gameObjects[tempuiObjectID].uiOwnerClientID == systemAddressToClientID(ownerSysAddress))
	{
		//Read in the information from the packet
		bsIn.Read(m_gameObjects[tempuiObjectID].fXPos);
		bsIn.Read(m_gameObjects[tempuiObjectID].fZPos);
		bsIn.Read(m_gameObjects[tempuiObjectID].fRedColour);
		bsIn.Read(m_gameObjects[tempuiObjectID].fGreenColour);
		bsIn.Read(m_gameObjects[tempuiObjectID].fBlueColour);
		//sendGameObjectToAllClients(m_gameObjects[tempuiObjectID], ownerSysAddress);
	}
}

void Server::sendGameObjectToAllClients(GameObject& gameObject, RakNet::SystemAddress& ownerSystemAddress)
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)GameMessages::ID_SERVER_FULL_OBJECT_DATA);
	bsOut.Write(gameObject.fXPos);
	bsOut.Write(gameObject.fZPos);
	bsOut.Write(gameObject.fRedColour);
	bsOut.Write(gameObject.fGreenColour);
	bsOut.Write(gameObject.fBlueColour);
	bsOut.Write(gameObject.uiOwnerClientID);
	bsOut.Write(gameObject.uiObjectID);

	m_pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, ownerSystemAddress, true);
}