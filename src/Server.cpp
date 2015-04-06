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

void Server::SendString(const std::string &data, bool sendToEveryone)
{
	if (sendToEveryone)
		mServer->Send(data.c_str(), data.length(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_RAKNET_GUID, sendToEveryone);
	else
		mServer->Send(data.c_str(), data.length(), HIGH_PRIORITY, RELIABLE_ORDERED, 0, lastSender, sendToEveryone);
}

void Server::SendPosUpdates()
{
	unsigned int count = mConnectedPlayers.size();
	for (unsigned int i = 0; i < count; ++i)
	{
		if (i != currentlyConnectedID)
		{
			//Send the position
			char pos[256];
			sprintf_s(pos, "pos %d %f %f %f", i, mConnectedPlayers[i].pos.x, mConnectedPlayers[i].pos.y, mConnectedPlayers[i].pos.z);
			SendString(pos, false);
			
			//Send the rotation quaternion
			char rot[256];
			sprintf_s(rot, "rot %d %f %f %f %f", i, mConnectedPlayers[i].rot.x, mConnectedPlayers[i].rot.y, mConnectedPlayers[i].rot.z, mConnectedPlayers[i].rot.w);
			SendString(rot, false);
		}
	}
}

void Server::RecieveString()
{
	while ((mPacket = mServer->Receive()) != NULL)
	{
		lastSender = mPacket->systemAddress;
		if (mPacket->data[0] == ID_NEW_INCOMING_CONNECTION)
		{
			std::cout << "A remote system has connected.\n";
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
				vec3 pos;
				sscanf_s(str.c_str(), "%*[^0-9]%d %f %f %f", &id, &pos.x, &pos.y, &pos.z);

				mConnectedPlayers[id].pos = pos;

				currentlyConnectedID = id;

				SendPosUpdates();
			}
			else if (phrase == "rot")
			{
				int id = 0;
				quaternion rot;
				sscanf_s(str.c_str(), "%*[^0-9]%d %f %f %f %f", &id, &rot.x, &rot.y, &rot.z, &rot.w);

				mConnectedPlayers[id].rot = rot;

				currentlyConnectedID = id;
			}
			else if (phrase == "addPlayers")
			{
				unsigned int local = 0;
				sscanf_s(str.c_str(), "%*[^0-9]%d", &local);

				unsigned int startingIndex = mConnectedPlayers.size();

				for (unsigned int p = 0; p < local; ++p)
				{
					Object newPlayer;
					mConnectedPlayers.push_back(newPlayer);
				}

				char buffer[32];
				sprintf_s(buffer, "startIndex %d", startingIndex);
				std::cout << buffer<<"\n";
				SendString(buffer, false);				
			}
			else if (phrase == "ready")
			{
				numReady++;

				std::cout << numReady << " == " << mConnectedPlayers.size() << "\n";
				if (numReady == mConnectedPlayers.size())
				{
					//Send the number of players
					char buffer[32];
					sprintf_s(buffer, "totalPlayers %d", mConnectedPlayers.size());
					SendString(buffer, true);

					std::cout << buffer << "\n";

					SendString("start", true);
				}
			}
			else if (phrase == "notready")
			{
				numReady--;
			}
			else if (phrase == "doneLoading")
			{
				unsigned int players = 0;
				sscanf_s(str.c_str(), "%*[^0-9]%d", &players);

				playersDoneLoading += players;

				std::cout << playersDoneLoading << " >= " << mConnectedPlayers.size() << "\n";
				if (playersDoneLoading >= mConnectedPlayers.size())
				{
					std::cout << "everyone done loading.\n";
					SendString("everyoneDoneLoading", true);
				}
			}
			else if (phrase == "reset")
			{
				mConnectedPlayers.clear();
			}
			else if (phrase == "result")
			{
				TimeObject temp;
				char value[32];

				std::cout << str.c_str() << "\n";

				sscanf_s(str.c_str(), "%*[^0-9]%d %d %d %d", &temp.id, &temp.minutes, &temp.seconds, &temp.ms);

				//Send back the rank, id, and time for the car to the other clients
				char buffer[64];
				sprintf_s(buffer, 64, "res %d %d %d %d %d", currentRank, temp.id, temp.minutes, temp.seconds, temp.ms);
				SendString(buffer);

				finishTimes.push_back(temp);

				currentRank++;

				std::cout << "race complete?\n" << finishTimes.size() << " == " << mConnectedPlayers.size() << "\n";
				if (finishTimes.size() == mConnectedPlayers.size())
				{
					SendString("raceComplete");
				}
			}
			else if (phrase == "item")
			{
				unsigned int i, item;
				sscanf_s(str.c_str(), "%*[^0-9]%d %d", &i, &item);

				SendString(str.c_str());
			}
			else
			{
				std::cout << str.c_str();
			}
		}

		mServer->DeallocatePacket(mPacket);
	}
}