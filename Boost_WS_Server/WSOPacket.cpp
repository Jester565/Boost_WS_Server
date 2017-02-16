#include "WSOPacket.h"
#include "WSIPacket.h"

WSOPacket::WSOPacket(IPacket* iPack, bool copyData)
	:OPacket(iPack, copyData)
{
	WSIPacket* wsIPack = (WSIPacket*)iPack;
	dataframe = wsIPack->getDataframe();
	if (copyData)
	{
		data = wsIPack->getData();
	}
}

WSOPacket::WSOPacket(const char * loc, IDType senderID)
		:OPacket(loc, senderID)
{
		dataframe = boost::make_shared<websocket::dataframe>();
}

WSOPacket::WSOPacket(const char* loc, IDType senderID, IDType sendToID)
		: OPacket(loc, senderID, sendToID)
{
		dataframe = boost::make_shared<websocket::dataframe>();
}

WSOPacket::~WSOPacket()
{
}
