#include "WSServer.h"
#include <iostream>

int main()
{
	Server* server = new WSServer();
	server->createManagers();
	server->run(8443);
	std::string str;
	std::cin >> str;
}