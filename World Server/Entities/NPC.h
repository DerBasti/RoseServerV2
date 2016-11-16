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
public:
	NPC() {}
	NPC(const word_t npcId, const float direction);
	virtual ~NPC() {}

	__inline word_t getTypeId() const {
		return this->typeId;
	}

	__inline virtual bool isNPC() const { return true; }
};

#endif //__ROSE_NPC__