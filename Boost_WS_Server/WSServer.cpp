#include "WSServer.h"

WSServer::WSServer()
	:Server()
{
}

HeaderManager* WSServer::createHeaderManager()
{
	return new WSHeaderManager(this);
}

WSServer::~WSServer()
{
}
