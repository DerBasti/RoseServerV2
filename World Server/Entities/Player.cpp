#include "Player.h"
#include "..\WorldServer.h"
#include "Monster.h"

#include "..\..\Common\BasicTypes\CallstackGetter.h"

Player::Player(NetworkInterface* iFace, const CryptInfo& cryptInfo) {

	this->accountInfo = new Account();
	this->attributes = new Attributes();
	this->character = new Character();
	this->questJournal = new QuestJournal();

	this->getCombatInformation()->getAttackTimer().setWrappingTime(1000);

	this->getBasicInformation()->setTeamId(0x02);

	this->logger = Logger(this);
	this->getStats()->setStance(new PlayerStance());
	Stance *stance = this->getStats()->getStance();

	this->networkInterface = new ROSESocketClient(iFace, cryptInfo);

	std::function<bool(const Packet&)> onPacketReceived = [&](const Packet& pak) {
		return this->handlePacket(pak);
	};
	this->networkInterface->setOnHandlePacket(onPacketReceived);

	stance->setOnStanceChanged([&](byte_t) {
		this->updateMovementSpeed();
		this->sendCurrentStance();
	});

};

Player::~Player() {
	delete this->accountInfo;
	this->accountInfo = nullptr;

	delete this->attributes;
	this->attributes = nullptr;

	delete this->character;
	this->character = nullptr;

	delete this->questJournal;
	this->questJournal = nullptr;
}

void Player::onDisconnect() {
	Map *map = this->getPositionInformation()->getMap();
	if (map == nullptr) {
		ROSEServer::getServer<WorldServer>()->getMap(1)->addEntity(this);
	}
}

void Player::doAction() {
	Entity::doAction(); //call basic version first.
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
	this->getStats()->setLevel(charInfoRow.get(0x01).toShort());
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
		item = Item(static_cast<byte_t>(row.get(0x02).toInt() / 10000), static_cast<word_t>(row.get(0x02).toInt() % 10000));
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
	return this->getNetworkInterface()->sendPacket(pak);
}

bool Player::sendGamingPlan() {
	Packet pak(PacketID::World::Response::GAMING_PLAN);
	pak.addWord(0x1001); //?
	pak.addDWord(0x02); //Unlimited plan
	return this->getNetworkInterface()->sendPacket(pak);
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
	pak.addWord(this->getStats()->getLevel());
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

	return this->getNetworkInterface()->sendPacket(pak);
}

bool Player::sendInventory() {
	Inventory* inventory = this->getCharacter()->getInventory();
	Packet pak(PacketID::World::Response::PLAYER_INVENTORY);
	pak.addQWord(inventory->get(0x00).getAmount());

	pak.addWord(0x00);
	pak.addDWord(0x00);
	for (byte_t i = 1; i < Inventory::Slots::MAXIMUM; i++) {
		pak.addWord(inventory->get(i).getPacketHeader());
		pak.addDWord(inventory->get(i).getPacketData());
	}
	return this->getNetworkInterface()->sendPacket(pak);
}


bool Player::sendQuestData() {
	Packet pak(PacketID::World::Response::QUEST_DATA);


	//Episode vars
	for (unsigned int i = 0; i < QuestJournal::MAXIMUM_EPISODE_VARS; i++) {
		pak.addWord(this->getQuestJournal()->getEpisodeVar(i));
	}
	//JobVars
	for (unsigned int i = 0; i < QuestJournal::MAXIMUM_JOB_VARS; i++) {
		pak.addWord(this->getQuestJournal()->getJobVar(i));
	}
	//Planet Vars
	for (unsigned int i = 0; i < QuestJournal::MAXIMUM_PLANET_VARS; i++) {
		pak.addWord(this->getQuestJournal()->getPlanetVar(i));
	}

	//Fraction Vars
	for (unsigned int i = 0; i < QuestJournal::MAXIMUM_UNION_VARS; i++) {
		pak.addWord(this->getQuestJournal()->getUnionVar(i));
	}

	//Quest Journey
	for (unsigned int i = 0; i < QuestJournal::MAXIMUM_QUESTS; i++) {
		QuestJournal::Entry* entry = this->getQuestJournal()->getQuestSlot(i);
		pak.addWord(entry->getQuestId()); //QuestId
		pak.addDWord(entry->getPassedTime()); //passed time

		for (unsigned int j = 0; j < QuestJournal::Entry::MAX_QUEST_VARS; j++) {
			pak.addWord(entry->getQuestVariable(j)); //Quest Vars
		}

		pak.addDWord(entry->getLeverData()); //Global Switches

		for (unsigned int j = 0; j < QuestJournal::Entry::MAX_QUEST_ITEMS; j++) {
			auto item = entry->getItem(j);
			pak.addWord(item.getPacketHeader()); //Item header
			pak.addDWord(item.getPacketData()); //item data
		}
	}
	for (unsigned int i = 0; i < 0x10; i++) {
		pak.addDWord(this->getQuestJournal()->getGlobalFlag<dword_t>(i)); //Quest Flags
	}
	return this->getNetworkInterface()->sendPacket(pak);
}



bool Player::sendWeightPercentage() {
	Packet pak (PacketID::World::Response::WEIGHT);
	pak.addWord(this->getBasicInformation()->getLocalId());
	pak.addByte(0x00); //WEIGHT PERCENT
	return this->getNetworkInterface()->sendPacket(pak);
}



bool Player::sendTeleport(const byte_t mapId, const Position& pos) {

	this->getCombatInformation()->clear();

	Packet pak(PacketID::World::Response::TELEGATE);
	pak.addWord(this->getBasicInformation()->getLocalId());
	pak.addWord(mapId);
	pak.addPosition(pos);
	pak.addWord(0x01); //Z axus
	if (!this->getNetworkInterface()->sendPacket(pak)) {
		return false;
	}
	this->getBasicInformation()->setIngameFlag(false);
	this->getPositionInformation()->getMap()->removeEntity(this);
	this->getPositionInformation()->setCurrent(pos);
	this->getPositionInformation()->setDestination(pos);
	this->getPositionInformation()->setMap(ROSEServer::getServer<WorldServer>()->getMap(mapId));
	return true;
}

bool Player::sendEntityVisuallyAdded(Entity* entity) {
	word_t id = 0x00;
	if (entity->isPlayer()) {
		id = PacketID::World::Response::SPAWN_PLAYER;
	}
	else if (entity->isNPC()) {
		id = PacketID::World::Response::SPAWN_NPC;
	}
	else if (entity->isMonster()) {
		id = PacketID::World::Response::SPAWN_MONSTER;
	}
	Packet pak(id);
	pak.addWord(entity->getBasicInformation()->getLocalId());
	pak.addPosition(entity->getPositionInformation()->getCurrent());
	pak.addPosition(entity->getPositionInformation()->getDestination());

	enum VisualityBits {
		IDLE, MOVING, FIGHTING,	DEAD
	};

	if (entity->getStats()->getHP() <= 0) {
		pak.addWord(DEAD);
	}
	else if (entity->getPositionInformation()->getCurrent() != entity->getPositionInformation()->getDestination()) {
		pak.addWord(MOVING);
	}
	else if (entity->getCombatInformation()->getTarget() != nullptr) {
		pak.addWord(FIGHTING);
	}
	else {
		pak.addWord(IDLE);
	}
	pak.addWord(entity->getCombatInformation()->getTargetId()); //Target
	pak.addByte(entity->getStats()->getStance()->getId()); //Stance
	pak.addDWord(entity->getStats()->getHP());
	pak.addDWord(entity->getBasicInformation()->getTeamId()); //Team = who is the enemy?
	pak.addDWord(0x00); //Buffs
	if (entity->isPlayer()) {
		return this->sendPlayerVisuallyAdded(entity, pak);
	}
	else if (entity->isNPC()) {
		return this->sendNPCVisuallyAdded(entity, pak);
	}
	return this->sendMonsterVisuallyAdded(entity, pak);
}

bool Player::sendPlayerVisuallyAdded(Entity* entity, Packet& pak) {
	if (!entity || !entity->isPlayer()) {
		return false;
	}
	Player* other = static_cast<Player*>(entity);
	pak.addByte(other->getCharacter()->getAppearance()->getSex());
	pak.addWord(other->getStats()->getMovementSpeed());
	return this->getNetworkInterface()->sendPacket(pak);
}

bool Player::sendNPCVisuallyAdded(Entity* entity, Packet& pak) {
	if (!entity || !entity->isNPC()) {
		return false;
	}
	NPC* npc = static_cast<NPC*>(entity);
	pak.addWord(npc->getTypeId());
	pak.addWord(npc->getTypeId() - 900); //Conversation Id
	pak.addFloat(npc->getDirection());
	pak.addWord(0x00);
	return this->getNetworkInterface()->sendPacket(pak);
}

bool Player::sendMonsterVisuallyAdded(Entity* entity, Packet& pak) {
	if (!entity || !entity->isMonster()) {
		return false;
	}
	Monster* mon = static_cast<Monster*>(entity);
	pak.addWord(mon->getTypeId());
	pak.addWord(0x00); //?
	return this->getNetworkInterface()->sendPacket(pak);
}

bool Player::sendEntityVisuallyRemoved(Entity* entity) {
	//only here is a check concerning the network interface necessary
	//When the client DCs he wants to clear his vision -> interface deleted -> nullpointer
	if (!entity || this->getNetworkInterface() == nullptr) {
		return false;
	}
	Packet pak(PacketID::World::Response::REMOVE_VISIBLE_ENTITY);
	pak.addWord(entity->getBasicInformation()->getLocalId());
 	return this->getNetworkInterface()->sendPacket(pak);
}

bool Player::sendNewDestinationVisually() {
	Packet pak(PacketID::World::Response::MOVEMENT_PLAYER);
	pak.addWord(this->getBasicInformation()->getLocalId());
	pak.addWord(this->getCombatInformation()->getTargetId());
	pak.addWord(static_cast<word_t>(this->getPositionInformation()->getDestination().distanceTo(this->getPositionInformation()->getCurrent())));
	pak.addPosition(this->getPositionInformation()->getDestination());
	pak.addWord(0x00); //Z-Axis
	return this->sendToVisible(pak);
}

bool Player::sendDebugMessage(const String& msg) {
	return this->sendWhisper(String("Server"), msg);
}

bool Player::sendWhisper(Entity* entity, const String& msg) {
	String name = String();
	if (entity->isPlayer()) {
		name = static_cast<Player*>(entity)->getCharacter()->getName();
	}
	else {
		name = static_cast<NPC*>(entity)->getNPCData()->get(0);
	}
	return sendWhisper(name, msg);
}

bool Player::sendWhisper(const String& name, const String& msg) {
	Packet pak(PacketID::World::Response::WHISPER_CHAT);
	pak.addString(name);
	pak.addString(msg);
	return this->getNetworkInterface()->sendPacket(pak);
}

bool Player::sendMonsterHP(Entity* mon) {
	if (!mon->isMonster()) {
		return false;
	}
	Packet pak(PacketID::World::Response::SHOW_MONSTER_HP);
	pak.addWord(mon->getBasicInformation()->getLocalId());
	pak.addDWord(mon->getStats()->getHP());
	return this->getNetworkInterface()->sendPacket(pak);
}

bool Player::handlePacket(const Packet& pak) {
	this->logger.info(String("Received Packet ") + String::fromHex(pak.getCommand()) + String(" with length: ") + String::fromInt(pak.getLength()));
	
	auto function = Player::PACKET_FUNCTIONS.getProcessingFunction(pak.getCommand());
	bool result = function != nullptr;
	if (function) {
		result &= (this->*function)(pak);
	}
	return result;
}
