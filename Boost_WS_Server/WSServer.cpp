#include "WSServer.h"

//test 123 

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
