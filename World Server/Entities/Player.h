#ifndef __ROSE_PLAYER__
#define __ROSE_PLAYER__

#ifdef _MSC_VER
#pragma once
#endif

#include "..\..\Common\ROSESocketClient.h"
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
private:
	Item items[Inventory::Slots::MAXIMUM];
public:
	Inventory() {
		for (unsigned int i = 0; i < Inventory::Slots::MAXIMUM; i++) {
			this->items[i].clear();
		}
	}
	virtual ~Inventory() {
	}
	__inline Item& get(const byte_t slot) {
		return this->items[slot];
	}
	__inline Item& operator[](const byte_t pos) {
		return this->get(pos);
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
	dword_t experience;
	word_t jobId;
	word_t statPoints;
	word_t skillPoints;
	word_t saveTownId;

	PlayerAppearance appearance;
	Attributes attributes;
	Inventory inventory;

public:
	Character() {}
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
		return this->experience;
	}
	__inline void setExperience(const dword_t exp) {
		this->experience = exp;
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

class Player : public Entity, public ROSESocketClient {
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
	static FunctionBinder<Player, unsigned long, bool(Player::*)()> PACKET_FUNCTIONS;

	Account accountInfo;
	Character character;

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

	bool sendEntityVisuallyAdded(Entity* entity);
	bool sendPlayerVisuallyAdded(Entity* entity, Packet& pak);
	bool sendNPCVisuallyAdded(Entity* entity, Packet& pak);
	bool sendMonsterVisuallyAdded(Entity* entity, Packet& pak);
	bool sendEntityVisuallyRemoved(Entity* entity);

	bool sendNewDestinationVisually();

	bool pakAssignId();
	bool pakIdentify();
	bool pakTeleport();
	bool pakMovement();
	bool pakLocalChat();
	bool pakChangeStance();
public:
	Player(NetworkInterface* IFace, const CryptInfo& cryptInfo);
	virtual ~Player();

	bool handlePacket();
	bool sendTeleport(const byte_t mapId, const Position& pos);

	virtual void updateStats();

	__inline Account* getAccountInfo() {
		return &this->accountInfo;
	}
	__inline Character* getCharacter() {
		return &this->character;
	}
	__inline bool isPlayer() const { return true; }
};

class GMService {
private:
	GMService() {}
	~GMService() {}
public:
	static void teleport(Player* cmdExecutor, SharedArrayPtr<String>& cmdAsTokens);
};

#endif