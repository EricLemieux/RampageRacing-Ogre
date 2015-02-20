#pragma once

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>

#include <iostream>

class Client
{
public:
	Client();
	~Client();

	void Connect(const char* address, const unsigned int& serverPort, const unsigned int clientPort, const char* password);
	void SendString(const std::string &data);
	void RecieveString();

private:
	RakNet::RakPeerInterface* mClient;
	RakNet::SocketDescriptor* mSockDesc;
	DataStructures::List<RakNet::RakNetSmartPtr<RakNet::RakNetSocket2>> mSockets;

	RakNet::Packet* mPacket;
	unsigned char mPacketID;
};