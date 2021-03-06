#pragma once

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>

#include <iostream>
#include <vector>

#include <Ogre.h>

#include "ItemBox.h"

#define EXPECTED_TIME_BETWEEN_NETWORK_UPDATES 0.05f

class Object
{
public:
	Ogre::Vector3 pos = Ogre::Vector3(0,0,0);
	Ogre::Quaternion rot = Ogre::Quaternion(1,0,0,0);

	float time = 0.0f;
	Ogre::Vector3 currentPos = Ogre::Vector3(0, 0, 0);
	Ogre::Vector3 futurePos = Ogre::Vector3(0, 0, 0);

	Ogre::Quaternion currentRot = Ogre::Quaternion(1, 0, 0, 0);
	Ogre::Quaternion futureRot = Ogre::Quaternion(1, 0, 0, 0);

	ITEM_BOX_TYPE item = IBT_NONE;
};

struct TimeObject
{
	int rank, id, minutes = 99, seconds = 99, ms = 99;
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

	std::vector<TimeObject> raceResults;

	bool raceComplete = false;

	Object* mConnectedPlayers;

	int id = 0;

private:
	RakNet::RakPeerInterface* mClient;
	RakNet::SocketDescriptor* mSockDesc;
	DataStructures::List<RakNet::RakNetSmartPtr<RakNet::RakNetSocket2>> mSockets;

	RakNet::Packet* mPacket;
	unsigned char mPacketID;
	
	

	const unsigned int MAX_CONNECTIONS = 10;
};