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
			word_t unknownValue;
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

			__inline word_t getUnknownValue() const {
				return this->unknownValue;
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
			word_t mobId;
			word_t amount;
			bool tacticalFlag;
		public:
			Round() : Round(String(), 0, 0, false) {}
			Round(const String& name, const word_t id, const word_t count, bool tacticalFlag) {
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
			__inline word_t getMobId() const {
				return this->mobId;
			}
			__inline word_t getAmount() const {
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

		__inline const Round& getRound(const dword_t roundId) const {
			return this->allRounds[roundId];
		}
		__inline size_t getRoundAmount() const {
			return this->allRounds.size();
		}
		__inline dword_t getMaxAmount() const {
			return this->maxAmount;
		}
		__inline dword_t getRespawnInterval() const {
			return this->respawnInterval;
		}
		__inline float getAllowedMaxDistance() const {
			return this->allowedDistance;
		}
		__inline dword_t getTacticalPoints() const {
			return this->tacticalPoints;
		}
	};

	class TelegateSource : public Entry {
	private:
		dword_t id;
	public:
		TelegateSource(BufferedFileReader& bfr) : Entry(bfr) {}
		virtual ~TelegateSource() {}

		__inline dword_t getId() const {
			return this->id;
		}
		__inline void setId(const dword_t newId) {
			this->id = newId;
		}
	};

	class NPC : public Entry{
	private:
		String conFile;
	public:
		NPC(BufferedFileReader& bfr);
		virtual ~NPC();

		__inline word_t getNpcId() const {
			return static_cast<word_t>(this->getObjectId());
		}

		__inline String getConFile() const {
			return this->conFile;
		}
	};
private:
	std::vector<IFO::Spawn> spawns;
	std::vector<IFO::NPC> npcs;
	std::vector<IFO::TelegateSource> telegates;
public:
	const static unsigned long DEFAULT_SECTOR_SIZE = 16000;

	IFO(const String& path, const SharedArrayPtr<char>& data);
	virtual ~IFO();

	std::vector<IFO::Spawn>& getSpawns() {
		return this->spawns;
	}
	std::vector<IFO::NPC>& getNPCs() {
		return this->npcs;
	}
	std::vector<IFO::TelegateSource>& getTelegateSources() {
		return this->telegates;
	}
};

//Rest implemented in: Monster.cpp
class MonsterSpawn {
private:
	typedef IFO::Spawn Spawn;
	Spawn spawn;

	byte_t mapId;
	std::vector<class Monster*> spawnedMonsters;
	time_t nextSpawnWave;
	
public:
	MonsterSpawn(const byte_t idOfMap, const IFO::Spawn& ifoSpawn);
	virtual ~MonsterSpawn();

	bool checkSpawn();

	__inline void addMonster(const word_t typeId, const Position& pos) {
		this->addMonster(typeId, pos, 1);
	}
	void addMonster(const word_t typeId, const Position& pos, const word_t amount);
	void removeMonster(class Monster* monRef);
	
	__inline dword_t getCurrentAmount() const {
		return this->spawnedMonsters.size();
	}

	__inline byte_t getMapId() const {
		return this->mapId;
	}
	__inline const IFO::Spawn& getSpawnData() const {
		return this->spawn;
	}
};

#endif