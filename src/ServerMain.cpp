//Server for the game

#include "Server.h"

int main(int argc, char** argv)
{
	Server gameServer;
	gameServer.Activate("password", 8080, 8);

	float p[3] = { 0.0f, -50.0f, 0.0f };

	//While the server should be running
	while (true)
	{
		//Take in the position data from the client 
		gameServer.RecieveString();

		//TEMP
		//DELETE
		if (gameServer.mConnectedPlayers.size() > 1)
		{
			p[1] += 0.01f;
			char s[256];
			sprintf_s(s, 256, "%s %d %f %f %f", "pos", 1, p[0], p[1], p[2]);
			gameServer.mConnectedPlayers[1].pos = std::string(s);
			std::cout << s << "\n";
		}

		//Reply with information about all the other clients
		gameServer.SendPosUpdates();
	}

	return 0;
}