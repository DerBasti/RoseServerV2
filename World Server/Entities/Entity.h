#ifndef __ROSE_ENTITY__
#define __ROSE_ENTITY__

#ifdef _MSC_VER
#pragma once
#endif

#include "..\..\Common\BasicTypes\StoppableClock.h"
#include "..\..\Common\BasicTypes\Observable.h"
#include "..\Map.h"

typedef unsigned char byte_t;
typedef unsigned short word_t;
typedef unsigned long dword_t;
typedef unsigned long long qword_t;

class EntityInfo {
private:
	word_t localId;
	bool ingameFlag;
public:
	EntityInfo() : EntityInfo(0) {}
	EntityInfo(const word_t localId) {
		this->localId = localId;
		this->ingameFlag = false;
	}
	virtual ~EntityInfo() {}

	__inline word_t getLocalId() const {
		return this->localId;
	}
	__inline void setLocalId(const word_t id) {
		this->localId = id;
	}

	__inline void setIngameFlag(bool flag) {
		this->ingameFlag = flag;
	}
	__inline bool isIngame() const {
		return this->ingameFlag;
	}
};

class Position {
private:
	float x;
	float y;
	StoppableClock lastCheckTime;
public:
	Position() : Position(0.0f, 0.0f) {}
	Position(const float x, const float y) {
		this->x = x;
		this->y = y;
		this->lastCheckTime.start();
	}
	virtual ~Position() {}
	Position& operator=(const Position& pos) = default;

	__inline float getX() const {
		return this->x;
	}
	__inline float getY() const {
		return this->y;
	}
};

class PositionInformation {
private:
	Position current;
	Position dest;
	Map* map;
	Map::Sector* currentSector;
public:
	PositionInformation() {
		map = nullptr;
		currentSector = nullptr;
	}
	virtual ~PositionInformation() {}

	PositionInformation& operator=(const PositionInformation& other) = default;

	__inline Map* getMap() const {
		return this->map;
	}
	__inline void setMap(Map* map) {
		this->map = map;
	}

	__inline Position getCurrent() const {
		return this->current;
	}
	__inline void setCurrent(const Position& p) {
		this->current = p;
	}
	__inline Position getDestination() const {
		return this->dest;
	}
	__inline void setDestination(const Position& p) {
		this->dest = p;
	}
};

class Stance {
private:
	byte_t stanceId;
	dword_t walkSpeed;
	dword_t runSpeed;
public:
	Stance() : Stance(0x00, 0x00, 0x00) {}
	Stance(const byte_t stanceId, const dword_t walkSpeed, const dword_t runSpeed) {
		this->stanceId = stanceId;
		this->walkSpeed = walkSpeed;
		this->runSpeed = runSpeed;
	}
	virtual ~Stance() {}

	operator byte_t() const {
		return this->stanceId;
	}
	__inline unsigned long getWalkSpeed() const {
		return this->walkSpeed;
	}
	__inline void setWalkSpeed(const dword_t speed) {
		this->walkSpeed = speed;
	}
	__inline unsigned long getRunSpeed() const {
		return this->runSpeed;
	}
	__inline void setRunSpeed(const dword_t speed) {
		this->runSpeed = speed;
	}
	__inline bool isWalking() const {
		return false;
	}
	__inline bool isRunning() const {
		return true;
	}
};

class Stats {
protected:
	dword_t currentHp;
	dword_t maxHp;

	dword_t currentMp;
	dword_t maxMp;

	dword_t movementSpeed;
	word_t stamina;

	Observable<Stance> stance;
	__inline void setMaxHP(const unsigned long newMax) {
		this->maxHp = newMax;
	}
	__inline void setMaxMP(const unsigned long newMax) {
		this->maxMp = newMax;
	}
public:
	Stats() {
		stance.setOnNewValueAssigned([&](Stance stance) {
			if (stance.isWalking()) {
				this->movementSpeed = stance.getWalkSpeed();
			}
			else if (stance.isRunning()) {
				this->movementSpeed = stance.getRunSpeed();
			}
		});
		this->currentHp = this->maxHp = 100;
		this->currentMp = this->maxMp = 100;
		this->stamina = 5000;
	}
	virtual ~Stats() {}

	__inline unsigned long getHP() const {
		return this->currentHp;
	}
	__inline void setHP(const unsigned long current) {
		this->currentHp = (this->getHP() + current > this->getMaxHP() ? this->getMaxHP() : this->getHP() + current);
	}
	__inline unsigned long getMaxHP() const {
		return this->maxHp;
	}

	__inline unsigned long getMP() const {
		return this->currentMp;
	}
	__inline void setMP(const unsigned long current) {
		this->currentMp = (this->getMP() + current > this->getMaxHP() ? this->getMaxHP() : this->getMP() + current);
	}
	__inline unsigned long getMaxMP() const {
		return this->maxMp;
	}
	__inline word_t getStamina() const {
		return this->stamina;
	}
};

class Entity {
private:
	EntityInfo basicIngameInformation;
	PositionInformation positions;
	Stats stats;
public:
	Entity() {	}
	virtual ~Entity() {}

	__inline PositionInformation* getPositionInformation() {
		return &this->positions;
	}
	__inline EntityInfo* getBasicInformation() {
		return &this->basicIngameInformation;
	}
	__inline Stats* getStats() {
		return &this->stats;
	}
};

#endif