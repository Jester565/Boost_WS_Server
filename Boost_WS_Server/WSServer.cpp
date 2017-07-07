#include "WSServer.h"

WSServer::WSServer(boost::asio::ip::tcp ipVersion)
	:Server(ipVersion)
{
}

HeaderManager* WSServer::createHeaderManager()
{
	return new WSHeaderManager(this);
}

WSServer::~WSServer()
{
}