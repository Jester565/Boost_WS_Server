#pragma once
#include <HeaderManager.h>
#include <dataframe_manager.h>
#include <handshake_manager.h>

class WSHeaderManager : public HeaderManager
{
public:
	static const int HEADER_OUT_SIZE = 2;
	static const int HEADER_IN_SIZE = 2;
	WSHeaderManager(Server* server);

	boost::shared_ptr<std::vector<unsigned char>> encryptHeader(boost::shared_ptr<OPacket> pack) override;

	boost::shared_ptr<IPacket> decryptHeader(boost::shared_ptr<std::vector<unsigned char>> data, unsigned int size, IDType cID) override;

	~WSHeaderManager();

protected:
	boost::shared_ptr<std::vector<unsigned char>> encryptHeaderAsBigEndian(boost::shared_ptr<OPacket> pack) override;

	boost::shared_ptr<std::vector<unsigned char>> encryptHeaderToBigEndian(boost::shared_ptr<OPacket> pack) override;

	boost::shared_ptr<IPacket> decryptHeaderAsBigEndian(boost::shared_ptr<std::vector<unsigned char>> data, unsigned int size, IDType cID) override;

	boost::shared_ptr<IPacket> decryptHeaderFromBigEndian(boost::shared_ptr<std::vector<unsigned char>> data, unsigned int size, IDType cID) override;

	websocket::dataframe_manager* dfm;

	websocket::handshake_manager* hsm;

	bool handshakeComplete;
};

