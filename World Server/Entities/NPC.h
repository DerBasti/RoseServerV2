#ifndef __ROSE_NPC__
#define __ROSE_NPC__

#ifdef _MSC_VER
#pragma once
#endif

#include "Entity.h"

class NPC : public Entity {
private:
	float dir;
protected:
	word_t typeId;
	String name;
public:
	
	NPC() {}
	NPC(const word_t npcId, const byte_t mapId, const Position& pos, const float direction);
	virtual ~NPC() {}

	__inline word_t getTypeId() const {
		return this->typeId;
	}

	__inline virtual bool isNPC() const { return true; }
	__inline float getDirection() const {
		return this->dir;
	}

};

#endif //__ROSE_NPC__