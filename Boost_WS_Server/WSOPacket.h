/*
Because we are working with websockets, we want to store the dataframe
so we can access status codes and other information. We also need 
the dataframe later on for sending
*/

#pragma once
#include <OPacket.h>
#include <dataframe.h>
#include <vector>

class IPacket;

class WSOPacket : public OPacket
{
	friend class WSHeaderManager;
public:
	WSOPacket(IPacket* iPack, bool copyData);

	WSOPacket(const std::string& loc, IDType senderID = 0);

	WSOPacket(const std::string& loc, IDType senderID, IDType sendToID);

	boost::shared_ptr<websocket::dataframe> getDataframe()
	{
		return dataframe;
	}

	~WSOPacket();

protected:
	boost::shared_ptr<websocket::dataframe> dataframe;
};
