#include "Player.h"
#include "..\..\Common\PacketIDs.h"
#include "..\WorldServer.h"

FunctionBinder<Player, unsigned long, bool(Player::*)()> Player::PACKET_FUNCTIONS = {
	{ PacketID::World::Request::GET_ID, &Player::pakAssignId },
	{ PacketID::World::Request::IDENFITY, &Player::pakIdentify },
	{ PacketID::World::Request::TELEGATE, &Player::pakTeleport }
};

Player::Player(NetworkInterface* iFace, const CryptInfo& cryptInfo) : ROSESocketClient(iFace, cryptInfo) {
};

Player::~Player() {

}

bool Player::loadEntireCharacter() {
	return this->loadCharacterInformation() && this->loadCharacterStats() &&
		this->loadCharacterInventory() && this->loadCharacterSkills();
}

bool Player::loadCharacterInformation() {
	auto server = ROSEServer::getServer<WorldServer>();
	auto db = server->getDB();
	Statement lastChar(db, DBQueries::Select::LAST_CHAR_AND_ACCESSLEVEL);
	lastChar.setUInt(this->getAccountInfo()->getId());
	auto lastCharResult = lastChar.executeWithResult();
	if (!lastCharResult->hasResult()) {
		return false;
	}
	auto lastCharResultRow = lastCharResult->getFirst();
	this->getCharacter()->setId(lastCharResultRow.get(0x00).toInt());
	this->getAccountInfo()->setAccessLevel(lastCharResultRow.get(0x01).toInt());

	Statement charInfos(db, DBQueries::Select::CHARACTER_BASIC_INFORMATION);
	charInfos.setUInt(this->getCharacter()->getId());
	auto charInfoResult = charInfos.executeWithResult();
	if (!charInfoResult->hasResult()) {
		return false;
	}
	auto charInfoRow = charInfoResult->getFirst();
	this->getCharacter()->setName(charInfoRow.get(0x00));
	this->getCharacter()->setLevel(charInfoRow.get(0x01).toShort());
	this->getCharacter()->setExperience(charInfoRow.get(0x02).toInt());
	this->getCharacter()->setJobId(charInfoRow.get(0x03).toShort());
	this->getCharacter()->getAppearance()->setFaceStyle(charInfoRow.get(0x04).toShort());
	this->getCharacter()->getAppearance()->setHairStyle(charInfoRow.get(0x05).toShort());
	this->getCharacter()->getAppearance()->setSex(charInfoRow.get(0x06).toByte());

	Item& moneySlot = this->getCharacter()->getInventory()->get(0x00);
	moneySlot = Item(ItemType::MONEY, 0x00);
	moneySlot.setAmount(charInfoRow.get(0x07).toInt());
	this->getCharacter()->setSaveTown(charInfoRow.get(0x08).toShort());
	const byte_t mapId = charInfoRow.get(0x09).toByte();
	this->getPositionInformation()->setMap(server->getMap(mapId)); //TODO
	this->getPositionInformation()->setCurrent(Position(520000, 520000));
	this->getPositionInformation()->setDestination(this->getPositionInformation()->getCurrent());

	return true;
}

bool Player::loadCharacterStats() {
	Statement stm(ROSEServer::getDatabase(), DBQueries::Select::CHARACTER_STATS);
	stm.setUInt(this->getCharacter()->getId());
	auto result = stm.executeWithResult();
	if (!result->hasResult()) {
		return false;
	}
	auto row = result->getFirst();
	Attributes *attributes = this->getCharacter()->getAttributes();
	attributes->setStrength(row.get(0x01).toShort());
	attributes->setDexterity(row.get(0x02).toShort());
	attributes->setIntelligence(row.get(0x03).toShort());
	attributes->setConcentration(row.get(0x04).toShort());
	attributes->setCharm(row.get(0x05).toShort());
	attributes->setSensibility(row.get(0x06).toShort());
	this->getCharacter()->setStatPoints(row.get(0x07).toShort());
	this->getCharacter()->setSkillPoints(row.get(0x08).toShort());

	return true;
}

bool Player::loadCharacterInventory() {
	Statement stm(ROSEServer::getDatabase(), DBQueries::Select::CHARACTER_INVENTORY);
	stm.setUInt(this->getCharacter()->getId());
	auto result = stm.executeWithResult();
	if (!result->hasResult()) {
		return false;
	}
	for (unsigned long i = 0; i < result->getResultAmount(); i++) {
		auto row = result->getRow(i);
		byte_t slotId = row.get(0x01).toByte();
		Item& item = this->getCharacter()->getInventory()->get(slotId);
		item = Item(row.get(0x02).toInt() / 10000, row.get(0x02).toInt() % 10000);
		item.setDurability(row.get(0x03).toByte());
		item.setLifeSpan(row.get(0x04).toShort());
		item.setAmount(row.get(0x05).toInt());
		//Refine?
	}
	return true;
}

bool Player::loadCharacterSkills() {
	//TODO
	return true;
}

bool Player::sendEncryption() {
	Packet pak(PacketID::World::Response::IDENFITY);
	pak.addByte(0x00); //??
	pak.addDWord(0x87654321); //Encryption
	pak.addDWord(0x00);
	return this->sendPacket(pak);
}

bool Player::sendGamingPlan() {
	Packet pak(PacketID::World::Response::GAMING_PLAN);
	pak.addWord(0x1001); //?
	pak.addDWord(0x02); //Unlimited plan
	return this->sendPacket(pak);
}

bool Player::sendPlayerInformation() {
	Packet pak(PacketID::World::Response::PLAYER_INFOS);
	pak.addByte(this->getCharacter()->getAppearance()->getSex());
	pak.addWord(this->getPositionInformation()->getMap()->getId());
	pak.addFloat(this->getPositionInformation()->getCurrent().getX());
	pak.addFloat(this->getPositionInformation()->getCurrent().getY());
	pak.addWord(this->getCharacter()->getSaveTown()); //SAVED POSITION
	pak.addDWord(this->getCharacter()->getAppearance()->getFaceStyle());
	pak.addDWord(this->getCharacter()->getAppearance()->getHairStyle());

	pak.addDWord(this->getCharacter()->getInventory()->get(Inventory::Slots::HEADGEAR).getVisualityData());
	pak.addDWord(this->getCharacter()->getInventory()->get(Inventory::Slots::ARMOR).getVisualityData());
	pak.addDWord(this->getCharacter()->getInventory()->get(Inventory::Slots::GLOVES).getVisualityData());
	pak.addDWord(this->getCharacter()->getInventory()->get(Inventory::Slots::SHOES).getVisualityData());
	pak.addDWord(this->getCharacter()->getInventory()->get(Inventory::Slots::FACE).getVisualityData());
	pak.addDWord(this->getCharacter()->getInventory()->get(Inventory::Slots::BACK).getVisualityData());
	pak.addDWord(this->getCharacter()->getInventory()->get(Inventory::Slots::WEAPON).getVisualityData());
	pak.addDWord(this->getCharacter()->getInventory()->get(Inventory::Slots::SHIELD).getVisualityData());

	pak.addByte(0x00); //BIRTHSTONE
	pak.addWord(0x00); //BIRTHPLACE?
	pak.addWord(this->getCharacter()->getJobId()); //JOB
	pak.addByte(0x00); //UNION
	pak.addByte(0x00); //RANK
	pak.addByte(0x00); //FAME

	pak.addWord(this->getCharacter()->getAttributes()->getSkilledStrength());
	pak.addWord(this->getCharacter()->getAttributes()->getSkilledDexterity());
	pak.addWord(this->getCharacter()->getAttributes()->getSkilledIntelligence());
	pak.addWord(this->getCharacter()->getAttributes()->getSkilledConcentration());
	pak.addWord(this->getCharacter()->getAttributes()->getSkilledCharm());
	pak.addWord(this->getCharacter()->getAttributes()->getSkilledSensibility());
	pak.addWord(this->getStats()->getHP());
	pak.addWord(this->getStats()->getMP());

	pak.addDWord(this->getCharacter()->getExperience());
	pak.addWord(this->getCharacter()->getLevel());
	pak.addWord(this->getCharacter()->getStatPoints());
	pak.addWord(this->getCharacter()->getSkillPoints());
	pak.addByte(0x64); //BodySize
	pak.addByte(0x64); //HeadSize
	pak.addDWord(0x00);
	pak.addDWord(0x00);

	for (dword_t i = 0; i < 10; i++) {
		pak.addWord(0x00); //Union points
	}
	
	pak.addDWord(0x00);
	pak.addDWord(0x00);
	pak.addByte(0x00);
	pak.addWord(this->getStats()->getStamina());
	for (short i = 0; i < 0x146; i++) {
		pak.addByte(0x00); //?
	}
	for (unsigned int i = 0; i<120; i++) {
		pak.addWord(0x00); //SkillIDs
	}
	for (unsigned int i = 0; i < 32; i++) {
		pak.addWord(0x00); //QUICK BAR
	}
	pak.addDWord(this->getCharacter()->getId());
	pak.addString(this->getCharacter()->getName());

	return this->sendPacket(pak);
}

bool Player::sendInventory() {
	Inventory* inventory = this->getCharacter()->getInventory();
	Packet pak(PacketID::World::Response::PLAYER_INVENTORY);
	pak.addQWord(inventory->get(0x00).getAmount());

	pak.addWord(0x00);
	pak.addDWord(0x00);
	for (unsigned int i = 1; i < Inventory::Slots::MAXIMUM; i++) {
		pak.addWord(inventory->get(i).getPacketHeader());
		pak.addDWord(inventory->get(i).getPacketData());
	}
	return this->sendPacket(pak);
}


bool Player::sendQuestData() {
	Packet pak(PacketID::World::Response::QUEST_DATA);

	//Episode vars
	for (unsigned int i = 0; i < 5; i++) {
		pak.addWord(0x00);
	}
	//JobVars
	for (unsigned int i = 0; i < 3; i++) {
		pak.addWord(0x00);
	}
	//Planet Vars
	for (unsigned int i = 0; i < 7; i++) {
		pak.addWord(0x00);
	}

	//Fraction Vars
	for (unsigned int i = 0; i < 10; i++) {
		pak.addWord(0x00);
	}

	//Quest Journey
	for (unsigned int i = 0; i < 10; i++) {
		pak.addWord(0x00); //QuestId
		pak.addDWord(0x00); //passed time

		for (unsigned int j = 0; j < 10; j++) {
			pak.addWord(0x00); //Quest Vars
		}

		pak.addDWord(0x00); //Global Switches

		for (unsigned int j = 0; j < 6; j++) {
			pak.addWord(0x00); //Item header
			pak.addDWord(0x00); //item data
		}
	}
	for (unsigned int i = 0; i < 16; i++) {
		pak.addWord(0x00); //Quest Flags
	}
	return this->sendPacket(pak);
}

bool Player::pakAssignId() {
	Packet pak(PacketID::World::Response::ASSIGN_ID);
	pak.addWord(this->getBasicInformation()->getLocalId());
	pak.addWord(this->getStats()->getHP());
	pak.addWord(this->getStats()->getMP());
	pak.addDWord(this->getCharacter()->getExperience());
	pak.addDWord(0x00); //UNUSED

	pak.addWord(0x64);
	pak.addDWord(0x0C1F4B79);
	pak.addWord(0x64);
	pak.addDWord(0x3232cd50);
	pak.addDWord(0x32323235);
	pak.addDWord(0x35323232);

	//PVP-MAP (0 = false, 1 = true)
	pak.addDWord(0x00);

	//MAP TIME
	pak.addDWord(0x00);

	//White icon (friendly)
	pak.addDWord(0x00);

	this->getBasicInformation()->setIngameFlag(true);

	return this->sendPacket(pak) && this->sendWeightPercentage();
}

bool Player::sendWeightPercentage() {
	Packet pak (PacketID::World::Response::WEIGHT);
	pak.addWord(this->getBasicInformation()->getLocalId());
	pak.addByte(0x00); //WEIGHT PERCENT
	return this->sendPacket(pak);
}

bool Player::pakIdentify() {
	this->getAccountInfo()->setId(this->getPacket().getDWord(0x00));
	if (!this->sendEncryption() || !this->loadEntireCharacter()) {
		return false;
	}

	return this->sendPlayerInformation() && this->sendInventory() && this->sendQuestData() && this->sendGamingPlan();
}

bool Player::pakTeleport() {
	return true;
}

bool Player::handlePacket() {
	std::cout << "New Packet: " << std::hex << this->getPacket().getCommand() << " with Length " << std::dec << this->getPacket().getLength() << "\n";
	auto function = Player::PACKET_FUNCTIONS.getProcessingFunction(this->getPacket().getCommand());
	if (function) {
		(this->*function)();
	}
	return true;
}