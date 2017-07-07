#pragma once
#include <dataframe.h>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <IPacket.h>
#include <vector>
#include <iostream>

class WSIPacket : public IPacket
{
	friend class WSHeaderManager;
public:
	WSIPacket();

	boost::shared_ptr<websocket::dataframe> getDataframe()
	{
		return dataf;
	}

	virtual ~WSIPacket();

protected:
	void setDataframe(boost::shared_ptr<websocket::dataframe> df);

	boost::shared_ptr<websocket::dataframe> dataf;
};
