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
			const static word_t FACE = 1;
			const static word_t HEADGEAR = 2;
			const static word_t ARMOR = 3;
			const static word_t GLOVES = 5;
			const static word_t BACK = 4;
			const static word_t SHOES = 6;
			const static word_t WEAPON = 7;
			const static word_t SHIELD = 8;

			const static word_t ARROWS = 132;
			const static word_t BULLETS = 133;
			const static word_t CANNONSHELLS = 134;
			const static word_t CART_FRAME = 135;
			const static word_t CART_ENGINE = 136;
			const static word_t CART_WHEELS = 137;
			const static word_t CART_WEAPON = 138;
			const static word_t CART_ABILITY = 139;

			const static word_t TAB_SIZE = 30;
			const static word_t MAXIMUM = 140;
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
		for (unsigned int i = 0; i < Inventory::Slots::MAXIMUM; i++) {
			this->items[i].clear();
		}
	}
	__inline Item& get(const byte_t slot) {
		return this->items[slot];
	}
	__inline Item& operator[](unsigned long pos) {
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
	word_t level;
	word_t jobId;
	word_t statPoints;
	word_t skillPoints;

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
	__inline word_t getLevel() const {
		return this->level;
	}
	__inline void setLevel(const word_t level) {
		this->level = level;
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
private:
	static FunctionBinder<Player, unsigned long, bool(Player::*)()> PACKET_FUNCTIONS;

	Account accountInfo;
	Character character;

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

	bool pakAssignId();
	bool pakIdentify();
	bool pakTeleport();
public:
	Player(NetworkInterface* IFace, const CryptInfo& cryptInfo);
	virtual ~Player();

	bool handlePacket();

	__inline Account* getAccountInfo() {
		return &this->accountInfo;
	}
	__inline Character* getCharacter() {
		return &this->character;
	}
};

#endif