#include "Player.h"

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
	word_t basicMovementspeed = this->getStats()->getStance()->isRunning() ? 425 : 200;
	this->getStats()->setMovementSpeed(basicMovementspeed);
}

void Player::updateAttackSpeed() {
	word_t atkSpeed = 115;
	this->getStats()->setAttackSpeed(atkSpeed);
}

void Player::updateAttackRange() {
	word_t atkRange = 100;
	this->getStats()->setAttackRange(atkRange);
}
