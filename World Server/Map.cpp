#include "Map.h"
#include "Entities\Entity.h"
#include "Entities\Monster.h"
#include "Entities\Player.h"



void Map::Sector::addEntity(Entity* entity) {
	if (entity) {
		this->entitiesOnMap[entity->getBasicInformation()->getLocalId()] = entity;
		if (entity->isPlayer()) {
			this->playerOnMap[entity->getBasicInformation()->getLocalId()] = entity;
		}
	}
}

void Map::Sector::removeEntity(Entity* entity) {
	if (entity && this->hasEntity(entity)) {
		this->entitiesOnMap[entity->getBasicInformation()->getLocalId()] = nullptr;
	}
}

Entity* Map::Sector::getEntity(const word_t localId) {
	auto iterator = this->entitiesOnMap.find(localId);
	return (iterator == this->entitiesOnMap.end() ? nullptr : iterator->second);
}

bool Map::Sector::hasEntity(Entity* entity) {
	return entity != nullptr && this->entitiesOnMap.count(entity->getBasicInformation()->getLocalId()) > 0;
}

Map::Map(const word_t id, ZON* zoneData, std::vector<IFO>& ifoData) {
	this->id = id;
	this->zone = zoneData;

	for (IFO data : ifoData) {
		auto spawns = data.getSpawns();
		std::for_each(spawns.begin(), spawns.end(), [this](IFO::Spawn& spawn) {
			this->spawns.push_back(new MonsterSpawn(spawn));
		});

		//TODO
		auto npcs = data.getNPCs();
		std::for_each(npcs.begin(), npcs.end(), [this](IFO::NPC& npc) {
			this->addEntity(new NPC(npc.getNpcId(), npc.getDirection()));
		});
		auto telegates = data.getTelegates();
	}
}

Map::~Map() {
	if (this->zone) {
		delete this->zone;
	}
	this->zone = nullptr;

	std::for_each(this->spawns.begin(), this->spawns.end(), [](MonsterSpawn* spawn) {
		delete spawn;
		spawn = nullptr;
	});
	this->spawns.clear();
}

bool Map::isActive() const {
	return std::any_of(this->sectors.begin(), this->sectors.end(), [this](Sector* sector) {
		return sector->isActive() == true;
	});
}

bool Map::addEntity(Entity* entity) {
	return true;
}

Entity* Map::getEntity(const word_t localId) {
	Entity* entity = nullptr;
	if (localId != 0 && this->entitiesOnMap.count(localId) > 0) {
		entity = this->entitiesOnMap[localId].first;
	}
	return entity;
}

Map::Sector* Map::getNearestSector(Map::Sector* currentSector, const class Position& pos) const {
	if (currentSector) {
		//std::vector<Map::Sector*> surroundingSectors = this->getSurroundingSectors(currentSector);
		return currentSector;
	}
	return nullptr;
}



bool Map::updateEntity(Entity* entity) {
	return this->updateEntity(entity != nullptr ? entity->getBasicInformation()->getLocalId() : static_cast<word_t>(0x00));
}

bool Map::updateEntity(const word_t localId) {
	auto result = this->entitiesOnMap.find(localId);
	if (result != this->entitiesOnMap.end()) {
		Entity* entity = result->second.first;
		Map::Sector* nearestSector = this->getNearestSector(entity->getPositionInformation()->getSector(),  entity->getPositionInformation()->getCurrent());
	}
	return result != this->entitiesOnMap.end();
}

bool Map::hasEntity(Entity* entity) const {
	return this->hasEntity(entity != nullptr ? entity->getBasicInformation()->getLocalId(): static_cast<word_t>(0x00));
}

bool Map::hasEntity(const word_t localId) const {
	return this->entitiesOnMap.count(localId) > 0;
}

void Map::clearEntity(Entity* entity) {
	this->entitiesOnMap.erase(entity->getBasicInformation()->getLocalId());
}