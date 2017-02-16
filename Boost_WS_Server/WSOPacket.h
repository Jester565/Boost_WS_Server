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

	WSOPacket(const char* loc, IDType senderID = 0);

	WSOPacket(const char* loc, IDType senderID, IDType sendToID);

	boost::shared_ptr<websocket::dataframe> getDataframe()
	{
		return dataframe;
	}

	~WSOPacket();

protected:
	boost::shared_ptr<websocket::dataframe> dataframe;
};

