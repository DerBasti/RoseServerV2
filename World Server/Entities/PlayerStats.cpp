#include "Player.h"
#include "..\WorldServer.h"

void Player::updateStats() {
	this->updateAttributes();
	Entity::updateStats();
}

void Player::updateAttributes() {
	this->updateStrength();
	this->updateDexterity();
	this->updateIntelligence();
	this->updateConcentration();
	this->updateCharm();
	this->updateSensibility();
}

void Player::updateStrength() {
	word_t additionalStrength = 0x00;
	this->getCharacter()->getAttributes()->setAdditionalStrength(additionalStrength);
}

void Player::updateDexterity() {
	word_t additionalDexterity = 0x00;
	this->getCharacter()->getAttributes()->setAdditionalDexterity(additionalDexterity);
}

void Player::updateIntelligence() {
	word_t additionalIntelligence = 0x00;
	this->getCharacter()->getAttributes()->setAdditionalDexterity(additionalIntelligence);
}

void Player::updateConcentration() {
	word_t additionalConcentration = 0x00;
	this->getCharacter()->getAttributes()->setAdditionalDexterity(additionalConcentration);
}

void Player::updateCharm() {
	word_t additionalCharm = 0x00;
	this->getCharacter()->getAttributes()->setAdditionalDexterity(additionalCharm);
}

void Player::updateSensibility() {
	word_t additionalSensibility = 0x00;
	this->getCharacter()->getAttributes()->setAdditionalSensibility(additionalSensibility);
}


void Player::updateAttackPower() {
	word_t atkPower = 15;
	this->getStats()->setAttackPower(atkPower);
}

void Player::updateMaxHP() {
	word_t maxHP = 100;
	this->getStats()->setMaxHP(maxHP);
}

void Player::updateMaxMP() {
	word_t maxMP = 100;
	this->getStats()->setMaxMP(maxMP);
}

void Player::updatePhysicalDefense() {
	word_t defense = 15;
	this->getStats()->setPhysicalDefense(defense);
}

void Player::updateMagicalDefense() {
	word_t defense = 15;
	this->getStats()->setMagicalDefense(defense);
}

void Player::updateMovementSpeed() {
	word_t movementSpeed = 200;
	Inventory *inventory = this->getCharacter()->getInventory();
	Stance* playerStance = this->getStats()->getStance();

	if (playerStance->isRunning()) {
		movementSpeed = 425;
		if (inventory->get(Inventory::Slots::SHOES).getAmount() > 0) {
			EquipmentSTB* stb = ROSEServer::getServer<WorldServer>()->getEquipmentSTB(ItemType::SHOES);
			movementSpeed += stb->getMovementSpeed(inventory->get(Inventory::Slots::SHOES).getId()) - 50;
		}
	}
	else if (playerStance->isDriving()) {
		movementSpeed = 1200;
	}
	this->getStats()->setMovementSpeed(movementSpeed);
}

void Player::updateAttackSpeed() {
	word_t atkSpeed = 115;
	Inventory *inventory = this->getCharacter()->getInventory();
	if (inventory->get(Inventory::Slots::WEAPON).getAmount() > 0) {
		EquipmentSTB* stb = ROSEServer::getServer<WorldServer>()->getEquipmentSTB(ItemType::WEAPON);
		atkSpeed = 1500 / (5 + stb->getAttackSpeed(inventory->get(Inventory::Slots::WEAPON).getId()));
	}
	this->getStats()->setAttackSpeed(atkSpeed);
}

void Player::updateAttackRange() {
	word_t atkRange = 100;
	Inventory *inventory = this->getCharacter()->getInventory();
	if (inventory->get(Inventory::Slots::WEAPON).getAmount() > 0) {
		EquipmentSTB* stb = ROSEServer::getServer<WorldServer>()->getEquipmentSTB(ItemType::WEAPON);
		atkRange = stb->getAttackRange(inventory->get(Inventory::Slots::WEAPON).getId());
	}
	this->getStats()->setAttackRange(atkRange);
}
