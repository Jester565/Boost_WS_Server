#include "WSServer.h"
#include <iostream>

int main()
{
	Server* server = new WSServer(boost::asio::ip::tcp::v4());
	server->createManagers();
	server->run(8443);
	std::string str;
	std::cin >> str;
}