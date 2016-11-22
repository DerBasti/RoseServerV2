#ifndef __ROSE_NPC__
#define __ROSE_NPC__

#ifdef _MSC_VER
#pragma once
#endif

#include "Entity.h"
#include "..\FileTypes\AIP.h"
#include "..\FileTypes\STB.h"
#include "..\..\Common\BasicTypes\RingCounter.h"

class NPC : public Entity {
private:
	float dir;
protected:
	class AIHelper {
		private:
			StoppableClock timer;
			AIP* data;
		public:
			AIHelper(AIP* ptr) {
				this->data = ptr;
				this->timer.start();
			}
			__inline AIP* getData() const {
				return this->data;
			}
			__inline bool isAIReady() const {
				return this->data != nullptr && this->timer.getDuration() >= this->data->getCheckInterval();
			}
			__inline void updateTimer() {
				this->timer.timeLap();
			}
	};
	word_t typeId;
	String name;

	AIHelper* ai;
	NPCSTB::Entry* npcData;
	Entity* owner;

	virtual bool sendNewDestinationVisually();

	virtual void updateAttackPower();
	virtual void updateMaxHP();
	virtual void updateMaxMP();
	virtual void updatePhysicalDefense();
	virtual void updateMagicalDefense();
	virtual void updateMovementSpeed();
	virtual void updateAttackSpeed();
	virtual void updateAttackRange();
public:
	NPC() {}
	NPC(const word_t npcId, const byte_t mapId, const Position& pos, const float direction);
	virtual ~NPC();

	__inline word_t getTypeId() const {
		return this->typeId;
	}
	virtual void doAction();
	virtual void onSpawn();
	virtual void onDeath() {}

	__inline virtual bool isNPC() const { return true; }

	__inline float getDirection() const {
		return this->dir;
	}
	__inline AIHelper* getAI() const {
		return this->ai;
	}
	__inline const NPCSTB::Entry* getNPCData() const {
		return this->npcData;
	}
};

#endif //__ROSE_NPC__