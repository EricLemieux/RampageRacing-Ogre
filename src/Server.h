#pragma once

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>

#include <iostream>
#include <vector>

#include <time.h>

struct TimeObject
{
	int id, minutes = 99, seconds = 99, ms = 99;
};

struct vec3
{
	float x, y, z;
};

struct quaternion
{
	float x, y, z, w;
};

class Object
{
public:
	vec3 pos;
	quaternion rot;
};

class Server
{
public:
	Server();
	~Server();

	void Activate(const char* password, const unsigned int& port, const unsigned int& maxConnections);
	void SendString(const std::string &data, bool sendToEveryone = true);
	void SendPosUpdates();
	void RecieveString();

	int numReady = 0;

private:
	RakNet::RakPeerInterface* mServer;
	RakNet::SocketDescriptor* mSocketDesc;
	DataStructures::List<RakNet::RakNetSmartPtr<RakNet::RakNetSocket2>> mSockets;
	RakNet::Packet* mPacket;
	char mPacketID;

	RakNet::SystemAddress lastSender;

	int playersDoneLoading=0;

	std::vector<TimeObject> finishTimes;

	unsigned int currentRank = 1;

	int seed;

public:
	std::vector<Object> mConnectedPlayers;

	unsigned int currentlyConnectedID = 0;
};