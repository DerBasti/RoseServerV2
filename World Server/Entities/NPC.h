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
			const static byte_t MAX_AI_VARS = 0x05;
			const static byte_t MAX_OBJECT_VARS = 0x14;
			StoppableClock timer;
			AIP* data;
			word_t aiVar[MAX_AI_VARS];
			word_t objectVar[MAX_OBJECT_VARS];
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
			__inline word_t getAIVar(const byte_t idx) {
				return this->aiVar[idx];
			}
			__inline void setAIVar(const byte_t idx, int value) {
				//Var between 0 and 500
				this->aiVar[idx] = (std::min)(500, (std::max)(0, value));
			}
			__inline word_t getObjectVar(const byte_t idx) {
				return this->objectVar[idx];
			}
			void setObjectVar(const byte_t idx, word_t value) {
				if (idx == 0) {
					int before = this->getObjectVar(idx);
					this->objectVar[idx] = value;
					if (before != this->getObjectVar(idx)) {
						//??
					}
				}
				else {
					this->objectVar[idx] = value;
				}
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

	virtual void doAction();
	virtual void onSpawn();
	virtual void onDeath() {}

	__inline virtual bool isNPC() const { return true; }

	__inline word_t getTypeId() const {
		return this->typeId;
	}

	__inline float getDirection() const {
		return this->dir;
	}
	__inline AIHelper* getAI() const {
		return this->ai;
	}
	__inline const NPCSTB::Entry* getNPCData() const {
		return this->npcData;
	}
	__inline void setOwner(Entity* entity) {
		this->owner = entity;
	}
	__inline Entity* getOwner() const {
		return this->owner;
	}
	__inline word_t getOwnerId() const {
		return (this->getOwner() == nullptr ? 0x00 : this->getOwner()->getBasicInformation()->getLocalId());
	}
};

#endif //__ROSE_NPC__