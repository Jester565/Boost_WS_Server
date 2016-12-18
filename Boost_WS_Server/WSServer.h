#pragma once
#include "WSHeaderManager.h"
#include <Server.h>

class WSServer : public Server
{
public:
	WSServer(boost::asio::ip::tcp ipVersion);

	HeaderManager* createHeaderManager() override;

	~WSServer();
};
