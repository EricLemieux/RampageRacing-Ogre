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

	std::cout << "Server Started.\n";

	mServer->SetIncomingPassword(password, strlen(password));
}

void Server::SendString(const std::string &data)
{
	mServer->Send(data.c_str(), data.length(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Server::RecieveString()
{
	while ((mPacket = mServer->Receive()) != NULL)
	{
		if (mPacket->data[0] == ID_NEW_INCOMING_CONNECTION)
		{
			std::cout << "A remote system has connected.\n";

			Object newPlayer;
			mConnectedPlayers.push_back(newPlayer);
		}
		else if (mPacket->data[0] == ID_DISCONNECTION_NOTIFICATION)
		{
			std::cout << "A remote system has been disconnected.\n";
		}
		else if (mPacket->data[0] == ID_CONNECTION_LOST)
		{
			std::cout << "A remote system has lost it's connected.\n";
		}
		else
		{
			std::string str = std::string((char*)mPacket->data).substr(0, mPacket->length);

			auto p = str.find(" ");
			std::string phrase = str.substr(0, p);

			if (phrase == "pos")
			{
				int id = 0;
				sscanf_s(str.c_str(), "%*[^0-9]%d", &id);

				mConnectedPlayers[id].pos = str;
			}
			else if (phrase == "rot")
			{
				int id = 0;
				sscanf_s(str.c_str(), "%*[^0-9]%d", &id);

				mConnectedPlayers[id].rot = str;
			}
			else if (phrase == "something")
			{
			}
			else if (phrase == "reset")
			{
				mConnectedPlayers.clear();
			}
			else
			{
				std::cout << str.c_str();
			}
		}

		mServer->DeallocatePacket(mPacket);
	}
}