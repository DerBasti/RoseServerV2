#ifndef __ROSE_QSD__
#define __ROSE_QSD__

#ifdef _MSC_VER
#pragma once
#endif

#include "VFS.h"
#include "..\Entities\Player.h"

class QuestInformationTransfer {
private:
	Entity* triggerCauser;
	Entity* triggerTarget;
	QuestJournal::Entry* selectedQuest;
	bool questEstablishedDuringDryRun;
public:
	QuestInformationTransfer() : QuestInformationTransfer(nullptr) {}
	explicit QuestInformationTransfer(Entity* triggerCauser) {
		this->triggerCauser = triggerCauser;
		questEstablishedDuringDryRun = false;
	}
	__inline Entity* getTriggerCauser() const {
		return this->triggerCauser;
	}
	__inline void setTriggerTarget(Entity* entity) {
		this->triggerTarget = entity;
	}
	__inline Entity* getTriggerTarget() const {
		return this->triggerTarget;
	}
	__inline QuestJournal::Entry* getSelectedQuest() const {
		return this->selectedQuest;
	}
	__inline void setSelectedQuest(QuestJournal::Entry* entry) {
		this->selectedQuest = entry;
	}
	__inline void setDryRunQuestFlag() {
		questEstablishedDuringDryRun = true;
	}
	__inline bool isDryRunQuestFlagSet() const {
		return questEstablishedDuringDryRun;
	}
};

class QSD {
public:
	class DataSet {
		protected:
			static const unsigned int DEFAULT_HEADER_OFFSET = 0x08;
			unsigned long length;
			unsigned long opCode;
			SharedArrayPtr<char> data;

			struct AbilityCheck {
				dword_t abilityType;
				dword_t amount;
				byte_t operation;
			};

			static AbilityCheck GetAbilityData(DataInterpreter<char>& reader) {
				AbilityCheck ac;
				ac.abilityType = reader.get<dword_t>();
				ac.amount = reader.get<dword_t>();
				ac.operation = reader.get<byte_t>();
				return ac;
			}
			struct QuestVarCheck {
				word_t varNum;
				word_t varType;
				word_t amount;
				byte_t operation;
			};

			static QuestVarCheck GetQuestVarData(DataInterpreter<char>& reader) {
				QuestVarCheck qvc;
				qvc.varNum = reader.get<word_t>();
				qvc.varType = reader.get<word_t>();
				qvc.amount = reader.get<word_t>();
				qvc.operation = reader.get<byte_t>();
				return qvc;
			}
		public:
			DataSet(const SharedArrayPtr<char>& d) {
				this->length = *((dword_t*)d.get());
				this->opCode = *((dword_t*)d.get() + sizeof(dword_t));

				const unsigned int dataLength = this->length - DEFAULT_HEADER_OFFSET;
				if (dataLength > 0) {
					this->data = SharedArrayPtr<char>(new char[dataLength], dataLength);
					memcpy(this->data.get(), &d.get()[DEFAULT_HEADER_OFFSET], dataLength);
				}
			}
			virtual ~DataSet() {

			}

			__inline unsigned long getOpCode() const {
				return this->opCode;
			}
			__inline unsigned long getLength() const {
				return this->length - DEFAULT_HEADER_OFFSET;
			}
			__inline unsigned long getTotalLength() const {
				return this->length;
			}
			__inline char* getData() const {
				return this->data.get();
			}
	};
	class Condition : public DataSet {
		public:
			using ConditionFunctionPtr = bool(*)(const QSD::Condition* condition, QuestInformationTransfer* qit);
		private:
			const static unsigned long OPCODE_START = 0x04000001;
			const static ConditionFunctionPtr ConditionMapping[];
		public:
			Condition(const SharedArrayPtr<char>& d) : DataSet(d) {

			}
			__inline bool isConditionFulfilled(QuestInformationTransfer* qit) const {
				return (*ConditionMapping[this->getOpCode()])(this, qit);
			}

			
			static bool selectQuestId(const QSD::Condition* condition, QuestInformationTransfer* qit);
			static bool checkQuestVar(const QSD::Condition* condition, QuestInformationTransfer* qit);
	};
	class Reward : public DataSet {
		public:
			using RewardFunctionPtr = bool(*)(const QSD::Reward* reward, QuestInformationTransfer* qit, bool dryRun);
		private:
			const static unsigned long OPCODE_START = 0x04000001;
			const static RewardFunctionPtr RewardMapping[];
		public:
			Reward(const SharedArrayPtr<char>& d) : DataSet(d) {

			}
			__inline bool applyReward(QuestInformationTransfer* qit, bool applyRewardFlag) {
				return (*RewardMapping[this->getOpCode()])(this, qit, applyRewardFlag);
			}

			static bool rewardNewQuest(const QSD::Reward* reward, QuestInformationTransfer* qit, bool dryRun);
			static bool rewardQuestItem(const QSD::Reward* reward, QuestInformationTransfer* qit, bool dryRun);

	};
	class Record {
	private:
		Record* previous;
		Record* next;
		String questName;
		dword_t questHash;

		bool checkNextTrigger;

		Condition** conditions;
		dword_t condAmount;

		Reward** rewards;
		dword_t rewardAmount;
	public:
		Record(const String& questName, Condition** cond, const dword_t cAmount, Reward** rewards, const dword_t rAmount);

		virtual ~Record() {
			for (unsigned int i = 0; i < condAmount; i++) {
				delete this->conditions[i];
				this->conditions[i] = nullptr;
			}
			delete[] conditions;
			for (unsigned int i = 0; i < rewardAmount; i++) {
				delete this->rewards[i];
				this->rewards[i] = nullptr;
			}
			delete[] rewards;
		}
		__inline QSD::Record* getNextRecord() const {
			return this->next;
		}
		__inline void setNextRecord(QSD::Record* next) {
			this->next = next;
		}
		__inline QSD::Record* getPreviousRecord() const {
			return this->previous;
		}
		__inline void setPreviousRecord(QSD::Record* record) {
			this->previous = record;
		}
		__inline Condition** getConditions() const {
			return this->conditions;
		}
		__inline dword_t getConditionAmount() const {
			return this->condAmount;
		}

		__inline Reward** getRewards() const {
			return this->rewards;
		}
		__inline dword_t getRewardAmount() const {
			return this->rewardAmount;
		}

		__inline dword_t getQuestHash() const {
			return this->questHash;
		}
		
		__inline bool isNextTriggerRequired() const {
			return this->checkNextTrigger;
		}
		__inline void setCheckNextTrigger(bool flag) {
			this->checkNextTrigger = flag;
		}
	};
	class Entry {
	private:
		QSD* parent;
		Record** records;
		dword_t recordAmount;
	public:
		Entry() {
			parent = nullptr;
			records = nullptr;
			recordAmount = 0x00;
		}

		Entry(QSD* parentQsd, const dword_t amount) {
			this->parent = parentQsd;
			this->recordAmount = amount;
			this->records = new Record*[recordAmount];
		}

		virtual ~Entry() {
			for (unsigned int i = 0; i < this->getRecordAmount(); i++) {
				delete this->records[i];
				this->records[i] = nullptr;
			}
			delete[] records;
		}

		__inline Record** getRecords() const {
			return this->records;
		}
		__inline dword_t getRecordAmount() const {
			return this->recordAmount;
		}
	};
private:
	String name;
	word_t id;
	Entry** entries;
	dword_t entryAmount;

	void setConditions(QSD::Record* rec, Condition** cond, const dword_t conditionAmount);
	void setRewards(QSD::Record* rec, Reward** rewards, const dword_t rewardAmount);

	static dword_t createQuestHash(const char* questName);
public:
	QSD(const word_t id, const String& path, VFS::Entry& entry);
	QSD(const word_t id, const String& path, const SharedArrayPtr<char>& data);

	virtual ~QSD();

	__inline dword_t getQSDId() const {
		return this->id;
	}
	__inline const String& getName() const {
		return this->name;
	}
	__inline QSD::Entry** getQuestEntries() const {
		return this->entries;
	}
	__inline dword_t getQuestEntryAmount() const {
		return this->entryAmount;
	}
	static bool fireTrigger(const dword_t questHash, Entity* triggerCauser);
};

#endif