//Server for the game

#include "Server.h"

int main(int argc, char** argv)
{
	Server gameServer;
	gameServer.Activate("password", 8080, 8);

	//While the server should be running
	while (true)
	{
		if (gameServer.mode == sm_menu)
		{
			//Take in the position data from the client 
			gameServer.RecieveString();
		}
		else if (gameServer.mode == sm_gameplay)
		{
			//Take in the position data from the client 
			gameServer.RecieveString();

			//Reply with information about all the other clients
			gameServer.SendPosUpdates();
		}
	}

	return 0;
}