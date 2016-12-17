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
#include <iostream> //temp include

WSHeaderManager::WSHeaderManager(Server* server)
	:HeaderManager(server), handshakeComplete(false)
{
	dfm = new websocket::dataframe_manager();
	hsm = new websocket::handshake_manager();
}

boost::shared_ptr<IPacket> WSHeaderManager::decryptHeader(unsigned char* data, unsigned int size, IDType cID)
{
	if (handshakeComplete)
	{
		boost::shared_ptr<websocket::dataframe> df = dfm->parse_data(data, size);
		if (df != nullptr)
		{
			if (df->opcode == websocket::dataframe::binary_frame)
			{
				char* dataArr = new char[df->payload_len];
				for (int i = 0; i < df->payload_len; i++)
				{
					dataArr[i] = df->payload.data()[i];
				}
				boost::shared_ptr<WSIPacket> iPack;
				if (bEndian)
				{
					iPack = decryptHeaderAsBigEndian(dataArr, df->payload_len, cID);
				}
				else
				{
					iPack = decryptHeaderFromBigEndian(dataArr, df->payload_len, cID);
				}
				iPack->setDataframe(df);
				delete[] dataArr;
				return iPack;
			}
			else if (df->opcode == websocket::dataframe::text_frame)
			{
				std::cout << "text_frame was received: " << std::string(df->payload.at(0), df->payload_len) << std::endl;
			}
			else if (df->opcode == websocket::dataframe::connection_close)
			{
				std::cout << "connection was closed" << std::endl;
				server->getClientManager()->removeClient(cID);
			}
			else
			{
				std::cerr << "A packet with opcode: " << df->opcode << " was received" << std::endl;
			}
		}
	}
	else
	{
		websocket::reply rep = hsm->parse_handshake(data, size);
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
	oPack->dataframe->payload.resize(HEADER_OUT_SIZE + headerPackStr.size() + oPack->data->size());
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
	std::cout << "SENDER_ID: " << oPack->getSenderID() << std::endl;
	std::string headerPackStr = headerPackOut.SerializeAsString();
	char header[HEADER_OUT_SIZE];
	header[1] = headerPackStr.size() & 0xff;
	header[0] = (headerPackStr.size() >> 8) & 0xff;
	oPack->dataframe->payload.clear();
	oPack->dataframe->payload.resize(HEADER_OUT_SIZE + headerPackStr.size() + oPack->data->size());
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
	boost::shared_ptr<WSIPacket> iPack = boost::make_shared<WSIPacket>();
	unsigned int headerPackSize = ((data[1] & 0xff) << 8) | (data[0] & 0xff);
	ProtobufPackets::PackHeaderIn headerPackIn;
	headerPackIn.ParseFromArray(data + HEADER_IN_SIZE, headerPackSize);
	iPack->sentFromID = cID;
	iPack->locKey[0] = headerPackIn.lockey()[0];
	iPack->locKey[1] = headerPackIn.lockey()[1];
	iPack->locKey[2] = '\0';
	for (int i = 0; i < headerPackIn.sendtoids_size(); i++)
	{
		iPack->sendToClients.push_back(headerPackIn.sendtoids().Get(i));
		std::cout << "SEND_TO_CIENT: " << iPack->sendToClients.at(i) << std::endl;
	}
	iPack->serverRead = headerPackIn.serverread();
	unsigned int mainPackDataSize = size - headerPackSize - HEADER_IN_SIZE;
	boost::shared_ptr<std::string> mainPackDataStr = boost::make_shared<std::string>(data + HEADER_IN_SIZE + headerPackSize, size - (HEADER_IN_SIZE + headerPackSize));
	iPack->data = mainPackDataStr;
	return iPack;
}

boost::shared_ptr<WSIPacket> WSHeaderManager::decryptHeaderFromBigEndian(char* data, unsigned int size, IDType cID)
{
	boost::shared_ptr<WSIPacket> iPack = boost::make_shared<WSIPacket>();
	unsigned int headerPackSize = ((data[0] & 0xff) << 8) | (data[1] & 0xff);
	ProtobufPackets::PackHeaderIn headerPackIn;
	headerPackIn.ParseFromArray(data + HEADER_IN_SIZE, headerPackSize);
	iPack->sentFromID = cID;
	iPack->locKey[0] = headerPackIn.lockey()[0];
	iPack->locKey[1] = headerPackIn.lockey()[1];
	iPack->locKey[2] = '\0';
	for (int i = 0; i < headerPackIn.sendtoids_size(); i++)
	{
		iPack->sendToClients.push_back(headerPackIn.sendtoids().Get(i));
	}
	iPack->serverRead = headerPackIn.serverread();
	unsigned int mainPackDataSize = size - headerPackSize - HEADER_IN_SIZE;
	boost::shared_ptr<std::string> mainPackDataStr = boost::make_shared<std::string>(data + HEADER_IN_SIZE + headerPackSize, size - (HEADER_IN_SIZE + headerPackSize));
	iPack->data = mainPackDataStr;
	return iPack;
}

WSHeaderManager::~WSHeaderManager()
{
	if (dfm != nullptr)
	{
		delete dfm;
		dfm = nullptr;
	}
	if (hsm != nullptr)
	{
		delete hsm;
		hsm = nullptr;
	}
}
