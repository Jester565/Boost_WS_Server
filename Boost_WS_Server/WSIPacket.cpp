#include "WSIPacket.h"

WSIPacket::WSIPacket()
	:IPacket()
{
}

void WSIPacket::setDataframe(boost::shared_ptr<websocket::dataframe> df)
{
	dataf = boost::make_shared<websocket::dataframe>();
}

WSIPacket::~WSIPacket()
{
}