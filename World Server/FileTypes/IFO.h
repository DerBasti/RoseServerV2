#ifndef __ROSE_IFO__
#define __ROSE_IFO__

#ifdef _MSC_VER
#pragma once
#endif

#include "VFS.h"
#include "..\..\Common\datatypes.h"

class IFO {
public:
	class Entry {
		private:
			String name;
			dword_t objectType;
			dword_t objectId;
			float direction;
			Position position;
		protected:
			Entry() {}
		public:
			const static byte_t NPC_ENTRY = 0x02;
			const static byte_t SPAWN_ENTRY = 0x08;
			const static byte_t TELEGATE_ENTRY = 0x0A;
			const static byte_t EVENT_ENTRY = 0x0C;
			Entry(BufferedFileReader& bfr);
			virtual ~Entry();

			__inline Position getPosition() const {
				return this->position;
			}

			__inline float getDirection() const {
				return this->direction;
			}

			__inline String getName() const {
				return this->name;
			}

			__inline dword_t getObjectId() const {
				return this->objectId;
			}
	};
	class Spawn : public Entry {
	public:
		class Round {
		private:
			String mobName;
			dword_t mobId;
			dword_t amount;
			bool tacticalFlag;
		public:
			Round() : Round(String(), 0, 0, false) {}
			Round(const String& name, const dword_t id, const word_t count, bool tacticalFlag) {
				this->mobName = name;
				this->mobId = id;
				this->amount = count;
				this->tacticalFlag = tacticalFlag;
			}
			virtual ~Round() {}

			Round& operator=(const Round& round) = default;

			__inline String getName() const {
				return this->mobName;
			}
			__inline dword_t getMobId() const {
				return this->mobId;
			}
			__inline dword_t getAmount() const {
				return this->amount;
			}
		};
	private:
		String name;
		std::vector<Round> allRounds;
		dword_t respawnInterval;
		dword_t maxAmount;
		float allowedDistance;
		dword_t tacticalPoints;
	public:
		Spawn(BufferedFileReader& bfr);
		virtual ~Spawn();
	};
	typedef Entry Telegate;
	class NPC : public Entry{
	private:
		String conFile;
	public:
		NPC(BufferedFileReader& bfr);
		virtual ~NPC();

		__inline dword_t getNpcId() const {
			return this->getObjectId();
		}

		__inline String getConFile() const {
			return this->conFile;
		}
	};
private:
	std::vector<IFO::Spawn> spawns;
	std::vector<IFO::NPC> npcs;
	std::vector<IFO::Telegate> telegates;
public:
	IFO(const String& path, const SharedArrayPtr<char>& data);
	virtual ~IFO();

	std::vector<IFO::Spawn>& getSpawns() {
		return this->spawns;
	}
	std::vector<IFO::NPC>& getNPCs() {
		return this->npcs;
	}
	std::vector<IFO::Telegate> getTelegates() {
		return this->telegates;
	}
};

//Rest implemented in: Monster.cpp
class MonsterSpawn {
private:
	typedef IFO::Spawn Spawn;
	Spawn spawn;

	std::vector<class Monster*> spawnedMonsters;
	time_t nextSpawnWave;
	
public:
	MonsterSpawn(const IFO::Spawn& ifoSpawn) : spawn(ifoSpawn) {
		this->nextSpawnWave = 0;
	}
	virtual ~MonsterSpawn();

	__inline void addMonster(const word_t typeId) {
		this->addMonster(typeId, 1);
	}
	void addMonster(const word_t typeId, const word_t amount);
	void removeMonster(class Monster* monRef);

	__inline dword_t getCurrentAmount() const {
		return this->spawnedMonsters.size();
	}
};

#endif