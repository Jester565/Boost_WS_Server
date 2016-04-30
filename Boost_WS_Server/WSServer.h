#pragma once
#include "WSHeaderManager.h"
#include <Server.h>

class WSServer : public Server
{
public:
	WSServer();

	HeaderManager* createHeaderManager() override;

	~WSServer();
};

