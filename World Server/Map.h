#ifndef __ROSE_MAP__
#define __ROSE_MAP__

#ifdef _MSC_VER
#pragma once
#endif

#include "..\Common\datatypes.h"
#include <map>

class Map {
public:
	class Sector {
	private:
	public:
	};
private:
	const static word_t MAX_LOCAL_CLIENTS = 0x10000;

	word_t id;
	std::map<word_t, Sector*> sectors;
	std::map<word_t, class Entity*> entities;
public:
	Map() : Map(0) {}
	explicit Map(const word_t id);
	virtual ~Map();

	bool setEntity(class Entity* entity);
	void clearEntity(class Entity* entity);
	class Entity* getEntity(const word_t localId);

	bool hasEntity(const word_t localId) const;
	bool hasEntity(class Entity* entity) const;

	__inline word_t getId() const {
		return this->id;
	}
};

#endif