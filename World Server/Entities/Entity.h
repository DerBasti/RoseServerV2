#ifndef __ROSE_ENTITY__
#define __ROSE_ENTITY__

#ifdef _MSC_VER
#pragma once
#endif

#include "..\..\Common\BasicTypes\FunctionBinder.h"
#include "..\..\Common\BasicTypes\StoppableClock.h"
#include "..\..\Common\BasicTypes\Observable.h"
#include "..\Map.h"
#include <mutex>

typedef unsigned char byte_t;
typedef unsigned short word_t;
typedef unsigned long dword_t;
typedef unsigned long long qword_t;

class EntityInfo {
private:
	word_t localId;
	word_t teamId;
	bool ingameFlag;
	Map::Sector* sector;
public:
	EntityInfo() : EntityInfo(0) {}
	EntityInfo(const word_t localId) {
		this->localId = localId;
		this->ingameFlag = false;
		this->teamId = 0x00;
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
	__inline word_t getTeamId() const {
		return this->teamId;
	}
	__inline void setTeamId(const word_t teamId) {
		this->teamId = teamId;
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

	__inline const Position& getCurrent() const {
		return this->current;
	}
	__inline void setCurrent(const Position& p) {
		this->current = p;
	}
	__inline const Position& getDestination() const {
		return this->dest.getRefValue();
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
protected:
	Observable<byte_t> stanceId;
public:
	Stance() {
		stanceId = 0x00;
		stanceId.setOnNewValueAssigned([](byte_t){});
	}
	virtual ~Stance() {}

	__inline virtual bool isSitting() const {
		return false;
	}
	__inline virtual bool isDriving() const {
		return false;
	}
	__inline virtual bool isWalking() const {
		return false;
	}
	__inline virtual bool isRunning() const {
		return true;
	}
	__inline virtual void setSittingStance() {
	}
	__inline virtual void setWalkingStance() {
	}
	__inline virtual void setRunningStance() {
	}
	__inline virtual void setDrivingStance() {
	}
	__inline byte_t getId() const {
		return this->stanceId.getValue();
	}
	__inline void setId(const byte_t id) {
		this->stanceId = id;
	}
	__inline void setOnStanceChanged(std::function<void(byte_t)> f) {
		this->stanceId.setOnNewValueAssigned(f == nullptr ? [](byte_t){} : f);
	}
};

class NPCStance : public Stance {
private:
	const static byte_t WALKING = 0x00;
	const static byte_t RUNNING = 0x01;
public:
	NPCStance() {
		this->stanceId = RUNNING;
	}
	virtual ~NPCStance() {}

	__inline void setWalkingStance() {
		this->stanceId = NPCStance::WALKING;
	}
	__inline void setRunningStance() {
		this->stanceId = NPCStance::RUNNING;
	}

	__inline bool isWalking() const {
		return this->getId() == NPCStance::WALKING;
	}

	__inline bool isRunning() const {
		return this->getId() == NPCStance::RUNNING;
	}
};

class PlayerStance : public Stance {
private:
	const static byte_t SITTING = 0x01;
	const static byte_t WALKING = 0x02;
	const static byte_t RUNNING = 0x03;
	const static byte_t DRIVING = 0x04;
public:
	PlayerStance() {
		this->stanceId = RUNNING;
	}
	~PlayerStance() {}

	__inline bool isWalking() const {
		return this->getId() == PlayerStance::WALKING;
	}

	__inline bool isRunning() const {
		return this->getId() == PlayerStance::RUNNING;
	}

	__inline bool isDriving() const {
		return this->getId() == PlayerStance::DRIVING;
	}

	__inline bool isSitting() const {
		return this->getId() == PlayerStance::SITTING;
	}

	__inline void setWalkingStance() {
		this->stanceId = PlayerStance::WALKING;
	}
	__inline void setRunningStance() {
		this->stanceId = PlayerStance::RUNNING;
	}
	__inline void setDrivingStance() {
		this->stanceId = PlayerStance::DRIVING;
	}
	__inline void setSittingStance() {
		this->stanceId = PlayerStance::SITTING;
	}
};

class Stats {
protected:
	Observable<word_t> level;
	dword_t currentHp;
	dword_t maxHp;

	dword_t currentMp;
	dword_t maxMp;

	word_t accuracy;
	word_t movementSpeed;
	word_t stamina;

	word_t attackPower;
	word_t physicalDefense;
	word_t magicalDefense;
	word_t attackSpeed;
	float range;

	Stance* stance;
public:
	Stats() {
		this->currentHp = this->maxHp = 100;
		this->currentMp = this->maxMp = 100;
		this->accuracy = 50;
		this->stamina = 5000;
		this->attackPower = this->physicalDefense = this->magicalDefense = 50;
		this->range = 100.0f;
		this->attackSpeed = 100;
		this->movementSpeed = 425;
		this->stance = nullptr;
	}
	virtual ~Stats() {
		delete stance;
		stance = nullptr;
	}

	__inline unsigned long getHP() const {
		return this->currentHp;
	}
	__inline void setHP(const unsigned long current) {
		this->currentHp = (current > this->getMaxHP() ? this->getMaxHP() : current);
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
		this->currentMp = (current > this->getMaxHP() ? this->getMaxHP() : current);
	}
	__inline unsigned long getMaxMP() const {
		return this->maxMp;
	}
	__inline word_t getLevel() const {
		return this->level.getValue();
	}
	__inline void setLevel(const word_t level) {
		this->level = level;
	}
	__inline void setToNextLevel() {
		this->level = this->level + 1;
	}
	__inline void setOnLevelUp(std::function<void(word_t)> f) {
		this->level.setOnNewValueAssigned(f == nullptr ? [](word_t){} : f);
	}
	__inline word_t getAccuracy() const {
		return this->accuracy;
	}
	__inline void setAccuracy(const word_t acc) {
		this->accuracy = acc;
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
	__inline word_t getPhysicalDefense() const {
		return this->physicalDefense;
	}
	__inline void setPhysicalDefense(const word_t def) {
		this->physicalDefense = def;
	}
	__inline word_t getMagicalDefense() const {
		return this->magicalDefense;
	}
	__inline void setMagicalDefense(const word_t def) {
		this->magicalDefense = def;
	}
	__inline word_t getAttackSpeed() const {
		return this->attackSpeed;
	}
	__inline void setAttackSpeed(const word_t spd) {
		this->attackSpeed = spd;
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
	__inline Stance* getStance() const {
		return this->stance;
	}
	__inline void setStance(Stance* stance) {
		this->stance = stance;
	}
};

class Visuality {
public:
	const static byte_t MAXIMUM_VISIBLE_SECTORS = 9;
private:
	Map::Sector* visibleSectors[MAXIMUM_VISIBLE_SECTORS];
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
	void update(std::map<word_t, Map::Sector*>& resultMap);
	void forceClear();
	Entity* find(const word_t localId);

	__inline Map::Sector** getVisibleSectors() {
		return reinterpret_cast<Map::Sector**>(this->visibleSectors);
	}
};

class Combat {
public:
	class Type {
		private:
			byte_t id;
			Type(const byte_t id) {
				this->id = id;
			}
		public:
			Type() {}
			~Type() {}
			const static Combat::Type NONE;
			const static Combat::Type NORMAL;
			const static Combat::Type SKILL;
			
			Combat::Type& operator=(const Combat::Type& type) = default;

			bool operator==(const Combat::Type& type) const {
				return this->id == type.id;
			}
			bool operator!=(const Combat::Type& type) const {
				return !this->operator==(type);
			}
	};
private:
	Observable<class Entity*> target;
	Combat::Type type;
	WrappingTriggerClock attackTimer;
public:
	Combat() {
		this->target = nullptr;
		this->type = Combat::Type::NONE;
	}
	virtual ~Combat() {
		this->target = nullptr;
		this->type = Combat::Type::NONE;
	}

	class Entity* getTarget() const;
	void setTarget(class Entity* target);
	word_t getTargetId() const;

	void clear() {
		this->target = nullptr;
		this->type = Combat::Type::NONE;
		this->attackTimer.stop();
	}

	WrappingTriggerClock& getAttackTimer() {
		return this->attackTimer;
	}

	__inline void setOnNewTarget(std::function<void(Entity*)> f) {
		this->target.setOnNewValueAssigned(f == nullptr ? [](Entity*){} : f);
	}

	__inline Combat::Type getType() const {
		return this->type;
	}
	__inline void setType(const Combat::Type& type) {
		this->type = type;
	}
};

class Entity : public BasicObject {
protected:
	EntityInfo* basicIngameInformation;
	PositionInformation* positions;
	Stats* stats;
	Visuality* visuality;
	Combat* combat;
	class ZMO* currentAnimation;

	virtual void updateAttackPower() {}
	virtual void updateMaxHP() {}
	virtual void updateMaxMP() {}
	virtual void updatePhysicalDefense() {}
	virtual void updateMagicalDefense() {}
	virtual void updateMovementSpeed() {}
	virtual void updateAttackSpeed() {}
	virtual void updateAttackRange() {}

	bool sendToVisible(const Packet& pak);
	bool sendToVisible(const Packet& pak, Entity* exception);
	virtual bool sendEntityVisuallyAdded(Entity* entity) { return true; }
	virtual bool sendPlayerVisuallyAdded(Entity* entity, Packet& pak) { return true; }
	virtual bool sendNPCVisuallyAdded(Entity* entity, Packet& pak) { return true; }
	virtual bool sendMonsterVisuallyAdded(Entity* entity, Packet& pak) { return true; }

	virtual bool sendEntityVisuallyRemoved(Entity* entity) { return true; }

	virtual bool sendNewDestinationVisually() { return true; }
	virtual bool sendCurrentStance();

	virtual bool sendNewTarget();

	virtual void setAttackMotion();
public:

	Entity();
	virtual ~Entity();

	__inline PositionInformation* getPositionInformation() const {
		return this->positions;
	}
	__inline EntityInfo* getBasicInformation() const {
		return this->basicIngameInformation;
	}
	__inline Stats* getStats() const {
		return this->stats;
	}
	__inline Visuality* getVisuality() const {
		return this->visuality;
	}
	__inline Combat* getCombatInformation() const {
		return this->combat;
	}

	virtual void updateStats();
	void movementProc();
	virtual void doNormalAttack();
	virtual void doSkillAttack();
	virtual void doBuff();

	virtual void doAction();

	virtual bool isEnemyOf(Entity* entity) const { return (entity == nullptr ? false : this->getBasicInformation()->getTeamId() != entity->getBasicInformation()->getTeamId()); }

	virtual bool isPlayer() const { return false; }
	virtual bool isNPC() const { return false; }
	virtual bool isMonster() const { return false; }

	virtual float getSize() const { return 100.0f; }

	virtual void onDeath() {}

	virtual bool isActive() const {
		return true;
	}
	virtual void addDamage(Entity* dmgDealer, const dword_t damage);
};

extern FunctionBinder <Entity, dword_t, void(Entity::*)()> ZMOTRIGGER_TO_ID;

#endif