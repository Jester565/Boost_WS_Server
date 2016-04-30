#include "WSServer.h"
#include <iostream>

int main()
{
	Server* server = new WSServer();
	server->createManagers();
	server->syncInitServer(24560);
}