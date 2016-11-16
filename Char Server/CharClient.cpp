#include "CharClient.h"
#include "CharServer.h"
#include "..\Common\PacketIDs.h"

CharClient::~CharClient() {
	this->accountInfo.isLoggedIn = false;
	this->accountInfo.password = "";
	this->accountInfo.userName = "";

	this->selectedCharacter.id = 0x00;
	this->selectedCharacter.name = "";

	delete[] this->characters;
	this->characters = nullptr;
}

bool CharClient::pakIdentifyAccount() {
	this->accountInfo.id = this->getPacket().getDWord(0x00);

	Statement statement(ROSEServer::getDatabase(), DBQueries::Select::ACCOUNT_NAME_BY_ID);
	statement.setUInt(this->accountInfo.id);
	auto result = statement.executeWithResult();
	if (!result->hasResult()) {
		return false;
	}
	auto currentRow = result->getFirst();
	this->accountInfo.userName = String(currentRow[0]);
	this->accountInfo.isLoggedIn = true;

	Packet pak(PacketID::Character::Response::IDENTIFY);
	pak.addByte(0x00);
	pak.addDWord(0x87654321); //Encryption
	pak.addDWord(0x00);
	return this->sendPacket(pak);
}


void CharClient::getCharactersFromResult(ResultSet* rs) {
	auto rows = rs->getResultRows();
	this->charAmount = static_cast<byte_t>(rs->getResultAmount());

	byte_t currentChar = 0x00;
	std::for_each(rows.begin(), rows.end(), [&](ResultRow& row) {
		Character newChar;
		newChar.id = row[0].toInt();
		newChar.name = row[2];
		newChar.level = row[3].toInt();
		newChar.classId = row[5].toInt();
		newChar.faceStyle = row[6].toInt();
		newChar.hairStyle = row[7].toInt();
		newChar.sex = row[8].toInt();
		newChar.deleteTime = 0;
		this->characters[currentChar] = newChar;
		currentChar++;
	});
}

bool CharClient::pakGetCharacters() {
	BYTE itemAmount = 10;
	Statement getCharactersStatement(ROSEServer::getDatabase(), DBQueries::Select::ALL_CHARACTERS_FROM_ACCOUNT);
	getCharactersStatement.setUInt(this->accountInfo.id);
	auto result = getCharactersStatement.executeWithResult();
	if (!result->hasResult()) {
		return false;
	}

	Packet pak(PacketID::Character::Response::GET_CHARACTERS);
	pak.addByte(static_cast<BYTE>(result->getResultAmount()));
	this->getCharactersFromResult(result.get());

	for (unsigned int i = 0; i < this->getCharacterAmount(); i++) {
		Character& newChar = this->characters[i];
		Statement getEquipmentStatement(ROSEServer::getDatabase(), DBQueries::Select::CHARSELECT_EQUIPMENT);
		getEquipmentStatement.setUInt(newChar.id);
		auto equipRes = getEquipmentStatement.executeWithResult();
		if (!equipRes->hasResult()) {
			return false;
		}
		auto resultSet = equipRes->getResultRows();
		DWORD itemCnt = resultSet.getSize();
		std::for_each(resultSet.begin(), resultSet.end(), [&](ResultRow& row) {
			Item& curItem = newChar.equipment[row.get(0).toInt()];
			curItem = Item(row.get(1).toInt() / 10000, row.get(1).toInt() % 10000, 1);
			curItem.setRefineLevel(row.get(2).toInt());
		});
		pak.addString(newChar.name.toConstChar());
		pak.addByte(newChar.sex);
		pak.addWord(newChar.level);
		pak.addWord(newChar.classId);
		pak.addDWord(newChar.deleteTime);
		pak.addByte(0x00); //Platinum ?
		pak.addDWord(newChar.faceStyle);
		pak.addDWord(newChar.hairStyle);
#define ITEM_SENDING(slot) pak.addWord(newChar.equipment[slot].getId()); pak.addWord(newChar.equipment[slot].getRefineLevel());

		ITEM_SENDING(2);
		ITEM_SENDING(3);
		ITEM_SENDING(5);
		ITEM_SENDING(6);
		ITEM_SENDING(1);
		ITEM_SENDING(4);
		ITEM_SENDING(7);
		ITEM_SENDING(8);
	}
	return this->sendPacket(pak);
}

bool CharClient::pakCreateCharacter() {
	Packet pak(PacketID::Character::Response::CREATE_CHARACTER);
	if (this->getCharacterAmount() == 5) { //MAX CHARS REACHED
		pak.addWord(0x04);
		return this->sendPacket(pak);
	}
	Character newChar;

	newChar.classId = 0x00;
	newChar.deleteTime = 0x00;
	newChar.faceStyle = this->getPacket().getByte(0x03);
	newChar.id = this->charAmount+1;
	newChar.hairStyle = this->getPacket().getByte(0x02);
	newChar.level = 1;
	newChar.name = this->getPacket().getString(0x07);
	newChar.sex = this->getPacket().getByte(0x00) % 2;

	CharServer* server = ROSEServer::getServer<CharServer>();
	if (!server->addCharacter(this->accountInfo.id, newChar)) {
		return false;
	}
	this->characters[this->getCharacterAmount()] = newChar;
	this->charAmount++;
	pak.addWord(0x00);
	return this->sendPacket(pak);
}

bool CharClient::pakDeleteCharacter() {
	unsigned char caseType = this->getPacket().getByte(0x00);
	this->logger.info(String("Case type: ") + String::fromInt(caseType));
	Packet pak(PacketID::Character::Response::DELETE_CHARACTER);
	pak.addByte(caseType);
	this->sendPacket(pak);
	return true;
}

bool CharClient::pakGetWorldserverIp() {
	const char *data = &this->getPacket().getData()[0x03];
	this->selectedCharacter.name = std::string(data);

	DWORD charId = 0x00;
	for (unsigned int i = 0; i < this->getCharacterAmount(); i++) {
		if (this->characters[i].name.contentEquals(this->selectedCharacter.name)) {
			charId = this->characters[i].id;
		}
	}

	auto server = ROSEServer::getServer<CharServer>();
	server->updateLastPlayedChar(this->accountInfo.id, charId);

	Packet pak(PacketID::Character::Response::GET_WORLDSERVER_IP);
	pak.addWord(server->getConfig().get("WorldPort").toInt()); //PORT
	pak.addDWord(this->accountInfo.id);
	pak.addDWord(CharServer::DEFAULT_ENCRYPTION_KEY);
	pak.addString(server->getConfig().get("WorldIp")); //IP
	if (!this->sendPacket(pak))
		return false;

	pak.newPacket(PacketID::Character::Response::MESSAGE_MANAGER);
	pak.addByte(0x06); //Send Friendlist
	pak.addByte(0x00); //NUMBER OF FRIENDS
	return this->sendPacket(pak);
}

bool CharClient::pakUnknown() {
	byte_t type = this->getPacket().getByte(0x00);
	Packet pak(PacketID::Character::Response::UNKNOWN);
	switch (type) {
		case 0x03:
			pak.addByte(0x01);
			pak.addWord(0x00);
			if (!this->sendPacket(pak))
				return false;
		break;
		default:
			this->logger.info(String("[PacketId 0x7e5 (Length: ") + String::fromInt(this->getPacket().getLength()) + String(") Unknown switch - case type: ") + String::fromInt(type));
	}
	return true;
}

bool CharClient::handlePacket() {
	std::cout << "New Packet: " << std::hex << this->getPacket().getCommand() << " with Length " << std::dec << this->getPacket().getLength() << "\n";
	switch (this->getPacket().getCommand()) {
		case PacketID::Character::Request::IDENTIFY:
			return this->pakIdentifyAccount();

		case PacketID::Character::Request::GET_CHARACTERS:
			return this->pakGetCharacters();

		case PacketID::Character::Request::CREATE_CHARACTER:
			return this->pakCreateCharacter();

		case PacketID::Character::Request::DELETE_CHARACTER:
			return this->pakDeleteCharacter();

		case PacketID::Character::Request::GET_WORLDSERVER_IP:
			return this->pakGetWorldserverIp();

		case PacketID::Character::Request::CLAN_MANAGER:
			return true;

		case PacketID::Character::Request::MESSAGE_MANAGER:
			return true;

		case PacketID::Character::Request::UNKNOWN:
			return this->pakUnknown();
	}
	return false;
}