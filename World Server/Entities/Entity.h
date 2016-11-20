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
	Map::Sector* sector;
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
	__inline Map::Sector* getSector() const {
		return this->sector;
	}
	__inline void setSector(Map::Sector* sector) {
		this->sector = sector;
	}
};

class PositionInformation {
private:
	Position current;
	Observable<Position> dest;
	StoppableClock lastCheckTime;
	Map* map;
public:
	PositionInformation() {
		map = nullptr;
		this->lastCheckTime.start();
	}
	virtual ~PositionInformation() {}

	PositionInformation& operator=(const PositionInformation& other) = default;

	__inline Map* getMap() const {
		return this->map;
	}
	__inline void setMap(Map* map) {
		this->map = map;
	}

	__inline unsigned long long updateDuration() {
		return this->lastCheckTime.timeLap();
	}

	__inline Position getCurrent() const {
		return this->current;
	}
	__inline void setCurrent(const Position& p) {
		this->current = p;
	}
	__inline Position getDestination() const {
		return this->dest.getValue();
	}
	__inline void setOnDestinationAssigned(std::function<void(Position)> func) {
		this->dest.setOnNewValueAssigned(func);
	}
	__inline void setDestination(const Position& p) {
		this->dest = p;
	}
	__inline bool isIdling() const {
		return this->getCurrent() == this->getDestination();
	}
};

class Stance {
private:
	byte_t stanceId;
	word_t walkSpeed;
	word_t runSpeed;
public:
	Stance() : Stance(0x00, 0x00, 0x00) {}
	Stance(const byte_t stanceId, const word_t walkSpeed, const word_t runSpeed) {
		this->stanceId = stanceId;
		this->walkSpeed = walkSpeed;
		this->runSpeed = runSpeed;
	}
	virtual ~Stance() {}

	operator byte_t() const {
		return this->stanceId;
	}
	__inline word_t getWalkSpeed() const {
		return this->walkSpeed;
	}
	__inline void setWalkSpeed(const word_t speed) {
		this->walkSpeed = speed;
	}
	__inline word_t getRunSpeed() const {
		return this->runSpeed;
	}
	__inline void setRunSpeed(const word_t speed) {
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

	word_t movementSpeed;
	word_t stamina;

	word_t attackPower;
	word_t physicalDefense;
	word_t magicalDefense;
	float range;

	Observable<Stance> stance;
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
		this->attackPower = this->physicalDefense = this->magicalDefense = 50;
		this->range = 100.0f;
		this->movementSpeed = 425;
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

	__inline void setMaxHP(const unsigned long newMax) {
		this->maxHp = newMax;
	}
	__inline void setMaxMP(const unsigned long newMax) {
		this->maxMp = newMax;
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
	__inline word_t getAttackPower() const {
		return this->attackPower;
	}
	__inline void setAttackPower(const word_t atk) {
		this->attackPower = atk;
	}
	__inline float getAttackRange() const {
		return this->range;
	}
	__inline void setAttackRange(const float range) {
		this->range = range;
	}
	__inline word_t getMovementSpeed() const {
		return this->movementSpeed;
	}
	__inline void setMovementSpeed(const word_t moveSpd) {
		this->movementSpeed = moveSpd;
	}
};

class Visuality {
private:
	std::map<word_t, Map::Sector*> visibleSectors;
	std::function<void(Map::Sector*)> newSectorFunction;
	std::function<void(Map::Sector*)> removeSectorFunction;

	__inline std::function<void(Map::Sector*)> getEmptyFunction() {
		return [](Map::Sector*){};
	}
public:
	Visuality() {
		this->setOnNewSector(nullptr);
		this->setOnRemoveSector(nullptr);
	}
	virtual ~Visuality() {
		std::for_each(this->visibleSectors.begin(), this->visibleSectors.end(), [&](std::pair<const word_t, Map::Sector*> sectorPair) {
			this->removeSectorFunction(sectorPair.second);
		});
		this->visibleSectors.clear();
	}
	__inline void setOnNewSector(std::function<void(Map::Sector*)> func) {
		this->newSectorFunction = (func == nullptr ? this->getEmptyFunction() : func);
	}
	__inline void setOnRemoveSector(std::function<void(Map::Sector*)> func) {
		this->removeSectorFunction = (func == nullptr ? this->getEmptyFunction() : func);
	}
	void addSector(Map::Sector* sector) {
		this->newSectorFunction(sector);
	}
	void removeSector(Map::Sector* sector) {
		this->removeSectorFunction(sector);
	}
	void update(std::map<word_t, Map::Sector*> newVisibleSectors);
	void forceClear();

	__inline std::map<word_t, Map::Sector*> getVisibleSectors() const {
		return this->visibleSectors;
	}
};

class Entity {
private:
	EntityInfo basicIngameInformation;
	PositionInformation positions;
	Stats stats;
	Visuality visuality;
protected:
	bool sendToVisible(const Packet& pak);
	virtual bool sendEntityVisuallyAdded(Entity* entity) {
		return true;
	}
	virtual bool sendPlayerVisuallyAdded(Entity* entity, Packet& pak) { return true; }
	virtual bool sendNPCVisuallyAdded(Entity* entity, Packet& pak) { return true; }
	virtual bool sendMonsterVisuallyAdded(Entity* entity, Packet& pak) { return true; }

	virtual bool sendEntityVisuallyRemoved(Entity* entity) {
		return true;
	}
	virtual bool sendNewDestinationVisually();
public:
	Entity() {
		auto onNewSectorFunc = [&](Map::Sector* sector) {
			std::for_each(sector->beginEntities(), sector->endEntities(), [&](std::pair<const word_t, Entity*> entityPair) {
				Entity* other = entityPair.second;
				if (other != this && other != nullptr) {
					this->sendEntityVisuallyAdded(other);
					other->sendEntityVisuallyAdded(this);
				}
			});
		};
		auto onRemoveSectorFunc = [&](Map::Sector* sector) {
			std::for_each(sector->beginEntities(), sector->endEntities(), [&](std::pair<const word_t, Entity*> entityPair) {
				Entity* other = entityPair.second;
				if (other != this && other != nullptr) {
					this->sendEntityVisuallyRemoved(other);
					other->sendEntityVisuallyRemoved(this);
				}
			});
		};
		this->getPositionInformation()->setOnDestinationAssigned([this](Position old) {
			this->sendNewDestinationVisually();
		});
		this->getVisuality()->setOnNewSector(onNewSectorFunc);
		this->getVisuality()->setOnRemoveSector(onRemoveSectorFunc);
	}
	virtual ~Entity();

	__inline PositionInformation* getPositionInformation() {
		return &this->positions;
	}
	__inline EntityInfo* getBasicInformation() {
		return &this->basicIngameInformation;
	}
	__inline Stats* getStats() {
		return &this->stats;
	}
	__inline Visuality* getVisuality() {
		return &this->visuality;
	}

	void movementProc();

	virtual void doAction() {
	}

	virtual bool isPlayer() const { return false; }
	virtual bool isNPC() const { return false; }
	virtual bool isMonster() const { return false; }

	virtual void onDeath() {}
};

#endif