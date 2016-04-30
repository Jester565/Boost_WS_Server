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

WSOPacket::~WSOPacket()
{
}
