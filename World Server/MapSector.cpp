#include "Map.h"
#include "Entities\Entity.h"
#include "Entities\Monster.h"
#include "Entities\Player.h"

void Map::Sector::addEntity(Entity* entity) {
	const word_t localId = entity->getBasicInformation()->getLocalId();
	this->entitiesOnMap[localId] = entity;
	if (entity->isPlayer()) {
		this->playerOnMap[localId] = entity;
	}
	entity->getBasicInformation()->setSector(this);
}

void Map::Sector::removeEntity(Entity* entity){
	const word_t localId = entity->getBasicInformation()->getLocalId();
	this->entitiesOnMap.erase(localId);
	if (entity->isPlayer()) {
		this->playerOnMap.erase(localId);
	}
	if (entity->getBasicInformation()->getSector() == this) {
		entity->getBasicInformation()->setSector(nullptr);
	}
}

Entity* Map::Sector::getEntity(const word_t localId) {
	auto result = this->entitiesOnMap.find(localId);
	return (result == this->entitiesOnMap.cend() ? nullptr : result->second);
}

bool Map::Sector::hasEntity(Entity* entity) {
	return this->entitiesOnMap.count(entity->getBasicInformation()->getLocalId()) > 0;
}