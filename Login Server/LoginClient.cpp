#include "LoginClient.h"
#include "LoginServer.h"
#include <iostream>

void LoginClient::setBaseInfo(const WORD userId, const DWORD accessLevel, String& userNam, String& pw) {
	this->accountInfo.userId = userId;
	this->accountInfo.accessLevel = accessLevel;
	this->accountInfo.userName = userNam;
	this->accountInfo.password = pw;
}

bool LoginClient::pakEncryptionRequest() {
	Packet pak(PacketID::Login::Response::ENCRYPTION);
	pak.addByte(0x02);
	pak.addDWord(this->getCryptInfo().status.additionalValuePerStep);

	return this->sendPacket(pak);
}

bool LoginClient::pakRequestChannelList() {
	DWORD requestedServer = this->getPacket().getDWord(0x00);
	
	Packet pak(PacketID::Login::Response::GET_SERVERLIST);
	pak.addDWord(requestedServer);
	//if (mainServer->isCharServerConnected()) {
		pak.addByte(0x01); //Amount

		pak.addWord(0x01); //ID
		pak.addByte(0x00); //?
		pak.addWord(0x00); //STATUS
		pak.addString("TestChannel"); //NAME
		pak.addByte(0x00);
	//}
	//else {
	//	pak.addByte(0x00);
	//}
	/*
	Iterate through all the channel
	LoginServer* server = dynamic_cast<LoginServer*>(this->serverDelegate);

	BYTE serverStatus = 0x00;
	for (unsigned int i = 0; i < server->getChannelAmount(); i++) {

	Channel* channel = server->getChannel(i);
	pak.addWord( channel->getID() );
	pak.addByte( 0x00 );

	serverStatus = (channel->getPlayerAmount() * 100) / (channel->getMaxConnections() == 0 ? 1 : channel->getMaxConnections());
	pak.addWord( serverStatus );

	pak.addWord( channel->getName() );
	pak.addByte( 0x00 );
	}
	*/
		return this->sendPacket(pak);
}
bool LoginClient::pakUserLogin() {
	String content = String(this->getPacket().getData());
	this->accountInfo.password = content.substring(0, 0x20);
	this->accountInfo.userName = content.substring(0x20, content.length() - 0x20);

	Statement request(LoginServer::getDB(), DBQueries::Select::ACCOUNT_INFOS);
	request.setString(this->accountInfo.userName);
	auto result = request.executeWithResult();
	if (!result.get()->hasResult()) {
		Packet pak(PacketID::Login::Response::USER_LOGIN);
		pak.addByte(0x02);
		pak.addDWord(0x00);
		return this->sendPacket(pak);
	}
	auto currentRow = result.get()->getFirst();
	this->accountInfo.userId = static_cast<DWORD>(currentRow[0].toInt());
	this->accountInfo.accessLevel = static_cast<DWORD>(currentRow[1].toInt());

	Packet pak(PacketID::Login::Response::USER_LOGIN);
	//ADDDWORD( pak, 0x6f000000 );

	//OKAY
	pak.addDWord(0x0c000000);
	pak.addByte(0x00);
	
	//0x30 + ID
	pak.addByte(0x31);
	pak.addString("TestServer");
	//pak.addString(config->getValueString("ServerName"));
	pak.addByte(0x00);

	//ChannelId
	pak.addDWord(0x01);

	return this->sendPacket(pak);
}

bool LoginClient::pakRequestChannelIP() {

	DWORD serverId = this->getPacket().getDWord(0x00);
	BYTE channel = this->getPacket().getByte(0x04);

	Packet pak(PacketID::Login::Response::GET_CHANNEL_IP);
	pak.addByte(0x00); //Channel status
	pak.addDWord(this->accountInfo.userId);
	pak.addDWord(0x87654321); //Encryption
	//pak.addString(config->getValueString("ChannelIp"));
	pak.addString("127.0.0.1");
	pak.addByte(0x00);
	pak.addWord(29100); //PORT

	return this->sendPacket(pak);
}

bool LoginClient::pakIdentifyCharServer() {
	return false;
}



bool LoginClient::handlePacket() {
	std::cout << "New Packet: " << std::hex << this->getPacket().getCommand() << " with Length " << std::dec << this->getPacket().getLength() << "\n";
	switch (this->getPacket().getCommand()) {
		case PacketID::Login::Request::CHARSERVER_IDENTIFY:
			return this->pakIdentifyCharServer();

		case PacketID::Login::Request::ENCRYPTION:
			return this->pakEncryptionRequest();

		case PacketID::Login::Request::GET_SERVER_LIST:
			return this->pakRequestChannelList();

		case PacketID::Login::Request::USER_LOGIN:
			return this->pakUserLogin();

		case PacketID::Login::Request::GET_CHANNEL_IP:
			return this->pakRequestChannelIP();

		case PacketID::Login::Request::GAME_GUARD:
			return true;
	}
	return true;
}