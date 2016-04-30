#include "WSHeaderManager.h"
#include "Packets/PackFW.pb.h"
#include "WSOPacket.h"
#include "WSIPacket.h"
#include <ClientManager.h>
#include <cstdint>
#include <PacketManager.h>
#include <dataframe.h>
#include <Server.h>
#include <reply.h>
#include <cassert>
#include <string>


WSHeaderManager::WSHeaderManager(Server* server)
	:HeaderManager(server), handshakeComplete(false)
{
	dfm = new websocket::dataframe_manager();
	hsm = new websocket::handshake_manager();
}

boost::shared_ptr<std::vector<unsigned char>> WSHeaderManager::encryptHeader(boost::shared_ptr<OPacket> pack)
{
	return boost::shared_ptr<std::vector<unsigned char>>();
}

boost::shared_ptr<IPacket> WSHeaderManager::decryptHeader(boost::shared_ptr<std::vector<unsigned char>> data, unsigned int size, IDType cID)
{
	if (handshakeComplete)
	{
		boost::shared_ptr<websocket::dataframe> dataframe = dfm->parse_data(*data, size);
		if (dataframe != nullptr)
		{
			if (dataframe->opcode == websocket::dataframe::binary_frame)
			{
				char* dataArr = new char[dataframe->payload_len];
				for (int i = 0; i < dataframe->payload_len; i++)
				{
					dataArr[i] = dataframe->payload.data()[i];
				}
				boost::shared_ptr<WSIPacket> iPack;
				if (bEndian)
				{
					iPack = decryptHeaderAsBigEndian(dataArr, dataframe->payload_len, cID);
				}
				else
				{
					iPack = decryptHeaderFromBigEndian(dataArr, dataframe->payload_len, cID);
				}
				iPack->dataframe = dataframe;
				return iPack;
			}
			else if (dataframe->opcode == websocket::dataframe::text_frame)
			{
				std::cout << "text_frame was received: " << std::string(dataframe->payload.at(0), dataframe->payload_len) << std::endl;
			}
			else if (dataframe->opcode == websocket::dataframe::connection_close)
			{
				std::cout << "connection was closed" << std::endl;
				server->getClientManager()->removeClient(cID);
			}
			else
			{
				std::cerr << "A packet with opcode: " << dataframe->opcode << " was received" << std::endl;
			}
		}
	}
	else
	{
		websocket::reply rep = hsm->parse_handshake(*data, size);
		if (rep.status != websocket::reply::bad_request)
		{
			handshakeComplete = true;
		}
		boost::shared_ptr<std::vector<unsigned char>> data = boost::make_shared<std::vector<unsigned char>>(rep.to_buffer());
		server->getClientManager()->send(data, cID);
	}
	return nullptr;
}

boost::shared_ptr<std::vector<unsigned char>> WSHeaderManager::encryptHeaderAsBigEndian(boost::shared_ptr<OPacket> oPackParent)
{
	boost::shared_ptr<WSOPacket> oPack = boost::static_pointer_cast<WSOPacket>(oPackParent);
	if (oPack->data == nullptr)
		throw std::invalid_argument("The packet did not provide data");
	ProtobufPackets::PackHeaderOut headerPackOut;
	headerPackOut.set_lockey(oPack->getLocKey());
	headerPackOut.set_sentfromid(oPack->getSenderID());
	std::string headerPackStr = headerPackOut.SerializeAsString();
	char header[HEADER_OUT_SIZE];
	header[0] = headerPackStr.size() & 0xff;
	header[1] = (headerPackStr.size() >> 8) & 0xff;
	oPack->dataframe->payload.clear();
	oPack->dataframe->payload.reserve(HEADER_OUT_SIZE + headerPackStr.size() + oPack->data->size());
	for (int i = 0; i < HEADER_OUT_SIZE; i++)
	{
		oPack->dataframe->payload.at(i) = header[i];
	}
	for (int i = 0; i < headerPackStr.size(); i++)
	{
		oPack->dataframe->payload.at(i + HEADER_OUT_SIZE) = headerPackStr.at(i);
	}
	for (int i = 0; i < oPack->data->size(); i++)
	{
		oPack->dataframe->payload.at(i + HEADER_OUT_SIZE + headerPackStr.size()) = oPack->data->at(i);
	}
	return boost::make_shared<std::vector<unsigned char>>(oPack->dataframe->to_buffer());
}

boost::shared_ptr<std::vector<unsigned char>> WSHeaderManager::encryptHeaderToBigEndian(boost::shared_ptr<OPacket> oPackParent)
{
	boost::shared_ptr<WSOPacket> oPack = boost::static_pointer_cast<WSOPacket>(oPackParent);
	if (oPack->data == nullptr)
		throw std::invalid_argument("The packet did not provide data");
	ProtobufPackets::PackHeaderOut headerPackOut;
	headerPackOut.set_lockey(oPack->getLocKey());
	headerPackOut.set_sentfromid(oPack->getSenderID());
	std::string headerPackStr = headerPackOut.SerializeAsString();
	char header[HEADER_OUT_SIZE];
	header[1] = headerPackStr.size() & 0xff;
	header[0] = (headerPackStr.size() >> 8) & 0xff;
	oPack->dataframe->payload.clear();
	oPack->dataframe->payload.reserve(HEADER_OUT_SIZE + headerPackStr.size() + oPack->data->size());
	for (int i = 0; i < HEADER_OUT_SIZE; i++)
	{
		oPack->dataframe->payload.at(i) = header[i];
	}
	for (int i = 0; i < headerPackStr.size(); i++)
	{
		oPack->dataframe->payload.at(i + HEADER_OUT_SIZE) = headerPackStr.at(i);
	}
	for (int i = 0; i < oPack->data->size(); i++)
	{
		oPack->dataframe->payload.at(i + HEADER_OUT_SIZE + headerPackStr.size()) = oPack->data->at(i);
	}
	return boost::make_shared<std::vector<unsigned char>>(oPack->dataframe->to_buffer());
}

boost::shared_ptr<WSIPacket> WSHeaderManager::decryptHeaderAsBigEndian(char* data, unsigned int size, IDType cID)
{
	boost::shared_ptr<WSIPacket> iPack = boost::static_pointer_cast<WSIPacket>(server->createIPacket());
	unsigned int headerPackSize = ((data[1] & 0xff) << 8) | (data[0] & 0xff);
	char* headerPackArr = new char[headerPackSize];
	strncpy(headerPackArr, data + HEADER_IN_SIZE, HEADER_IN_SIZE + headerPackSize);
	ProtobufPackets::PackHeaderIn headerPackIn;
	headerPackIn.ParseFromArray(headerPackArr, headerPackSize);
	iPack->sentFromID = cID;
	iPack->locKey[0] = headerPackIn.lockey()[0];
	iPack->locKey[1] = headerPackIn.lockey()[1];
	iPack->sendToClients = new std::vector <IDType>();
	iPack->sendToClients->reserve(headerPackIn.sendtoids_size());
	for (int i = 0; i < headerPackIn.sendtoids_size(); i++)
	{
		iPack->sendToClients->at(i) = headerPackIn.sendtoids().Get(i);
	}
	iPack->serverRead = headerPackIn.serverread();
	unsigned int mainPackDataSize = size - headerPackSize - HEADER_IN_SIZE;
	boost::shared_ptr<std::string> mainPackDataStr = boost::make_shared<std::string>(data + HEADER_IN_SIZE + headerPackSize, size - (HEADER_IN_SIZE + headerPackSize));
	iPack->data = mainPackDataStr;
	delete[] headerPackArr;
	return iPack;
}

boost::shared_ptr<WSIPacket> WSHeaderManager::decryptHeaderFromBigEndian(char* data, unsigned int size, IDType cID)
{
	boost::shared_ptr<WSIPacket> iPack = boost::static_pointer_cast<WSIPacket>(server->createIPacket());
	unsigned int headerPackSize = ((data[0] & 0xff) << 8) | (data[1] & 0xff);
	char* headerPackArr = new char[headerPackSize];
	strncpy(headerPackArr, data + HEADER_IN_SIZE, HEADER_IN_SIZE + headerPackSize);
	ProtobufPackets::PackHeaderIn headerPackIn;
	headerPackIn.ParseFromArray(headerPackArr, headerPackSize);
	iPack->sentFromID = cID;
	iPack->locKey[1] = headerPackIn.lockey()[0];
	iPack->locKey[0] = headerPackIn.lockey()[1];
	iPack->sendToClients = new std::vector <IDType>();
	iPack->sendToClients->reserve(headerPackIn.sendtoids_size());
	for (int i = 0; i < headerPackIn.sendtoids_size(); i++)
	{
		iPack->sendToClients->at(i) = headerPackIn.sendtoids().Get(i);
	}
	iPack->serverRead = headerPackIn.serverread();
	unsigned int mainPackDataSize = size - headerPackSize - HEADER_IN_SIZE;
	boost::shared_ptr<std::string> mainPackDataStr = boost::make_shared<std::string>(data + HEADER_IN_SIZE + headerPackSize, size - (HEADER_IN_SIZE + headerPackSize));
	iPack->data = mainPackDataStr;
	delete[] headerPackArr;
	return iPack;
}

WSHeaderManager::~WSHeaderManager()
{
}
