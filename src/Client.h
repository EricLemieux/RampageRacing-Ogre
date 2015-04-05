#pragma once

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>

#include <iostream>
#include <vector>

#include <Ogre.h>

#define EXPECTED_TIME_BETWEEN_NETWORK_UPDATES 0.5f

class Object
{
public:
	Ogre::Vector3 pos = Ogre::Vector3(0,0,0);
	Ogre::Quaternion rot = Ogre::Quaternion(1,0,0,0);

	float time = 0.0f;
	Ogre::Vector3 currentPos, futurePos;
};


class Client
{
public:
	Client();
	~Client();

	void Connect(const char* address, const unsigned int& serverPort, const unsigned int clientPort, const char* password);
	void SendString(const std::string &data);
	void Recieve();

	void Update(const float deltaTime, const unsigned int id);

	inline int GetID(){ return id; }

	inline Ogre::Vector3 GetPos(int id){ return mConnectedPlayers[id].pos; }
	inline Ogre::Quaternion GetRot(int id){ return mConnectedPlayers[id].rot; }

	int startingIndex=999;
	int totalPlayers;

	bool allReady = false;

	bool allDoneLoading = false;

	Ogre::String ip;

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