#include "Client.h"

Client::Client()
{
	//TODO REMOVE
	//id = 0;
	//for (unsigned int i = 0; i < MAX_CONNECTIONS; ++i)
	//{
	//	mConnectedPlayers.push_back(Object());
	//}
}
Client::~Client()
{
	RakNet::RakPeerInterface::DestroyInstance(mClient);
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

void Client::Recieve()
{
	while ((mPacket = mClient->Receive()) != NULL)
	{
		std::string str = std::string((char*)mPacket->data).substr(0, mPacket->length);

		auto p = str.find(" ");
		std::string phrase = str.substr(0, p);

		if (phrase == "pos")
		{
			int id = 0;
			float pos[3];

			sscanf_s(str.c_str(), "%*[^0-9]%d %f %f %f", &id, &pos[0], &pos[1], &pos[2]);

			mConnectedPlayers[id].pos = Ogre::Vector3(pos);
		}
		else if (phrase == "rot")
		{
			int id = 0;
			float rot[4];

			sscanf_s(str.c_str(), "%*[^0-9]%d %f %f %f %f", &id, &rot[0], &rot[1], &rot[2], &rot[3]);

			mConnectedPlayers[id].rot = Ogre::Quaternion(rot[3], rot[0], rot[1], rot[2]);
		}
		else if (phrase == "startIndex")
		{
			sscanf_s(str.c_str(), "%*[^0-9]%d", &startingIndex);
		}
		else if (phrase == "totalPlayers")
		{
			sscanf_s(str.c_str(), "%*[^0-9]%d", &totalPlayers);

			mConnectedPlayers = new Object[totalPlayers];
		}
		else if (phrase == "start")
		{
			allReady = true;
		}

		mClient->DeallocatePacket(mPacket);
	}
}