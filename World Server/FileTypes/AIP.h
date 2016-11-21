#ifndef __ROSE_AIP__
#define __ROSE_AIP__

#ifdef _MSC_VER
#pragma once
#endif

#include "..\..\Common\BasicTypes\StringWrapper.h"
#include "..\..\Common\BasicTypes\SharedArrayPointer.h"
#include "..\..\Common\BasicTypes\FileHeader.h"
#include <vector>

#include "VFS.h"

class AI {
public:
	class InformationTransfer {
	private:
		class Entity* triggerCauser;
		class Entity* designatedTarget;
		class Entity* foundTarget;
		byte_t blockType;
	public:
		InformationTransfer() : InformationTransfer(0) {}
		InformationTransfer(byte_t blockType) : InformationTransfer(0, nullptr) {}
		InformationTransfer(byte_t blockType, Entity* causer) : InformationTransfer(blockType, causer, nullptr) {}
		InformationTransfer(byte_t blockType, Entity* causer, Entity* target) {
			this->blockType = blockType;
			this->triggerCauser = causer;
			this->designatedTarget = target;
			this->foundTarget = nullptr;
		}
		__inline Entity* getTriggerCauser() const {
			return this->triggerCauser;
		}
		__inline Entity* getDesignatedTarget() const {
			return this->designatedTarget;
		}
		__inline Entity* setDesignatedTarget(Entity* entity) {
			this->designatedTarget = entity;
		}
		__inline Entity* getFoundTarget() const {
			return this->foundTarget;
		}
		__inline void setFoundTarget(Entity* entity) {
			this->foundTarget = entity;
		}
		__inline byte_t getBlockType() const {
			return this->blockType;
		}
	};
	class DataSet {
	protected:
		unsigned long length;
		unsigned long opCode;
		SharedArrayPtr<char> data;
		DataSet() {
			length = opCode = 0x00;
			data = SharedArrayPtr<char>();
		}
	public:
		DataSet(const SharedArrayPtr<char>& data) {
			char *dataPointer = data.get();
			this->length = *((dword_t*)dataPointer);
			this->opCode = *((dword_t*)&dataPointer[0x04]);

			const unsigned long headerOffset = 0x08;
			if (this->getLengthWithHeader() > headerOffset) {
				const unsigned long dataLength = length - headerOffset;
				this->data = SharedArrayPtr<char>(new char[dataLength], dataLength);
				memcpy(this->data.get(), &dataPointer[headerOffset], this->data.getSize());
			}
		}
		virtual ~DataSet() {
			std::cout << "Destroying DataSet.\n";
		}
		__inline const char* getData() const {
			return this->data.get();
		}
		__inline unsigned long getLength() const {
			return this->length - 0x08;
		}
		__inline unsigned long getLengthWithHeader() const {
			return this->length;
		}
		__inline unsigned long getOpCode() const {
			return this->opCode;
		}

		__inline String toString() const {
			return String("Length: ") + String::fromInt(this->getLength()) + String(", OpCode: ") + String::fromHex(this->getOpCode());
		}
	};
	class Condition : public DataSet {
	public:
		using ConditionFunctionPtr = bool(*)(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
	private:
		const static unsigned long OPCODE_START = 0x04000001;
	public:
		const static ConditionFunctionPtr ConditionMapping[];

		Condition(const SharedArrayPtr<char>& operationData) : DataSet(operationData) {	}

		virtual ~Condition(){
			std::cout << "Destroying Condition.\n";
		}

		__inline bool isConditionFulfilled(class NPC* entity, const AI::Condition* condition, InformationTransfer* trans) const {
			return AI::Condition::ConditionMapping[condition->getOpCode() - AI::Condition::OPCODE_START](entity, condition, trans);
		}

		static bool FightOrDelay(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool HasEnoughDamageReceived(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool HasEnoughTargets(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckDistanceFromSpawn(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckDistanceToTarget(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckAbilityDifference(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckPercentHP(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckRandomPercentage(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool FindNearestSuitableTarget(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool HasTargetChanged(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckAbility(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool HasEnoughStats(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool HasDaytimeArrived(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool HasBuff(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool IsObjectVarValid(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool IsWorldVarValid(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool IsEconomyValid(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool IsNPCNearby(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckDistanceToOwner(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckZoneTime(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckAreOwnStatsEnough(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool HasNoOwner(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool HasOwner(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckWorldTime(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckWeekDate(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckMonthDate(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckUnknown(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckSurroundingLevelDifference(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckAIVar(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool IsTargetClanMaster(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
		static bool CheckClanCreationTime(class NPC* entity, const AI::Condition* condition, InformationTransfer* transfer);
	};
	class Action : public DataSet {
	public:
		using ActionFunctionPtr = void(*)(class NPC* entity, const Action* action, InformationTransfer* transfer);
	private:
		const static unsigned long OPCODE_START = 0x0B000001;
	public:
		const static ActionFunctionPtr ActionMapping[];

		Action(const SharedArrayPtr<char>& operationData) : DataSet(operationData) {}

		virtual ~Action(){
			std::cout << "Destroying Action.\n";
		}

		__inline void doAction(class NPC* entity, const AI::Action* action, InformationTransfer* trans) const {
			AI::Action::ActionMapping[action->getOpCode() - AI::Action::OPCODE_START](entity, action, trans);
		}

		static void StopAction(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void SetEmote(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void SayBubbledMessage(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void SetRandomPositionFromCurrent(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void SetRandomPositionFromSpawn(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void SetRandomPositionFromTarget(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void AttackTarget(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void SetSpecialAttack(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void MoveToTarget(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void Convert(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void SpawnPet(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void CallAlliesForAttack(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void AttackNearestTarget(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void AttackFoundTarget(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void CallEntireFamilyForAttack(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void AttackDesignatedTarget(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void RunAway(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void DropRandomItem(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void CallFewAlliesForAttack(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void SpawnPetAtPosition(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void KillNPC(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void CastSkill(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void ChangeNPCVar(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void ChangeGlobalVar(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void ChangeEconomyVar(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void SayMessage(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void MoveToOwner(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void SetQuestTrigger(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void AttackOwnersTarget(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void SetMapAsPVPArea(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void GiveItemToOwner(class NPC* entity, const Action* action, InformationTransfer* transfer);
		static void SetAIVar(class NPC* entity, const Action* action, InformationTransfer* transfer);
	};

	static void doRoutine(class NPC*, const byte_t, class AIP*);
};


class AIP {
public:
	class State {
	public:
		class Record {
		private:
			std::vector<AI::Condition*> conditions;
			std::vector<AI::Action*> actions;
		public:
			Record() {}
			virtual ~Record() {
				std::for_each(conditions.begin(), conditions.end(), [](AI::Condition* cond) {
					delete cond;
					cond = nullptr;
				});
				conditions.clear();

				std::for_each(actions.begin(), actions.end(), [](AI::Action* action) {
					delete action;
					action = nullptr;
				});
				actions.clear();
			}

			__inline void addCondition(const SharedArrayPtr<char>& newData) {
				this->conditions.push_back(new AI::Condition(newData));
			}
			__inline void addAction(const SharedArrayPtr<char>& newData) {
				this->actions.push_back(new AI::Action(newData));
			}

			__inline AI::Action* getAction(const word_t id) const {
				return (id < this->actions.size() ? this->actions[id] : nullptr);
			}
			__inline AI::Condition* getCondition(const word_t id) const {
				return (id < this->conditions.size() ? this->conditions[id] : nullptr);
			}
			__inline dword_t getConditionAmount() const {
				return this->conditions.size();
			}
			__inline dword_t getActionAmount() const {
				return this->actions.size();
			}
		};
	private:
		std::vector<AIP::State::Record*> records;
	public:
		State() {}
		virtual ~State() {
			std::for_each(this->records.begin(), this->records.end(), [](AIP::State::Record* record) {
				delete record;
				record = nullptr;
			});
			this->records.clear();
		}

		__inline void addRecord(AIP::State::Record* newRecord) {
			this->records.push_back(std::move(newRecord));
		}
		__inline AIP::State::Record* getRecord(const unsigned long id) const {
			return (id < this->records.size() ? this->records[id] : nullptr);
		}
		__inline unsigned long getRecordAmount() const {
			return static_cast<unsigned long>(this->records.size());
		}
	};
private:
	unsigned long id;
	unsigned long checkInterval;
	unsigned long damageAmountTrigger;
	String filePath;

	std::vector<AIP::State*> states;
public:
	class StateTypes {
		private:
			StateTypes() {}
			~StateTypes() {}
		public:
			const static byte_t SPAWNED = 0x00;
			const static byte_t IDLING = 0x01;
			const static byte_t ATTACKING = 0x02;
			const static byte_t DAMAGE = 0x03;
			const static byte_t ENEMY_DIED = 0x04;
			const static byte_t DEATH = 0x05;
			const static byte_t DEFAULT_STATE_AMOUNT = 0x06;
	};
	AIP() : AIP(nullptr) {}
	AIP(const char *pathInVFS) : AIP(String(pathInVFS)) {}
	AIP(const String& pathInVFS) : AIP(pathInVFS, VFS::get()->getEntry(pathInVFS)) {}
	AIP(const String& pathInVFS, const VFS::Entry& entry) : AIP(pathInVFS, entry.getContent()) {}
	AIP(const String& pathInVFS, const SharedArrayPtr<char>& data) {
		this->filePath = pathInVFS;
		BufferedFileReader bfr(data, data.getSize());
		unsigned long triggerCount = bfr.readDWord();
		if (triggerCount == AIP::StateTypes::DEFAULT_STATE_AMOUNT) {
			this->checkInterval = bfr.readDWord()*1000;
			this->damageAmountTrigger = bfr.readDWord();

			unsigned long tempLen = bfr.readDWord();
			String tempString = bfr.readString(tempLen);

			for (unsigned long i = 0; i < AIP::StateTypes::DEFAULT_STATE_AMOUNT; i++) {
				AIP::State* currentState = new AIP::State();
				bfr.setCaret(bfr.getCaret() + 0x20); //Fixed string size of 32 bytes

				dword_t recordAmount = bfr.readDWord();
				for (unsigned int j = 0; j < recordAmount; j++) {
					bfr.setCaret(bfr.getCaret() + 0x20);
					AIP::State::Record* newRecord = new AIP::State::Record();

					typedef void (AIP::State::Record::*AddSubrecordFunction)(const SharedArrayPtr<char>& newData);
					std::function<void(AddSubrecordFunction)> recordReader = [&](AddSubrecordFunction function) -> void {
						unsigned long amount = bfr.readDWord();
						for (unsigned int y = 0; y < amount; y++) {
							unsigned long length = bfr.readDWord();
							bfr.setCaret(bfr.getCaret() - sizeof(dword_t));
							SharedArrayPtr<char> data = bfr.readBinary(length);
							(newRecord->*function)(data);
						}
					};

					recordReader(&AIP::State::Record::addCondition);
					recordReader(&AIP::State::Record::addAction);

					currentState->addRecord(newRecord);
				}

				this->states.push_back(currentState);
			}
		}
	}
	virtual ~AIP() {
		std::for_each(this->states.begin(), this->states.end(), [](AIP::State* state) {
			delete state;
			state = nullptr;
		});
		this->states.clear();
	}
	__inline unsigned long getCheckInterval() const {
		return this->checkInterval;
	}
	__inline unsigned long getDamageTriggerAmount() const {
		return this->damageAmountTrigger;
	}
	__inline AIP::State* getState(const byte_t state) {
		return (state < AIP::StateTypes::DEFAULT_STATE_AMOUNT ? this->states[state] : nullptr);
	}
};

#endif