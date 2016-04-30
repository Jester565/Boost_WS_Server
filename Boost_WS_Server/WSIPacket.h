#pragma once
#include <dataframe.h>
#include <IPacket.h>

class WSIPacket : public IPacket
{
public:
	WSIPacket();
	
	const boost::shared_ptr<std::string> getData() const
	{
		return data;
	}

	boost::shared_ptr<websocket::dataframe> getDataframe()
	{
		return dataframe;
	}

	~WSIPacket();

private:
	boost::shared_ptr<std::string> data;
	boost::shared_ptr<websocket::dataframe> dataframe;
};

