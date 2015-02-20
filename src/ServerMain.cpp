//Server for the game

#include "Server.h"

int main(int argc, char** argv)
{
	Server gameServer;
	gameServer.Activate("password", 8080, 8);

	//While the server should be running
	while (true)
	{
		//Take in the position data from the client 
		gameServer.RecieveString();

		//Reply with information about all the other clients
		gameServer.SendString("test");
	}

	return 0;
}