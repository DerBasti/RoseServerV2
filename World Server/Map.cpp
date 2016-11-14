#include "Map.h"
#include "Entities\Entity.h"

Map::Map(const word_t id) {
	this->id = id;
}

Map::~Map() {
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

bool Map::updateEntity(Entity* entity) {
	return this->updateEntity(entity != nullptr ? entity->getBasicInformation()->getLocalId() : static_cast<word_t>(0x00));
}

bool Map::updateEntity(const word_t localId) {
	auto result = this->entitiesOnMap.find(localId);
	if (result != this->entitiesOnMap.end()) {
		auto pair = result->second;
		//TODO
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