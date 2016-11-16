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

class Map {
public:
	class Sector {
	private:
		word_t sectorId;
		std::map<word_t, class Entity*> entitiesOnMap;
		std::map<word_t, class Entity*> playerOnMap;
	public:
		Sector() : Sector(0) {}
		explicit Sector(const word_t id) {
			this->sectorId = id;
		}

		__inline word_t getId() const {
			return this->sectorId;
		}

		void addEntity(class Entity* entity);
		void removeEntity(class Entity* entity);
		class Entity* getEntity(const word_t localId);
		bool hasEntity(class Entity* entity);

		__inline bool isActive() const {
			return this->playerOnMap.size() > 0;
		}
	};
private:
	const static word_t MAX_LOCAL_CLIENTS = 0xFFFF;

	ZON *zone;
	word_t id;
	std::map<word_t, std::pair<class Entity*, Map::Sector*>> entitiesOnMap;
	Map::Sector* getNearestSector(Map::Sector* currentSector, const class Position& pos) const;

	std::map<byte_t, Sector*> sectors;
	std::vector<MonsterSpawn*> spawns;

public:
	Map(const word_t id, ZON* zoneData, std::vector<IFO>& ifoData);
	virtual ~Map();

	bool addEntity(class Entity* entity);
	bool updateEntity(class Entity* entity);
	bool updateEntity(const word_t localId);
	void clearEntity(class Entity* entity);
	class Entity* getEntity(const word_t localId);

	bool hasEntity(const word_t localId) const;
	bool hasEntity(class Entity* entity) const;

	bool isActive() const;

	__inline word_t getId() const {
		return this->id;
	}
};

#endif