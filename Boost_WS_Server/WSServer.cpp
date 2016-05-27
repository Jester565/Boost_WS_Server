#include "WSServer.h"

//test 123 asdf

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
