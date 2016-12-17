#include "WSServer.h"

WSServer::WSServer()
	:Server(boost::asio::ip::tcp::v4())
{
}

HeaderManager* WSServer::createHeaderManager()
{
	return new WSHeaderManager(this);
}

WSServer::~WSServer()
{
}
