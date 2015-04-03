#pragma once

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>

#include <iostream>
#include <vector>

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

public:
	std::vector<Object> mConnectedPlayers;

	unsigned int currentlyConnectedID = 0;
};