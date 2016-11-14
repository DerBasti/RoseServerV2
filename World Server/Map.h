#ifndef __ROSE_MAP__
#define __ROSE_MAP__

#ifdef _MSC_VER
#pragma once
#endif

#include "..\Common\datatypes.h"
#include <map>
#include <vector>

class Map {
public:
	class Sector {
	private:
		word_t sectorId;
	public:
		Sector() : Sector(0) {}
		explicit Sector(const word_t id) {
			this->sectorId = id;
		}

		__inline word_t getId() const {
			return this->sectorId;
		}
	};
private:
	const static word_t MAX_LOCAL_CLIENTS = 0xFFFF;

	word_t id;
	std::map<word_t, std::pair<class Entity*, Map::Sector*>> entitiesOnMap;
public:
	Map() : Map(0) {}
	explicit Map(const word_t id);
	virtual ~Map();

	bool addEntity(class Entity* entity);
	bool updateEntity(class Entity* entity);
	bool updateEntity(const word_t localId);
	void clearEntity(class Entity* entity);
	class Entity* getEntity(const word_t localId);

	bool hasEntity(const word_t localId) const;
	bool hasEntity(class Entity* entity) const;

	__inline word_t getId() const {
		return this->id;
	}
};

#endif