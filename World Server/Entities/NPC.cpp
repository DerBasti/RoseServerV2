#include "NPC.h"
#include "..\WorldServer.h"

NPC::NPC(const word_t npcId, const byte_t mapId, const Position& pos, const float direction) {

	this->getVisuality()->setOnNewSector(nullptr);
	this->getVisuality()->setOnRemoveSector(nullptr);

	this->typeId = npcId;
	auto server = ROSEServer::getServer<WorldServer>();
	NPCSTB *stb = server->getNPCSTB();

	this->name = stb->getName(this->getTypeId());

	this->getPositionInformation()->setCurrent(pos);
	this->getPositionInformation()->setDestination(pos);
	Map *newMap = ROSEServer::getServer<WorldServer>()->getMap(mapId);
	if (newMap) {
		newMap->addEntity(this);
		newMap->updateEntity(this);
	}
	this->dir = direction;

	Stats* stats = this->getStats();
	stats->setAttackPower(stb->getAttackpower(this->getTypeId()));
	stats->setAttackRange(stb->getAttackrange(this->getTypeId()));
	stats->setMaxHP(stb->getHPperLevel(this->getTypeId()) * stb->getLevel(this->getTypeId()));
	stats->setHP(stats->getHP());
}