#include "Server.h"

Server::Server()
{

}

Server::~Server()
{

}

void Server::Activate(const char* password, const unsigned int& port, const unsigned int& maxConnections)
{
	mSocketDesc = new RakNet::SocketDescriptor(port, NULL);
	mSocketDesc->socketFamily = AF_INET;

	mServer = RakNet::RakPeerInterface::GetInstance();
	RakNet::StartupResult res = mServer->Startup(maxConnections, mSocketDesc, 1);

	mServer->SetMaximumIncomingConnections(maxConnections);

	assert(res == RakNet::RAKNET_STARTED && "Server Could Not Start.\n");

	mServer->SetIncomingPassword(password, strlen(password));
}

void Server::SendString(const std::string &data)
{
	mServer->Send(data.c_str(), data.length(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Server::RecieveString()
{
	while ((mPacket == mServer->Receive()) != NULL)
	{
		std::string str = std::string((char*)mPacket->data).substr(0, mPacket->length);

		//TODO:
		//Store the data or something for later use.

		mServer->DeallocatePacket(mPacket);
	}
}