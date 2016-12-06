#ifndef __ROSE_PLAYER__
#define __ROSE_PLAYER__

#ifdef _MSC_VER
#pragma once
#endif

#include "..\..\Common\ROSESocketClient.h"
#include "..\..\Common\ROSEDummyClient.h"
#include "..\..\Common\BasicTypes\FunctionBinder.h"
#include "Entity.h"

class Account {
private:
	unsigned long id;
	String name;
	unsigned long accessLevel;
public:
	Account() {}
	virtual ~Account() {}

	__inline unsigned long getId() const {
		return this->id;
	}
	__inline void setId(const unsigned long newId) {
		this->id = newId;
	}
	__inline String getName() const {
		return this->name;
	}
	__inline void setName(const String& newName) {
		this->name = newName;
	}
	__inline unsigned long getAccessLevel() const {
		return this->accessLevel;
	}
	__inline void setAccessLevel(const unsigned long newLevel) {
		this->accessLevel = newLevel;
	}
};

class Inventory {
public:
	class Slots {
		private:
			Slots() {}
			~Slots() {}
		public:
			const static byte_t FACE = 1;
			const static byte_t HEADGEAR = 2;
			const static byte_t ARMOR = 3;
			const static byte_t GLOVES = 5;
			const static byte_t BACK = 4;
			const static byte_t SHOES = 6;
			const static byte_t WEAPON = 7;
			const static byte_t SHIELD = 8;

			const static byte_t ARROWS = 132;
			const static byte_t BULLETS = 133;
			const static byte_t CANNONSHELLS = 134;
			const static byte_t CART_FRAME = 135;
			const static byte_t CART_ENGINE = 136;
			const static byte_t CART_WHEELS = 137;
			const static byte_t CART_WEAPON = 138;
			const static byte_t CART_ABILITY = 139;

			const static byte_t TAB_SIZE = 30;
			const static byte_t MAXIMUM = 140;
	};
	const static word_t MAXIMUM_STACKABLE = 999;
private:
	byte_t slotAmount;
	Item* items;
public:
	Inventory() : Inventory(Inventory::Slots::MAXIMUM) {
	}
	explicit Inventory(const byte_t slotAmount) {
		this->slotAmount = slotAmount;
		this->items = new Item[this->slotAmount];
		for (unsigned int i = 0; i < this->slotAmount; i++) {
			this->items[i].clear();
		}
	}
	virtual ~Inventory() {
		delete[] this->items;
		this->items = nullptr;
	}

	byte_t getSuitableSlot(const Item& item) {
		if (item.isStackable()) {
			for (byte_t i = 0; i < this->slotAmount; i++) {
				if (item.getType() == this->items[i].getType() &&
					item.getId() == this->items[i].getId() &&
					item.getAmount() + this->items[i].getAmount() <= MAXIMUM_STACKABLE) {
					return i;
				}
			}
		}			
		for (byte_t i = 0; i < this->slotAmount; i++) {
			if (!this->items[i].isValid()) {
				return i;
			}
		}
		return (std::numeric_limits<byte_t>::max)();
	}

	bool addItem(const Item& i) {
		const byte_t slot = this->getSuitableSlot(i);
		if (slot < this->slotAmount) {
			this->items[slot] = Item(i.getType(), i.getId(), i.getAmount() + this->items[slot].getAmount());
		}
		return slot < this->slotAmount;
	}
	
	__inline Item& get(const byte_t slot) {
		return this->items[slot];
	}
	__inline Item& operator[](const byte_t pos) {
		return this->get(pos);
	}
	__inline byte_t getSlotAmount() const {
		return this->slotAmount;
	}
};

class PlayerAppearance {
private:
	byte_t sex;
	word_t hairStyle;
	word_t faceStyle;
public:
	PlayerAppearance() : PlayerAppearance(0, 0, 0) {}
	PlayerAppearance(const word_t hair, const word_t face, const byte_t sex) {
		this->setSex(sex);
		this->setHairStyle(hair);
		this->setFaceStyle(face);
	}
	virtual ~PlayerAppearance() {}

	PlayerAppearance& operator=(const PlayerAppearance& pa) = default;

	__inline byte_t getSex() const {
		return this->sex;
	}
	__inline void setSex(const byte_t newSex) {
		this->sex = newSex;
	}
	__inline word_t getHairStyle() const {
		return this->hairStyle;
	}
	__inline void setHairStyle(const word_t hair) {
		this->hairStyle = hair;
	}
	__inline word_t getFaceStyle() const {
		return this->faceStyle;
	}
	__inline void setFaceStyle(const word_t face) {
		this->faceStyle = face;
	}
};

class Attributes {
private:
	word_t skilledStr;
	word_t externalStr; //from clothes etc.

	word_t skilledDex;
	word_t externalDex; //from clothes etc.

	word_t skilledInt;
	word_t externalInt; //from clothes etc.

	word_t skilledCon;
	word_t externalCon; //from clothes etc.

	word_t skilledCha;
	word_t externalCha; //from clothes etc.

	word_t skilledSen;
	word_t externalSen; //from clothes etc.

public:
	Attributes() : Attributes(15, 15, 15, 15, 10, 10) {}
	Attributes(const word_t str, const word_t dex, const word_t Int, const word_t con, const word_t cha, const word_t sen) {
		this->setStrength(str);
		this->externalCha = this->externalCon = this->externalDex = this->externalInt = this->externalSen = this->externalStr = 0;
		this->setDexterity(dex);
		this->setIntelligence(Int);
		this->setConcentration(con);
		this->setCharm(cha);
		this->setSensibility(sen);
	}

	__inline word_t getSkilledStrength() const {
		return this->skilledStr;
	}
	__inline word_t getAdditionalStrength() const {
		return this->externalStr;
	}
	__inline word_t getTotalStrength() const {
		return this->getSkilledStrength() + this->getAdditionalStrength();
	}
	__inline void setStrength(const word_t val) {
		this->skilledStr = val;
	}
	__inline void setAdditionalStrength(const word_t amount) {
		this->externalStr = amount;
	}

	__inline word_t getSkilledDexterity() const {
		return this->skilledDex;
	}
	__inline word_t getAdditionalDexterity() const {
		return this->externalDex;
	}
	__inline word_t getTotalDexterity() const {
		return this->getSkilledDexterity() + this->getAdditionalDexterity();
	}
	__inline void setDexterity(const word_t val) {
		this->skilledDex = val;
	}
	__inline void setAdditionalDexterity(const word_t amount) {
		this->externalDex = amount;
	}

	__inline word_t getSkilledIntelligence() const {
		return this->skilledInt;
	}
	__inline word_t getAdditionalIntelligence() const {
		return this->externalInt;
	}
	__inline word_t getTotalIntelligence() const {
		return this->getSkilledIntelligence() + this->getAdditionalIntelligence();
	}
	__inline void setIntelligence(const word_t val) {
		this->skilledInt = val;
	}
	__inline void setAdditionalIntelligence(const word_t amount) {
		this->externalInt = amount;
	}

	__inline word_t getSkilledConcentration() const {
		return this->skilledCon;
	}
	__inline word_t getAdditionalConcentration() const {
		return this->externalCon;
	}
	__inline word_t getTotalConcentration() const {
		return this->getSkilledConcentration() + this->getAdditionalConcentration();
	}
	__inline void setConcentration(const word_t val) {
		this->skilledCon = val;
	}
	__inline void setAdditionalConcentration(const word_t amount) {
		this->externalCon = amount;
	}

	__inline word_t getSkilledCharm() const {
		return this->skilledCha;
	}
	__inline word_t getAdditionalCharm() const {
		return this->externalCha;
	}
	__inline word_t getTotalCharm() const {
		return this->getSkilledCharm() + this->getAdditionalCharm();
	}
	__inline void setCharm(const word_t val) {
		this->skilledCha = val;
	}
	__inline void setAdditionalCharm(const word_t amount) {
		this->externalCha = amount;
	}

	__inline word_t getSkilledSensibility() const {
		return this->skilledSen;
	}
	__inline word_t getAdditionalSensibility() const {
		return this->externalSen;
	}
	__inline word_t getTotalSensibility() const {
		return this->getSkilledSensibility() + this->getAdditionalSensibility();
	}
	__inline void setSensibility(const word_t val) {
		this->skilledSen = val;
	}
	__inline void setAdditionalSensibility(const word_t amount) {
		this->externalSen = amount;
	}
};

class Character {
private:
	String name;
	dword_t id;
	Observable<dword_t> experience;
	word_t jobId;
	word_t statPoints;
	word_t skillPoints;
	word_t saveTownId;

	PlayerAppearance appearance;
	Attributes attributes;
	Inventory inventory;

public:
	Character() {
		experience.setTriggerListenerOnce(true);
	}
	virtual ~Character() {}

	__inline dword_t getId() const {
		return this->id;
	}
	__inline void setId(const dword_t id) {
		this->id = id;
	}
	__inline String getName() const {
		return this->name;
	}
	__inline void setName(const String& newName) {
		this->name = newName;
	}
	__inline dword_t getExperience() const {
		return this->experience.getValue();
	}
	__inline void setExperience(const dword_t exp) {
		this->experience = exp;
	}
	__inline void addExperience(const dword_t addExp) {
		this->setExperience(this->getExperience() + addExp);
	}
	__inline void setOnExperienceAdded(std::function<void(dword_t)> f)  {
		this->experience.setOnNewValueAssigned(f == nullptr ? [](dword_t){} : f);
	}
	__inline word_t getJobId() const {
		return this->jobId;
	}
	__inline void setJobId(const word_t newJob) {
		this->jobId = newJob;
	}
	__inline word_t getStatPoints() const {
		return this->statPoints;
	}
	__inline void setStatPoints(const word_t points) {
		this->statPoints = points;
	}
	__inline word_t getSkillPoints() const {
		return this->skillPoints;
	}
	__inline void setSkillPoints(const word_t points) {
		this->skillPoints = points;
	}
	__inline void setSaveTown(const word_t id) {
		this->saveTownId = id;
	}
	__inline word_t getSaveTown() const {
		return this->saveTownId;
	}

	__inline Inventory* getInventory() {
		return &this->inventory;
	}
	__inline PlayerAppearance* getAppearance() {
		return &this->appearance;
	}
	__inline Attributes* getAttributes() {
		return &this->attributes;
	}
};


class QuestJournal {
public:
	const static byte_t MAXIMUM_EPISODE_VARS = 5;
	const static byte_t MAXIMUM_JOB_VARS = 3;
	const static byte_t MAXIMUM_PLANET_VARS = 7;
	const static byte_t MAXIMUM_UNION_VARS = 10;
	const static byte_t MAXIMUM_QUESTS = 10;

	class Entry {
	public:
		const static byte_t MAX_QUEST_VARS = 10;
		const static byte_t MAX_QUEST_ITEMS = 6;
	private:
		const static word_t SWITCHES_AS_BITS = 0x20;
		const static word_t MAX_SWITCHES = (SWITCHES_AS_BITS / (sizeof(byte_t)*8));

		word_t vars[MAX_QUEST_VARS];
		Inventory* questInventory;
		word_t questId;
		dword_t startTime;
		byte_t lever[MAX_SWITCHES];
	public:
		Entry() : Entry(0) {}
		Entry(const word_t questId) {
			this->questInventory = new Inventory(MAX_QUEST_ITEMS);
			this->reset();
			this->questId = questId;
		}
		virtual ~Entry() {
			delete this->questInventory;
			this->questInventory = nullptr;
		}
		void reset() {
			reset(0);
		}
		void reset(word_t newQuestId) {
			this->questId = newQuestId;
			this->startTime = 0x00;
			for (unsigned int i = 0; i < MAX_SWITCHES; i++) {
				this->lever[i] = 0x00;
			}
			for (unsigned int i = 0; i < MAX_QUEST_VARS; i++) {
				this->vars[i] = 0x00;
			}
			for (unsigned int i = 0; i < MAX_QUEST_ITEMS; i++) {
				this->questInventory->get(i).clear();
			}
		}
		__inline const word_t getQuestId() const {
			return this->questId;
		}
		__inline void replaceQuestId(const word_t newQuestId) {
			this->questId = newQuestId;
		}
		__inline const Item& getItem(const byte_t slotId) const {
			return this->questInventory->get(slotId);
		}
		__inline const word_t getQuestVariable(const word_t slotId) const {
			return this->vars[slotId];
		}
		__inline void setQuestVariable(const byte_t slotId, const word_t value) {
			this->vars[slotId] = value;
		}
		__inline dword_t getPassedTime() const {
			return this->startTime;
		}

		__inline dword_t getLeverData() const {
			return lever[0] | (lever[1] << 8) || (lever[2] << 16) | (lever[3] << 24);
		}

		__inline byte_t getLever(const word_t slot) const {
			return (this->lever[slot >> 3] & (1 << (slot & 0x07)));
		}
		__inline void setLever(const word_t slot) {
			(this->lever[slot >> 3] |= (1 << (slot & 0x07)));
		}
		__inline void clearLever(const word_t slot) {
			(this->lever[slot >> 3] &= ~(1 << (slot & 0x07)));
		}
		void flipLever(const word_t slot) {
			if (this->getLever(slot)) {
				this->clearLever(slot);
			}
			else {
				this->setLever(slot);
			}
		}
	};
private:
	Entry* currentlySelectedQuest;
	Entry* journal[MAXIMUM_QUESTS];

	word_t episodeVars[MAXIMUM_EPISODE_VARS];
	word_t jobVars[MAXIMUM_JOB_VARS];
	word_t planetVars[MAXIMUM_PLANET_VARS];
	word_t unionVars[MAXIMUM_UNION_VARS];

	byte_t globalFlag[0x40];
public:
	QuestJournal() {
		currentlySelectedQuest = nullptr;
		for (unsigned int i = 0; i < MAXIMUM_QUESTS; i++) {
			journal[i] = new Entry();
		}
	}
	virtual ~QuestJournal() {
		for (unsigned int i = 0; i < MAXIMUM_QUESTS; i++) {
			delete journal[i];
			journal[i] = nullptr;
		}
	}

	Entry* getQuestEntry(const word_t questId) const {
		for (unsigned int i = 0; i < MAXIMUM_QUESTS; i++) {
			if (this->journal[i]->getQuestId() == questId) {
				return this->journal[i];
			}
		}
		return nullptr;
	}

	Entry* getEmptyEntry() const {
		for (unsigned int i = 0; i < MAXIMUM_QUESTS; i++) {
			if (this->journal[i]->getQuestId() == 0x00) {
				return this->journal[i];
			}
		}
		return nullptr;
	}

	__inline bool selectQuest(const word_t questId) {
		return this->selectQuest(questId, false);
	}
	bool selectQuest(const word_t questId, bool dryRun) {
		auto entry = getQuestEntry(questId);
		if (entry != nullptr && !dryRun) {
			this->currentlySelectedQuest = entry;
		}
		return entry != nullptr;
	}

	__inline Entry* getQuestSlot(const byte_t slot) const {
		return this->journal[slot];
	}

	__inline Entry* getCurrentlySelectedQuest() const {
		return this->currentlySelectedQuest;
	}

	__inline word_t getEpisodeVar(const word_t slot) const {
		return this->episodeVars[slot];
	}
	__inline void setEpisodeVar(const word_t slot, const word_t value) {
		this->episodeVars[slot] = value;
	}
	__inline word_t getJobVar(const word_t slot) const {
		return this->jobVars[slot];
	}
	__inline void setJobVar(const word_t slot, const word_t value) {
		this->jobVars[slot] = value;
	}
	__inline word_t getPlanetVar(const word_t slot) const {
		return this->planetVars[slot];
	}
	__inline void setPlanetVar(const word_t slot, const word_t value) {
		this->planetVars[slot] = value;
	}
	__inline word_t getUnionVar(const word_t slot) const {
		return this->unionVars[slot];
	}
	__inline void setUnionVar(const word_t slot, const word_t value) {
		this->unionVars[slot] = value;
	}

	word_t getVarByType(const word_t type, const word_t varId) const {
		byte_t realType = (type >> 8);
		switch (realType) {
			case 0x00:
				if (this->getCurrentlySelectedQuest() != nullptr) {
					return this->getCurrentlySelectedQuest()->getQuestVariable(varId);
				}
			break;
			case 0x01:
				if (this->getCurrentlySelectedQuest() != nullptr) {
					return this->getCurrentlySelectedQuest()->getLever(varId);
				}
			break;
			case 0x02:
				//TODO: Time
			break;
			case 0x03:
				return this->getEpisodeVar(varId);
			break;
			case 0x04:
				return this->getJobVar(varId);
			break;
			case 0x05:
				return this->getPlanetVar(varId);
			break;
			case 0x06:
				return this->getUnionVar(varId);
			break;
		}
		return 0x00;
	}

	void setVarByType(const word_t type, const byte_t varId, const word_t value) {
		byte_t realType = (type >> 8);
		switch (realType) {
			case 0x00:
				if (this->getCurrentlySelectedQuest() != nullptr) {
					this->getCurrentlySelectedQuest()->setQuestVariable(varId, value);
				}
			break;
			case 0x01:
				if (this->getCurrentlySelectedQuest() != nullptr) {
					if (value > 0) {
						this->getCurrentlySelectedQuest()->setLever(varId);
					}
					else {
						this->getCurrentlySelectedQuest()->clearLever(varId);
					}
				}
			break;
			case 0x02:
				//TODO: Time
			break;
			case 0x03:
				this->setEpisodeVar(varId, value);
			break;
			case 0x04:
				this->setJobVar(varId, value);
			break;
			case 0x05:
				this->setPlanetVar(varId, value);
			break;
			case 0x06:
				this->setUnionVar(varId, value);
			break;
		}
	}

	template<class _T = byte_t>
	__inline _T getGlobalFlag(const byte_t slot) const {
		return ((_T*)this->globalFlag)[slot];
	}
};

class Player : public Entity {
public:
	class AbilityTypes {
	private:
		AbilityTypes() {}
		~AbilityTypes() {}
	public:
		const static byte_t BIRTHSTONE = 0x03;
		const static byte_t JOB = 0x04;
		const static byte_t FACTION = 0x05;
		const static byte_t RANKING = 0x06;
		const static byte_t REPUTATION = 0x07;
		const static byte_t FACE = 0x08;
		const static byte_t HAIR = 0x09;
		const static byte_t STRENGTH = 0x0A;
		const static byte_t DEXTERITY = 0x0B;
		const static byte_t INTELLIGENCE = 0x0C;
		const static byte_t CONCENTRATION = 0x0D;
		const static byte_t CHARM = 0x0E;
		const static byte_t SENSIBILITY = 0x0F;
		const static byte_t CURRENT_HP = 0x10;
		const static byte_t CURRENT_MP = 0x11;
		const static byte_t ATTACK_POWER = 0x12;
		const static byte_t PHYISCAL_DEFENSE = 0x13;
		const static byte_t ACCURACY = 0x14;
		const static byte_t MAGICAL_DEFENSE = 0x15;
		const static byte_t DODGE_RATE = 0x16;
		const static byte_t MOVEMENT_SPEED = 0x17;
		const static byte_t ATTACK_SPEED = 0x18;
		const static byte_t INVENTORY_CAPACITY = 0x19;
		const static byte_t CRITICAL_RATE = 0x1A;
		const static byte_t HP_RECOVERY = 0x1B;
		const static byte_t MP_RECOVERY = 0x1C;
		const static byte_t MP_CONSUMPTION = 0x1D;
		const static byte_t EXP_RATE = 0x1E;
		const static byte_t LEVEL = 0x1F;
		const static byte_t POINT = 0x20;
		const static byte_t TENDENCY = 0x21;
		const static byte_t PK_LEVEL = 0x22;
		const static byte_t HEAD_SIZE = 0x23;
		const static byte_t BODY_SIZE = 0x24;
		const static byte_t SKILL_POINTS = 0x25;
		const static byte_t MAX_HP = 0x26;
		const static byte_t MAX_MP = 0x27;
		const static byte_t MONEY = 0x28;
		const static byte_t UNARMED_ATTACKPOWER = 0x29;
		const static byte_t ONEHANDED_ATTACKPOWER = 0x2A;
		const static byte_t TWOHANDED_ATTACKPOWER = 0x2B;
		const static byte_t BOW_ATTACKPOWER = 0x2C;
		const static byte_t GUN_ATTACKPOWER = 0x2D;
		const static byte_t MAGICWEAPONG_ATTACKPOWER = 0x2E;
		const static byte_t CROSSBOW_ATTACKPOWER = 0x2F;
		const static byte_t COMBATWEAPON_ATTACKPOWER = 0x30;
		const static byte_t BOW_ATTACKSPEED = 0x31;
		const static byte_t GUN_ATTACKSPEED = 0x32;
		const static byte_t COMBATWEAPON_ATTACKSPEED = 0x33;
		const static byte_t MOVEMENT_SPEED_OTHER = 0x34;
		const static byte_t PHYSICAL_DEFENSE_OTHER = 0x35;
		const static byte_t MAX_HP_OTHER = 0x36;
		const static byte_t MAX_MP_OTHER = 0x37;
		const static byte_t HP_RECOVERY_OTHER = 0x38;
		const static byte_t MP_RECOVERY_OTHER = 0x39;
		const static byte_t BAGPACK_CAPACITY = 0x3A;
		const static byte_t SALES_DISCOUNT = 0x3B;
		const static byte_t SALES_PREMIUM = 0x3C;
		const static byte_t MP_COST_REDUCTION = 0x3D;
		const static byte_t SUMMON_GAUGE_INCREASE = 0x3E;
		const static byte_t ITEM_DROP_RATE = 0x3F;

		const static byte_t PLANET_REQUIREMENT = 0x4B;
		const static byte_t STAMINA = 0x4C;

		const static byte_t NO_STORAGE_CHARGE = 0x5E;
		const static byte_t STORAGE_EXPANSION = 0x5F;
		const static byte_t PERSONAL_SHOP_REMODELLING = 0x60;
		const static byte_t CART_GAUGE = 0x61;
	};
private:
	class Debugging {
		private:
			bool printTargetPosition;
			bool printOwnPosition;
		public:
			Debugging() {
				printOwnPosition = printTargetPosition = false;
			}
			__inline bool getPrintTargetPosition() const {
				return this->printTargetPosition;
			}
			__inline void setPrintTargetPosition(bool flag) {
				this->printTargetPosition = flag;
			}
			__inline bool getPrintOwnPosition() const {
				return this->printOwnPosition;
			}
			__inline void setPrintOwnPosition(bool flag) {
				this->printOwnPosition = flag;
			}
	};
	Debugging debuggingFlags;
	static FunctionBinder<Player, unsigned long, bool(Player::*)(const Packet&)> PACKET_FUNCTIONS;

	Account *accountInfo;
	Character *character;
	Attributes *attributes;
	QuestJournal* questJournal;

	std::mutex socketMutex;
	ROSESocketClient* networkInterface;

	dword_t getExpForLevelUp() const;
	void setAttackMotion();

	void updateAttributes();

	void updateStrength();
	void updateDexterity();
	void updateIntelligence();
	void updateConcentration();
	void updateCharm();
	void updateSensibility();

	void updateAttackPower();
	void updateMaxHP();
	void updateMaxMP();
	void updatePhysicalDefense();
	void updateMagicalDefense();
	void updateMovementSpeed();
	void updateAttackSpeed();
	void updateAttackRange();

	bool loadEntireCharacter();
	bool loadCharacterInformation();
	bool loadCharacterStats();
	bool loadCharacterInventory();
	bool loadCharacterSkills();

	bool sendEncryption();
	bool sendGamingPlan();
	bool sendPlayerInformation();
	bool sendQuestData();
	bool sendInventory();
	bool sendWeightPercentage();
	bool sendTeleport();
	bool sendBasicAttack();
	bool sendExperienceUpdate();
	bool sendLevelUp();

	bool sendEntityVisuallyAdded(Entity* entity);
	bool sendPlayerVisuallyAdded(Entity* entity, Packet& pak);
	bool sendNPCVisuallyAdded(Entity* entity, Packet& pak);
	bool sendMonsterVisuallyAdded(Entity* entity, Packet& pak);
	bool sendEntityVisuallyRemoved(Entity* entity);
	bool sendWhisper(Entity* entity, const String& msg);
	bool sendWhisper(const String& name, const String& msg);
	bool sendMonsterHP(Entity* mon);

	bool sendNewDestinationVisually();

	bool pakAssignId(const Packet& pak);
	bool pakIdentify(const Packet& pak);
	bool pakTeleport(const Packet& pak);
	bool pakMovement(const Packet& pak);
	bool pakLocalChat(const Packet& pak);
	bool pakChangeStance(const Packet& pak);
	bool pakInitBasicAttack(const Packet& pak);
public:
	Player(NetworkInterface* IFace, const CryptInfo& cryptInfo);
	virtual ~Player();

	bool handlePacket(const Packet& pak);
	bool sendTeleport(const byte_t mapId, const Position& pos);
	bool sendDebugMessage(const String& msg);

	void doAction();
	virtual void updateStats();

	bool sendPacket(const Packet& pak);

	__inline Account* getAccountInfo() const {
		return this->accountInfo;
	}
	__inline Character* getCharacter() const {
		return this->character;
	}

	__inline Attributes* getAttributes() const {
		return this->attributes;
	}
	__inline QuestJournal* getQuestJournal() const {
		return this->questJournal;
	}

	__inline Debugging& getDebuggingFlags() {
		return this->debuggingFlags;
	}

	__inline NetworkClient* getNetworkInterface() const {
		return this->networkInterface;
	}

	__inline bool isPlayer() const { return true; }

	__inline bool isActive() const {
		return this->getNetworkInterface() != nullptr && dynamic_cast<ROSEDummyClient*>(this->getNetworkInterface()) == nullptr;
	}

	__inline void invalidateNetworkInterface() {
		this->networkInterface = new ROSEDummyClient();
	}
	void onDisconnect();
};

class GMService {
private:
	GMService() {}
	~GMService() {}
public:
	static void teleport(Player* cmdExecutor, SharedArrayPtr<String>& cmdAsTokens);
	static void currentPosition(Player* cmdExecutor, SharedArrayPtr<String>& cmdAsTokens);
	static void setExp(Player* cmdExecutor, SharedArrayPtr<String>& cmdAsTokens);
	static void setLevel(Player* cmdExecutor, SharedArrayPtr<String>& cmdAsTokens);
	static void targetInformation(Player* cmdExecutor, SharedArrayPtr<String>& cmdAsTokens);
};

#endif