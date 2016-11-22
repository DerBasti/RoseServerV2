#ifndef __ROSE_MONSTER__
#define __ROSE_MONSTER__

#ifdef _MSC_VER
#pragma once
#endif

#include "NPC.h"

class Monster : public NPC {
private:
	class MonsterSpawn* spawnReference;
public:
	Monster(const word_t typeId, const byte_t mapId, const Position& pos, class MonsterSpawn* spawnRef);
	virtual ~Monster();

	void onDeath();

	__inline MonsterSpawn* getSpawn() const {
		return this->spawnReference;
	}
	__inline bool isMonster() const { return true; }
	__inline bool isNPC() const { return false; }
};

#endif