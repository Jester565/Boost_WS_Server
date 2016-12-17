#pragma once
#include <HeaderManager.h>
#include <dataframe_manager.h>
#include <handshake_manager.h>

class WSIPacket;

class WSHeaderManager : public HeaderManager
{
public:
	static const int HEADER_OUT_SIZE = 2;
	static const int HEADER_IN_SIZE = 2;
	WSHeaderManager(Server* server);

	boost::shared_ptr<IPacket> decryptHeader(unsigned char* data, unsigned int size, IDType cID) override;

	~WSHeaderManager();

protected:
	boost::shared_ptr<std::vector<unsigned char>> encryptHeaderAsBigEndian(boost::shared_ptr<OPacket> pack) override;

	boost::shared_ptr<std::vector<unsigned char>> encryptHeaderToBigEndian(boost::shared_ptr<OPacket> pack) override;

	boost::shared_ptr<WSIPacket> decryptHeaderAsBigEndian(char* data, unsigned int size, IDType cID);

	boost::shared_ptr<WSIPacket> decryptHeaderFromBigEndian(char* data, unsigned int size, IDType cID);

	websocket::dataframe_manager* dfm;

	websocket::handshake_manager* hsm;

	bool handshakeComplete;
};

