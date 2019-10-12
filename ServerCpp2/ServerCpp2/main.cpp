#include "Server.h"
#include "windows.h"
#include "iostream"

Server* server;

void startServer()
{
	server->startServerLoop();
}

int main()
{
	server = new Server();

	thread startServerThread(startServer);

	while (server->getLastError() == NICE)
	{
		Sleep(500);
	}

	say "Server has error\n";
	delete server;

	return 0;
}