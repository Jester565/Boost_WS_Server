#pragma once
#include <OPacket.h>
#include <dataframe.h>

class IPacket;

class WSOPacket : public OPacket
{
	friend class WSHeaderManager;
public:
	WSOPacket(IPacket* iPack, bool copyData);

	bool setData(boost::shared_ptr<std::string> data)
	{
		bool reset = !(data == nullptr);
		this->data = data;
		return reset;
	}

	boost::shared_ptr<websocket::dataframe> getDataframe()
	{
		return dataframe;
	}

	~WSOPacket();

protected:
	boost::shared_ptr<websocket::dataframe> dataframe;
	boost::shared_ptr<std::string> data;
};

