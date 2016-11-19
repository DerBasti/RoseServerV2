#ifndef __ROSE_MAP__
#define __ROSE_MAP__

#ifdef _MSC_VER
#pragma once
#endif

#include "..\Common\datatypes.h"
#include "FileTypes\ZON.h"
#include "FileTypes\IFO.h"
#include <map>
#include <vector>

class SingleTelegate {
private:
	Position position;
	byte_t mapId;
public:
	SingleTelegate() : SingleTelegate(Position(), 0) {}
	SingleTelegate(const Position& pos, const byte_t mapId) {
		this->position = pos;
		this->mapId = mapId;
	}
	__inline Position getPosition() const {
		return this->position;
	}
	__inline byte_t getMapId() const {
		return this->mapId;
	}
};

class Telegate {
private:
	SingleTelegate source;
	SingleTelegate destination;
public:
	Telegate() {}
	Telegate(const SingleTelegate& src, const SingleTelegate& dest) {
		this->source = src;
		this->destination = dest;
	}
	__inline const SingleTelegate& getSource() const {
		return this->source;
	}
	__inline const SingleTelegate& getDestination() const {
		return this->destination;
	}
};

class Map {
public:
	class Sector {
	private:
		word_t id;
		Map* parent;
		Position position;
		std::map<word_t, class Entity*> entitiesOnMap;
		std::map<word_t, class Entity*> playerOnMap;
	public:
		Sector() : Sector(0, nullptr, Position()) {}
		Sector(const word_t id, Map* map, const Position& pos) {
			this->id = id;
			this->parent = map;
			this->position = pos;
		}

		typedef std::map<word_t, class Entity*>::iterator EntityIterator;

		void addEntity(class Entity* entity);
		void removeEntity(class Entity* entity);
		class Entity* getEntity(const word_t localId);
		bool hasEntity(class Entity* entity);

		__inline EntityIterator beginEntities() {
			return this->entitiesOnMap.begin();
		}
		__inline EntityIterator endEntities() {
			return this->entitiesOnMap.end();
		}
		__inline EntityIterator beginPlayer() {
			return this->playerOnMap.begin();
		}
		__inline EntityIterator endPlayer() {
			return this->playerOnMap.end();
		}

		__inline word_t getId() const {
			return this->id;
		}
		__inline bool isActive() const {
			return this->playerOnMap.size() > 0;
		}

		__inline Map* getParent() const {
			return this->parent;
		}
		__inline const Position& getPosition() const {
			return this->position;
		}
	};
private:
	const static word_t MAX_LOCAL_CLIENTS = 0xFFFF;
	const static word_t DEFAULT_SECTOR_SIZE_MULTIPLIER = 16000;

	class SectorDimensions {
	private:
		word_t amountOfX;
		word_t amountOfY;

	public:
		SectorDimensions() {}
		SectorDimensions(word_t amountX, word_t amountY) {
			this->amountOfX = amountX;
			this->amountOfY = amountY;

		}
		virtual ~SectorDimensions() {}

		__inline word_t getAmountOfX() const {
			return this->amountOfX;
		}
		__inline word_t getAmountOfY() const {
			return this->amountOfY;
		}
	};

	ZON *zone;
	byte_t id;
	std::map<word_t, class Entity*> entitiesOnMap;
	std::map<word_t, class Entity*> playerOnMap;

	std::vector<Sector*> sectors;
	std::vector<MonsterSpawn*> spawns;
	std::vector<Telegate> telegates;

	SectorDimensions sectorDescriptor;

	void createSectors(std::vector<VFS::Entry>& entries);
	void detectMinMaxSectors(std::vector<VFS::Entry>& entries, word_t* xSector, word_t* ySector);
	Map::Sector* getBestSector(class Entity* entity) const;
	Map::Sector* getBestSector(const class Position& pos) const;

	__inline const SectorDimensions& getSectorDescriptor() const {
		return this->sectorDescriptor;
	}
	std::map<word_t, Map::Sector*> getVisibleSectors(Map::Sector*) const;
public:
	typedef std::map<word_t, class Entity*>::iterator EntityIterator;
	typedef std::vector<Sector*>::iterator SectorIterator;

	Map(const byte_t id, ZON* zoneData, std::vector<VFS::Entry>& ifoFiles);
	virtual ~Map();

	bool addEntity(class Entity* entity);
	bool updateEntity(class Entity* entity);
	void removeEntity(class Entity* entity);

	bool hasEntity(const word_t localId) const;
	bool hasEntity(class Entity* entity) const;

	bool isActive() const;

	bool addNPC(const IFO::NPC& npc);
	bool addSpawn(const IFO::Spawn& spawn);
	bool addTelegate(const SingleTelegate& src, const SingleTelegate& dest);

	__inline EntityIterator beginEntities() {
		return this->entitiesOnMap.begin();
	}
	__inline EntityIterator endEntities() {
		return this->entitiesOnMap.end();
	}

	__inline SectorIterator beginSectors() {
		return this->sectors.begin();
	}
	__inline SectorIterator endSectors() {
		return this->sectors.end();
	}
	__inline Sector* getSector(const byte_t id) const {
		return (this->sectors.size() <= id ? nullptr : this->sectors[id]);
	}

	__inline byte_t getId() const {
		return this->id;
	}

	__inline ZON* getZoneData() const {
		return this->zone;
	}

	__inline bool isValid() const {
		return this->getZoneData() != nullptr;
	}

	__inline dword_t getSectorSize() const {
		return (!this->isValid() ? 4000 : this->getZoneData()->getSectorSize());
	}
};

#endif