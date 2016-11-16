#include "NPC.h"
#include "..\WorldServer.h"

NPC::NPC(const word_t npcId, const float direction) {
	this->typeId = npcId;
	auto server = ROSEServer::getServer<WorldServer>();
	NPCSTB *stb = server->getNPCSTB();

	Stats* stats = this->getStats();
	stats->setAttackPower(stb->getAttackpower(this->getTypeId()));
	stats->setAttackRange(stb->getAttackrange(this->getTypeId()));
	stats->setMaxHP(stb->getHPperLevel(this->getTypeId()) * stb->getLevel(this->getTypeId()));
	stats->setHP(stats->getHP());
}