#include "Client.h"

Client::Client()
{

}
Client::~Client()
{

}

void Client::Connect(const char* address, const unsigned int& serverPort, const unsigned int clientPort, const char* password)
{
	mSockDesc = new RakNet::SocketDescriptor(clientPort, NULL);
	mSockDesc->socketFamily = AF_INET;

	mClient = RakNet::RakPeerInterface::GetInstance();
	RakNet::StartupResult res = mClient->Startup(1, mSockDesc, 1);

	assert(res == RakNet::RAKNET_STARTED && "client Count Not Connect.\n");

	RakNet::ConnectionAttemptResult conRes = mClient->Connect(address, serverPort, password, strlen(password));

	assert(conRes == RakNet::CONNECTION_ATTEMPT_STARTED && "Client could not connect.\n");
}

void Client::SendString(const std::string &data)
{
	mClient->Send(data.c_str(), data.length(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Client::RecieveString()
{
	while ((mPacket == mClient->Receive()) != NULL)
	{
		std::string str = std::string((char*)mPacket->data).substr(0, mPacket->length);

		//TODO:
		//Store the information and have it be used by the game

		mClient->DeallocatePacket(mPacket);
	}
}