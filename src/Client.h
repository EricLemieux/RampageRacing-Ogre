#pragma once

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>

#include <iostream>
#include <vector>

#include <Ogre.h>

class Object
{
public:
	Ogre::Vector3 pos;
	Ogre::Quaternion rot;
};


class Client
{
public:
	Client();
	~Client();

	void Connect(const char* address, const unsigned int& serverPort, const unsigned int clientPort, const char* password);
	void SendString(const std::string &data);
	void Recieve();

	inline int GetID(){ return id; }

	inline Ogre::Vector3 GetPos(int id){ return mConnectedPlayers[id].pos; }

	int startingIndex;
	int totalPlayers;

	bool allReady = false;

private:
	RakNet::RakPeerInterface* mClient;
	RakNet::SocketDescriptor* mSockDesc;
	DataStructures::List<RakNet::RakNetSmartPtr<RakNet::RakNetSocket2>> mSockets;

	RakNet::Packet* mPacket;
	unsigned char mPacketID;
	
	int id=0;

	Object* mConnectedPlayers;
	const unsigned int MAX_CONNECTIONS = 10;
};