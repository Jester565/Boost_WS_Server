/*
Extracts the payload from the Websocket message, converting it into an IPacket
Converts OPacket into a websocket message
*/

#pragma once
#include <HeaderManager.h>
#include <dataframe_manager.h>
#include <handshake_manager.h>

//Forward declaration
class WSIPacket;

class WSHeaderManager : public HeaderManager
{
public:
	/*
	The number of bytes in front of the Google Protocol Buffer Header
	indicating the size of the Google Protocol Buffer header
	*/
	static const int HEADER_OUT_SIZE = 2;

	/*
	The number of bytes in front of the Google Protocol Buffer Header
	indicating the size of the Google Protocol Buffer header
	*/
	static const int HEADER_IN_SIZE = 2;

	//Initializes the classes used to handle the websocket headers
	WSHeaderManager(Server* server);

	//Override decryptHeader because bigEndian and littleEndian can shar some code, that code being the parsing of the websocket header
	boost::shared_ptr<IPacket> decryptHeader(unsigned char* data, unsigned int size, IDType cID) override;

	//Destroy header parsing classes
	~WSHeaderManager();

protected:
	boost::shared_ptr<std::vector<unsigned char>> encryptHeaderAsBigEndian(boost::shared_ptr<OPacket> pack) override;

	boost::shared_ptr<std::vector<unsigned char>> encryptHeaderToBigEndian(boost::shared_ptr<OPacket> pack) override;

	boost::shared_ptr<WSIPacket> decryptHeaderAsBigEndian(char* data, unsigned int size, IDType cID);

	boost::shared_ptr<WSIPacket> decryptHeaderFromBigEndian(char* data, unsigned int size, IDType cID);

	/*
	The websocket protocol contains an extra step before data can be sent: the handshake
	The handshake_manager, will parse the handshake and send back the appropiate response (this code is in decryptHeader)
	The dataframe manager, handles the websocket header found in normal data messages
	*/

	websocket::dataframe_manager* dfm;

	websocket::handshake_manager* hsm;

	bool handshakeComplete;
};
