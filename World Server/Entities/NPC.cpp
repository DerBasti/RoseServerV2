#include "NPC.h"
#include "..\WorldServer.h"

NPC::NPC(const word_t npcId, const byte_t mapId, const Position& pos, const float direction) {
	this->getStats()->setStance(new NPCStance());

	auto newStance = this->getStats()->getStance();
	newStance->setOnStanceChanged([this](byte_t) {
		Stance* stance = this->getStats()->getStance();
		if (stance->isWalking()) { 
			this->getStats()->setMovementSpeed(this->getNPCData()->get(NPCSTB::Columns::WALKSPEED_COLUMN).toUShort());
		}
		else {
			this->updateMovementSpeed();
		}
		this->sendCurrentStance();
	});
	this->getStats()->setStance(new NPCStance());

	this->getBasicInformation()->setIngameFlag(true);
	this->getBasicInformation()->setTeamId(0x01);

	this->getVisuality()->setOnNewSector(nullptr);
	this->getVisuality()->setOnRemoveSector(nullptr);

	this->getCombatInformation()->getAttackTimer().setWrappingTime(2000);

	this->typeId = npcId;
	auto server = ROSEServer::getServer<WorldServer>();
	this->npcData = server->getNPCSTB()->getEntry(this->getTypeId());

	this->name = this->getNPCData()->get(NPCSTB::Columns::NAME_COLUMN);
	this->getStats()->setLevel(this->getNPCData()->get(NPCSTB::Columns::LEVEL_COLUMN).toUShort());

	this->getPositionInformation()->setCurrent(pos);
	this->getPositionInformation()->setDestination(pos);
	Map *newMap = ROSEServer::getServer<WorldServer>()->getMap(mapId);
	if (newMap) {
		newMap->addEntity(this);
		newMap->updateEntity(this);
	}
	this->dir = direction;

	word_t aiFileId = this->getNPCData()->get(NPCSTB::Columns::AI_COLUMN).toUShort();
	this->ai = new AIHelper(server->getAIData(aiFileId));

	Stats* stats = this->getStats();
	this->updateStats();
	stats->setHP(stats->getMaxHP());
}

NPC::~NPC() {
	delete this->ai;
	this->ai = nullptr;

	this->owner = nullptr;
}

void NPC::doAction() {
	Entity::doAction(); //call parent version first
	if (this->getPositionInformation()->isIdling()) {
		if (this->getAI()->isAIReady() && this->getCombatInformation()->getTarget() == nullptr) {
			AI::doRoutine(this, AIP::StateTypes::IDLING, this->getAI()->getData());
		}
	}
	else {
		this->getAI()->updateTimer();
	}
}

void NPC::setAttackMotion() {
	auto server = ROSEServer::getServer<WorldServer>();
	this->currentAnimation = server->getNPCAnimation(this->getTypeId(), CHR::MotionType::ATTACK);
	Entity::setAttackMotion();
}

void NPC::onSpawn() {
	AI::doRoutine(this, AIP::StateTypes::SPAWNED, this->getAI()->getData());
}

bool NPC::sendNewDestinationVisually() {
	Packet pak(PacketID::World::Response::MOVEMENT_MONSTER);
	pak.addWord(this->getBasicInformation()->getLocalId());
	pak.addWord(this->getCombatInformation()->getTargetId());
	pak.addWord(static_cast<word_t>(this->getPositionInformation()->getDestination().distanceTo(this->getPositionInformation()->getCurrent())));
	pak.addPosition(this->getPositionInformation()->getDestination());
	pak.addWord(0x00); //Z-Axis
	pak.addByte(this->getStats()->getStance()->getId());
	return this->sendToVisible(pak);
}

void NPC::updateAttackPower() {
	word_t attackPower = this->getNPCData()->get(NPCSTB::Columns::ATTACKPOWER_COLUMN).toUShort();
	this->getStats()->setAttackPower(attackPower);
}

void NPC::updateMaxHP() {
	dword_t level = this->getNPCData()->get(NPCSTB::Columns::LEVEL_COLUMN).toUShort();
	dword_t hpPerLevel = this->getNPCData()->get(NPCSTB::Columns::HP_COLUMN).toUShort();
	this->getStats()->setMaxHP(level * hpPerLevel);
}

void NPC::updateMaxMP() {

}

void NPC::updatePhysicalDefense() {
	word_t defense = this->getNPCData()->get(NPCSTB::Columns::DEFENSE_COLUMN).toUShort();
	this->getStats()->setPhysicalDefense(defense);
}

void NPC::updateMagicalDefense() {
	word_t defense = this->getNPCData()->get(NPCSTB::Columns::MAGICDEFENSE_COLUMN).toUShort();
	this->getStats()->setPhysicalDefense(defense);
}

void NPC::updateMovementSpeed() {
	word_t movementSpeed = 0x00;
	if (this->getStats()->getStance()->isRunning()) {
		movementSpeed = this->getNPCData()->get(NPCSTB::Columns::SPRINTSPEED_COLUMN).toUShort();
	}
	else {
		movementSpeed = this->getNPCData()->get(NPCSTB::Columns::WALKSPEED_COLUMN).toUShort();
	}
	this->getStats()->setMovementSpeed(movementSpeed);
}

void NPC::updateAttackSpeed() {
	word_t atkSpeed = this->getNPCData()->get(NPCSTB::Columns::ATTACKSPEED_COLUMN).toUShort();
	this->getStats()->setAttackSpeed(atkSpeed);
}

void NPC::updateAttackRange() {
	word_t range = this->getNPCData()->get(NPCSTB::Columns::ATTACKRANGE_COLUMN).toUShort();
	this->getStats()->setAttackRange(range);
}