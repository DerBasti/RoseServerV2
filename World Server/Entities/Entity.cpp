#include "Entity.h"
#include "Player.h"
#include "..\..\Common\PacketIDs.h"

Entity::~Entity() {
	//TODO: Visuality on death
}

void Entity::movementProc() {
	Position current = this->getPositionInformation()->getCurrent();
	const Position dest = this->getPositionInformation()->getDestination();
	if (current == dest) {
		this->getPositionInformation()->updateDuration();
		return;
	}
	float distance = (current - dest).toLength();
	float threshold = 10;
	if (distance <= threshold) {
		this->getPositionInformation()->setCurrent(dest);
		this->getPositionInformation()->updateDuration();
		return;
	}
	unsigned long long timePassed = this->getPositionInformation()->updateDuration();
	float necessaryTime = (distance / static_cast<float>(this->getStats()->getMovementSpeed())) * 1000.0f;
	if (timePassed >= necessaryTime) {
		this->getPositionInformation()->setCurrent(dest);
		return;
	}
	float pathTraveled = timePassed * static_cast<float>(this->getStats()->getMovementSpeed()) / 1000.0f;
	float lineDiffs[2] = { dest.getX() - current.getX(), dest.getY() - current.getY() };
	float ratios[2] = { pathTraveled * (lineDiffs[0] / distance), pathTraveled * (lineDiffs[1] / distance) };
	this->getPositionInformation()->setCurrent(Position(current.getX() + ratios[0], current.getY() + ratios[1]));
	return;
}

void Visuality::update(std::map<word_t, Map::Sector*> newSectors) {
	if (newSectors.size() == 0) {
		return;
	}
	auto sector = (newSectors.begin())->second;
	Map* currentMap = sector->getParent();

	const float MAX_DISTANCE = static_cast<float>(1.5f * currentMap->getSectorSize());

	std::vector<Map::Sector*> toRemove;
	std::for_each(this->visibleSectors.begin(), this->visibleSectors.end(), [&](std::pair<const word_t, Map::Sector*> pair) {
		Map::Sector* sector = pair.second;
		if (newSectors.count(sector->getId()) == 0) {
			this->removeSector(sector);
			toRemove.push_back(sector);
		}
		else {
			newSectors.erase(sector->getId());
		}
	});
	std::for_each(toRemove.begin(), toRemove.end(), [&](Map::Sector* sector) {
		this->visibleSectors.erase(sector->getId());
	});
	std::for_each(newSectors.begin(), newSectors.end(), [&](std::pair<const word_t, Map::Sector*> pair) {
		Map::Sector *sector = pair.second;
		this->visibleSectors[sector->getId()] = sector;
		this->addSector(sector);
	});
}

void Visuality::forceClear() {
	std::for_each(this->visibleSectors.begin(), this->visibleSectors.end(), [&](std::pair<const word_t, Map::Sector*> pair) {
		Map::Sector* sector = pair.second;
		this->removeSector(sector);
	});
	this->visibleSectors.clear();
}

bool Entity::sendToVisible(const Packet& pak) {
	bool result = true;
	auto visibleSectors = this->getVisuality()->getVisibleSectors();
	std::for_each(visibleSectors.begin(), visibleSectors.end(), [&](std::pair<const word_t, Map::Sector*> p) {
		Map::Sector *sector = p.second;
		std::for_each(sector->beginPlayer(), sector->endPlayer(), [&](std::pair<const word_t, Entity*> otherPair) {
			Player *player = static_cast<Player*>(otherPair.second);
			result &= player->sendPacket(pak);
		});
	});
	return result;
}

bool Entity::sendNewDestinationVisually() {
	auto destination = this->getPositionInformation()->getDestination();
	Packet pak(PacketID::World::Response::MOVEMENT_PLAYER);
	pak.addWord(this->getBasicInformation()->getLocalId());
	pak.addWord(0x00);//pak.addWord(target != nullptr ? target->getLocalId() : 0x00);
	pak.addWord(this->getStats()->getMovementSpeed());
	pak.addFloat(destination.getX());
	pak.addFloat(destination.getY());
	pak.addWord(0xcdcd); //Z
	return this->sendToVisible(pak);
}