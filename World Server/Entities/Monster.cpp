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

Monster::Monster(const word_t typeId, const byte_t mapId, const Position& pos, MonsterSpawn* spawnRef) : NPC(typeId, mapId, pos, 0.0f) {
	this->spawnReference = spawnRef;
	this->onSpawn();
}

Monster::~Monster() {
	if (this->getSpawn()) {
		this->getSpawn()->removeMonster(this);
	}
	this->spawnReference = nullptr;
	this->getPositionInformation()->getMap()->removeEntity(this);
}

void Monster::onDeath() {
	//?
}