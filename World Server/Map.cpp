#include "Map.h"
#include "Entities\Entity.h"
#include "Entities\NPC.h"
#include "Entities\Monster.h"

Map::Map(const byte_t id, ZON* zoneData, std::vector<VFS::Entry>& ifoFiles) {
	this->id = id;
	this->zone = zoneData;

	if (zoneData != nullptr) {
		this->createSectors(ifoFiles);
	}
	else { // add a default sector, just in case
		this->sectors.push_back(new Map::Sector(0, this, Position()));
	}
}

Map::~Map() {
	if (this->zone) {
		delete this->zone;
	}
	this->zone = nullptr;

	std::for_each(this->sectors.begin(), this->sectors.end(), [](Sector* sector) {
		delete sector;
		sector = nullptr;
	});
	this->sectors.clear();

	std::for_each(this->spawns.begin(), this->spawns.end(), [](MonsterSpawn* spawn) {
		delete spawn;
		spawn = nullptr;
	});
	this->spawns.clear();

	std::for_each(this->telegates.begin(), this->telegates.end(), [](Telegate* gate) {
		delete gate;
		gate = nullptr;
	});
	this->telegates.clear();
}

void Map::createSectors(std::vector<VFS::Entry>& entries) {
	const byte_t ifoMin = 28;
	const byte_t ifoMax = 40;
	const byte_t ifoDiff = ifoMax - ifoMin;
	const dword_t mapSize = ifoDiff * IFO::DEFAULT_SECTOR_SIZE;
	Position positionMin(ifoMin * IFO::DEFAULT_SECTOR_SIZE, ifoMin * IFO::DEFAULT_SECTOR_SIZE);

	const dword_t subSectorSize = this->getZoneData()->getSectorSize();
	const word_t amountOfSectorsOnAxis = static_cast<word_t>(mapSize / subSectorSize) + 1;

	this->sectorDescriptor = SectorDimensions(amountOfSectorsOnAxis, amountOfSectorsOnAxis);

	for (unsigned int i = 0; i < amountOfSectorsOnAxis; i++) {
		for (unsigned int j = 0; j < amountOfSectorsOnAxis; j++) {
			Position sectorPosition = positionMin + Position(static_cast<float>(j*subSectorSize), static_cast<float>(i*subSectorSize));
			this->sectors.push_back(new Sector(j + (i * amountOfSectorsOnAxis), this, sectorPosition));
		}
	}
}

void Map::detectMinMaxSectors(std::vector<VFS::Entry>& entries, word_t* xSector, word_t* ySector) {
	struct _Margin {
		word_t x;
		word_t y;
	};

	_Margin minimum;
	_Margin maximum;
	_Margin current;

	minimum.x = minimum.y = 0xFF;
	maximum.x = maximum.y = 0;

	std::for_each(entries.begin(), entries.end(), [&](VFS::Entry& entry) {
		String fullName = entry.getPathInVFS();
		String fileName = fullName.substring(fullName.findLastOf("\\") + 1);

		//Type of naming style: YY_XX.ifo
		current.x = (fileName.substring(fileName.findLastOf("_") + 1)).toByte();
		current.y = fileName.toByte();

		minimum.x = (current.x < minimum.x ? current.x : minimum.x);
		maximum.x = (current.x > maximum.x ? current.x : maximum.x);

		minimum.y = (current.y < minimum.y ? current.y : minimum.y);
		maximum.y = (current.y > maximum.y ? current.y : maximum.y);
	});

	xSector[0] = minimum.x;
	xSector[1] = maximum.x;

	ySector[0] = minimum.y;
	ySector[1] = maximum.y;
}

void Map::getVisibleSectors(Map::Sector* sector, std::map<word_t, Map::Sector*>& resultMap) const {
	resultMap.clear();
	if (!sector) {
		return;
	}
	word_t id = sector->getId();

	std::function<bool(const word_t)> isLeftmost = [&](const word_t id) {
		return (id % this->getSectorDescriptor().getAmountOfX()) == 0;
	};
	std::function<bool(const word_t)> isRightmost = [&](const word_t id) {
		byte_t rightMostId = this->getSectorDescriptor().getAmountOfX() - 1;
		return (id % this->getSectorDescriptor().getAmountOfX()) == rightMostId;
	};

	std::function<bool(const word_t)> isFirstRow = [&](const word_t id) {
		return (id / this->getSectorDescriptor().getAmountOfX()) == 0;
	};
	std::function<bool(const word_t)> isLastRow = [&](const word_t id) {
		return (id / this->getSectorDescriptor().getAmountOfX()) == (this->getSectorDescriptor().getAmountOfY() - 1);
	};

	bool isLeftmostFlag = isLeftmost(id);
	bool isRightmostFlag = isRightmost(id);

	unsigned int idx = 0;
	if (!isFirstRow(id)) {
		word_t startOffset = static_cast<word_t>((isLeftmostFlag ? 0x01 : 0x00));
		word_t endOffset = static_cast<word_t>((isRightmostFlag ? 0x02 : 0x03));
		for (unsigned int i = startOffset; i < endOffset; i++) {
			word_t upperId = id - this->sectorDescriptor.getAmountOfX() - 1 + i;
			Map::Sector* newSector = nullptr;
			if (upperId < id && upperId < this->sectors.size()) {
				newSector = this->sectors[upperId];
			}
			resultMap[(idx - startOffset)] = newSector;
			idx++;
		}
	}
	word_t positionInRow = (id % this->sectorDescriptor.getAmountOfX());
	word_t leftId = id - 1; word_t rightId = id + 1;

	if (!isLeftmostFlag && leftId < this->sectors.size()) { //not left-most node -> has a left neighbor
		resultMap[leftId] = this->sectors[leftId];
	}

	resultMap[sector->getId()] = sector;

	if (!isRightmostFlag && rightId < this->sectors.size()) {
		resultMap[rightId] = this->sectors[rightId];
	}

	if (!isLastRow(id)) {
		word_t startOffset = static_cast<word_t>((isLeftmostFlag ? 0x01 : 0x00));
		word_t endOffset = static_cast<word_t>((isRightmostFlag ? 0x02 : 0x03));
		for (unsigned int i = startOffset; i < endOffset; i++) {
			word_t lowerId = id + this->sectorDescriptor.getAmountOfX() - 1 + i;
			if (lowerId < id || lowerId >= this->sectors.size()) {
				continue;
			}
			resultMap[lowerId] = this->sectors[lowerId];
		}
	}
}

bool Map::isActive() const {
	return this->playerOnMap.size() > 0;
}

bool Map::addTelegate(const word_t id, const SingleTelegate& src, const SingleTelegate& dest) {
	this->telegates.push_back(new Telegate(id, src, dest));
	return true;
}

bool Map::addNPC(const IFO::NPC& npcData) {
	NPC* npc = new NPC(npcData.getNpcId(), this->getId(), npcData.getPosition(), npcData.getDirection());
	return npc != nullptr;
}

bool Map::addSpawn(const IFO::Spawn& spawn) {
	if (spawn.getRoundAmount() == 0) {
		return false;
	}
	MonsterSpawn *newSpawn = new MonsterSpawn(this->getId(), spawn);
	this->spawns.push_back(newSpawn);
	return true;
}

bool Map::addEntity(Entity* entity) {
	entity->getPositionInformation()->setMap(this);

	word_t newLocalId = 0x00;
	word_t amountOfTries = 0x00;
	const word_t MAX_AMOUNT_OF_TRIES = 0x400;
	do {
		newLocalId = rand();
	} while (newLocalId == 0 || (newLocalId > 0 && this->entitiesOnMap.count(newLocalId) > 0));

	if (amountOfTries >= MAX_AMOUNT_OF_TRIES) {
		return false;
	}
	this->entitiesOnMap[newLocalId] = entity;
	if (entity->isPlayer()) {
		this->playerOnMap[newLocalId] = entity;
	}
	entity->getBasicInformation()->setLocalId(newLocalId);
	return true;
}

Map::Sector* Map::getBestSector(Entity* entity) const {
	if (entity != nullptr) {
		auto sector = entity->getBasicInformation()->getSector();
		if (sector != nullptr && entity->getPositionInformation()->getCurrent().distanceTo(sector->getPosition()) < this->getSectorSize()) {
			return sector;
		}
	}
	return this->getBestSector(entity != nullptr ? entity->getPositionInformation()->getCurrent() : Position());
}

Map::Sector* Map::getBestSector(const Position& pos) const {
	auto firstSector = this->getSector(0);
	register float sectorSize = static_cast<float>(this->getSectorSize());
	float fX = pos.getX() - firstSector->getPosition().getX();
	float fY = pos.getY() - firstSector->getPosition().getY();

	std::function<word_t(const float, const word_t)> offsetCalculation = [&](const float value, const word_t max) {
		long tmpReturnValue = static_cast<long>(roundf(value / sectorSize));
		if (tmpReturnValue < 0) {
			tmpReturnValue = 0;
		}
		else if (tmpReturnValue >= max) {
			tmpReturnValue = (max - 1);
		}
		return static_cast<word_t>(tmpReturnValue);
	};

	word_t xSector = offsetCalculation(fX, this->getSectorDescriptor().getAmountOfX());
	word_t ySector = offsetCalculation(fY, this->getSectorDescriptor().getAmountOfY());

	word_t totalId = ySector * this->getSectorDescriptor().getAmountOfX() + xSector;
	if (totalId < this->sectors.size()) {
		return this->sectors[totalId];
	}
	return nullptr;
}


bool Map::updateEntity(Entity* entity) const {
	if (!entity->getBasicInformation()->isIngame()) {
		return false;
	}
	Map::Sector* bestSector = this->getBestSector(entity);
	if (bestSector->hasEntity(entity)) {
		return false;
	}
	Map::Sector* oldSector = entity->getBasicInformation()->getSector();
	if (oldSector != nullptr) {
		oldSector->removeEntity(entity);
	}
	bestSector->addEntity(entity);

	std::map<word_t, Map::Sector*> visibleSectors;
	this->getVisibleSectors(bestSector, visibleSectors);
	entity->getVisuality()->update(visibleSectors);
	return true;
}

bool Map::hasEntity(Entity* entity) const {
	return this->hasEntity(entity != nullptr ? entity->getBasicInformation()->getLocalId(): static_cast<word_t>(0x00));
}

bool Map::hasEntity(const word_t localId) const {
	return this->entitiesOnMap.count(localId) > 0;
}

void Map::removeEntity(Entity* entity) {
	entity->getVisuality()->forceClear();

	this->entitiesOnMap.erase(entity->getBasicInformation()->getLocalId());
	if (entity->isPlayer()) {
		this->playerOnMap.erase(entity->getBasicInformation()->getLocalId());
	}

	entity->getBasicInformation()->getSector()->removeEntity(entity);
	entity->getBasicInformation()->setSector(nullptr);
	entity->getBasicInformation()->setLocalId(0x00);
}

Telegate* Map::getGate(const word_t id) {
	Telegate* result = nullptr;
	auto gatePtr = this->telegates.data();
	for (unsigned int i = 0; i < this->telegates.size(); i++, gatePtr++) {
		auto gate = *gatePtr;
		if (gate->getId() == id) {
			result = gate;
			break;
		}
	}
	return result;
}	

void Map::addInvalidEntity(Entity* entity) {
	this->invalidEntities[entity->getBasicInformation()->getLocalId()] = entity;
}

bool Map::isInvalidEntity(const word_t id) {
	return this->invalidEntities.count(id) > 0;
}