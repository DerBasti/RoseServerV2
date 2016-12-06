#include "Monster.h"
#include "..\FileTypes\IFO.h"
#include "..\WorldServer.h"
#include "..\..\Common\BasicTypes\Randomizer.h"



MonsterSpawn::MonsterSpawn(const byte_t idOfMap, const IFO::Spawn& ifoSpawn) : spawn(ifoSpawn) {
	this->nextSpawnWave = 0;
	this->mapId = idOfMap;

	auto spawnData = this->getSpawnData();
	while (this->getCurrentAmount() < spawnData.getMaxAmount()) {
		for (unsigned int i = 0; i < spawnData.getRoundAmount() && this->getCurrentAmount() < spawnData.getMaxAmount(); i++) {
			auto round = spawnData.getRound(i);
			float distance = spawnData.getAllowedMaxDistance() * 0.66f;
			Position pos(spawnData.getPosition().getX() + Randomize::GetFloat(-distance, distance),
				spawnData.getPosition().getY() + Randomize::GetFloat(-distance, distance));
			this->addMonster(round.getMobId(), pos, round.getAmount());
		}
	}
}

MonsterSpawn::~MonsterSpawn() {
	std::for_each(this->spawnedMonsters.begin(), this->spawnedMonsters.end(), [](Monster* mon) {
		delete mon;
		mon = nullptr;
	});
	this->spawnedMonsters.clear();
}

void MonsterSpawn::removeMonster(Monster* monRef) {
	if (monRef == nullptr) {
		return;
	}
	auto it = std::find_if(this->spawnedMonsters.begin(), this->spawnedMonsters.end(), [&](Monster* mon) {
		return (mon == monRef);
	});
	if (it != this->spawnedMonsters.end()) {
		this->spawnedMonsters.erase(it);
	}
}

bool MonsterSpawn::checkSpawn() {
	return true;
}

void MonsterSpawn::addMonster(const word_t typeId, const Position& pos, const word_t amount) {
	auto worldServer = ROSEServer::getServer<WorldServer>();
	for (word_t i = 0; i < amount; i++) {
		Monster *newMon = new Monster(typeId, this->getMapId(), pos, this);
		newMon->onSpawn();
		worldServer->getMap(this->getMapId())->updateEntity(newMon);
		this->spawnedMonsters.push_back(newMon);
	}
}
Monster::Monster(const word_t typeId, const byte_t mapId, const Position& pos) : Monster(typeId, mapId, pos, nullptr) {

}

Monster::Monster(const word_t typeId, const byte_t mapId, const Position& pos, MonsterSpawn* spawnRef) : Monster(typeId, mapId, pos, spawnRef, nullptr) {

}

Monster::Monster(const word_t typeId, const byte_t mapId, const Position& pos, MonsterSpawn* spawnRef, Entity* owner) : NPC(typeId, mapId, pos, 0.0f) {
	this->setOwner(owner);
	this->spawnReference = spawnRef;
	this->onSpawn();
	this->getBasicInformation()->setTeamId(0x100);
}

Monster::~Monster() {
	if (this->getSpawn()) {
		this->getSpawn()->removeMonster(this);
	}
	this->spawnReference = nullptr;
}

void Monster::onDeath() {
	//Give Exp?
	dword_t basicExp = (this->getStats()->getLevel() * this->getNPCData()->get(NPCSTB::Columns::EXPERIENCE_COLUMN).toUInt());
	double currentRatio = 0.0f;
	for (auto it = this->attackers.cbegin(); it != this->attackers.cend(); it++) {
		Entity* dmgDealer = (*it).first;
		if (dmgDealer == nullptr || !dmgDealer->isPlayer()) {
			continue;
		}
		Player* player = static_cast<Player*>(dmgDealer);
		dword_t dmg = (*it).second;
		currentRatio = dmg / static_cast<double>(this->getStats()->getMaxHP());
		player->getCharacter()->addExperience(static_cast<dword_t>(currentRatio * basicExp));
	}
}

void Monster::addDamage(Entity* dmgDealer, const dword_t damage) {
	this->attackers[dmgDealer] += damage;
	if (damage > this->getStats()->getHP()) { //double said damage
		this->attackers[dmgDealer] += (damage - this->getStats()->getHP())*2;
	}
	Entity::addDamage(dmgDealer, damage);
}