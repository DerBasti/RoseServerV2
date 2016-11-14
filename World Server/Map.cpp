#include "Map.h"
#include "Entities\Entity.h"

Map::Map(const word_t id) {
	this->id = id;
}

Map::~Map() {

}

bool Map::setEntity(Entity* entity) {
	if (this->entities.size() == Map::MAX_LOCAL_CLIENTS) {
		return false;
	}
	word_t newId = 0x00;
	auto iterator = this->entities.end();
	do {
		newId = rand();
		iterator = this->entities.find(newId);
	} while (iterator == this->entities.end() && newId > 0);
	entity->getBasicInformation()->setLocalId(newId);
	return true;
}

Entity* Map::getEntity(const word_t localId) {
	Entity* entity = nullptr;
	if (localId != 0 && this->entities.count(localId) > 0) {
		entity = this->entities[localId];
	}
	return entity;
}

bool Map::hasEntity(Entity* entity) const {
	return this->hasEntity(entity != nullptr ? entity->getBasicInformation()->getLocalId() : 0x00);
}

bool Map::hasEntity(const word_t localId) const {
	return this->entities.count(localId) > 0;
}

void Map::clearEntity(Entity* entity) {
	this->entities[entity->getBasicInformation()->getLocalId()] = nullptr;
}