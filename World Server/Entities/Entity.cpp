#include "Entity.h"
#include "Player.h"
#include "..\FileTypes\ZMO.h"
#include "..\..\Common\PacketIDs.h"
#include "..\..\Common\BasicTypes\Randomizer.h"

FunctionBinder <Entity, dword_t, void(Entity::*)()> ZMOTRIGGER_TO_ID = {
	{ 31, &Entity::doNormalAttack },
	{ 32, &Entity::doNormalAttack },
	{ 33, &Entity::doNormalAttack },
	{ 34, &Entity::doSkillAttack },
	{ 35, &Entity::doNormalAttack },
	{ 36, &Entity::doBuff }
};

Entity::Entity() {
	basicIngameInformation = new EntityInfo();
	positions = new PositionInformation();
	stats = new Stats();
	visuality = new Visuality();
	combat = new Combat();
	currentAnimation = nullptr;

	auto onNewSectorFunc = [&](Map::Sector* sector) {
		if (sector == nullptr) {
			return;
		}
		std::for_each(sector->beginEntities(), sector->endEntities(), [&](std::pair<const word_t, Entity*> entityPair) {
			Entity* other = entityPair.second;
			if (other != this && other != nullptr) {
				this->sendEntityVisuallyAdded(other);
				other->sendEntityVisuallyAdded(this);
			}
		});
	};
	auto onRemoveSectorFunc = [&](Map::Sector* sector) {
		if (sector == nullptr) {
			return;
		}
		std::for_each(sector->beginEntities(), sector->endEntities(), [&](std::pair<const word_t, Entity*> entityPair) {
			Entity* other = entityPair.second;
			if (other != this && other != nullptr) {
				this->sendEntityVisuallyRemoved(other);
				other->sendEntityVisuallyRemoved(this);
			}
		});
	};
	this->getPositionInformation()->setOnDestinationAssigned([this](Position old) {
		this->sendNewDestinationVisually();
	});

	this->getCombatInformation()->setOnNewTarget([this](Entity* oldTarget) {
		this->sendNewTarget();
	});

	this->getVisuality()->setOnNewSector(onNewSectorFunc);
	this->getVisuality()->setOnRemoveSector(onRemoveSectorFunc);
}

Entity::~Entity() {
	delete basicIngameInformation;
	basicIngameInformation = nullptr;

	delete positions;
	positions = nullptr;

	delete stats;
	stats = nullptr;
	
	delete visuality;
	visuality = nullptr;
	
	delete combat;
	combat = nullptr;
}

void Entity::updateStats() {
	this->updateAttackPower();
	this->updateMaxHP();
	this->updateMaxMP();
	this->updatePhysicalDefense();
	this->updateMagicalDefense();
	this->updateMovementSpeed();
	this->updateAttackSpeed();
	this->updateAttackRange();
};

void Entity::movementProc() {
	auto& attackTimer = this->getCombatInformation()->getAttackTimer();

	//Call update timer upon finishing within this function
	Autoclose<> attackTimerUpdate;
	attackTimerUpdate.setAutoclose([&]() {
		attackTimer.update();
	});
	auto target = this->getCombatInformation()->getTarget();
	bool isTargetValid = target != nullptr;

	const Position& current = this->getPositionInformation()->getCurrent();
	const Position& dest = (isTargetValid ? target->getPositionInformation()->getDestination() : this->getPositionInformation()->getDestination());
	Position diff = (dest - current);

	float threshold = (isTargetValid ? (this->getStats()->getAttackRange() + target->getSize()) : 10);
	float distance = diff.toLength() - threshold; //distance gets reduced by the actual reach (e.g. weapon range)

	float necessaryTime = (distance / static_cast<float>(this->getStats()->getMovementSpeed())) * 1000.0f;
	float timePassed = static_cast<float>(this->getPositionInformation()->updateDuration());

	bool playerFlag = this->isPlayer();
	if (playerFlag && isTargetValid) {
		Player* p = static_cast<Player*>(this);
		if (p->getDebuggingFlags().getPrintTargetPosition()) {
			p->sendDebugMessage(String("Position of target: ") + dest.toString());
		}
	}

	Position targetPosition;
	if (timePassed < necessaryTime && distance > 0.1f) { //basically: we didn't reach our target
		//Is currently an attack ongoing? If so, don't do anything.
		if (attackTimer.isRunning()) {
			attackTimer.softStop();
			targetPosition = current;
		}
		else { //No attack to do and we have a destination to reach.
			diff.normalize();

			float totalMoved = (timePassed * static_cast<float>(this->getStats()->getMovementSpeed()) / 1000.0f);
			float movedX = totalMoved * diff.getX();
			float movedY = totalMoved * diff.getY();

			targetPosition = Position(current.getX() + movedX, current.getY() + movedY);
		}
	} else {
		//We are in range. Are we attacking or not?
		if (isTargetValid) {
			if (!attackTimer.isRunning()) {
				attackTimer.start();
				this->setAttackMotion();
			}
			//In case we are attacking, stand still
			targetPosition = current;
		} else { //No target and we are in range -> current = dest
			targetPosition = dest;
		}
	}
	this->getPositionInformation()->setCurrent(targetPosition);
	return;
}

void Entity::doAction() {
}

void Entity::doNormalAttack() {
	auto target = this->getCombatInformation()->getTarget();
	if (!target) {
		return;
	}
	word_t damage = Randomize::getUInt(10, 30);
	target->addDamage(this, damage);
}

void Entity::doSkillAttack() {

}

void Entity::doBuff() {

}


void Entity::addDamage(Entity* dmgDealer, const dword_t damage) {
	if (this->isNPC()) {
		return;
	}

	word_t flag = 0x00;
	if (damage >= this->getStats()->getHP()) {
		this->getStats()->setHP(0);
		flag |= 0x8000; //Death
		this->onDeath();

		this->getCombatInformation()->clear();
		dmgDealer->getCombatInformation()->clear();
	}
	else {
		this->getStats()->setHP(this->getStats()->getHP() - damage);
	}
	Packet pak(PacketID::World::Response::BASIC_ATTACK);
	pak.addWord(dmgDealer->getBasicInformation()->getLocalId());
	pak.addWord(this->getBasicInformation()->getLocalId());
	pak.addWord((damage & 0x7FF) | flag);
	this->sendToVisible(pak);
}

void Visuality::update(std::map<word_t, Map::Sector*>& visibleSectors) {
	auto sectors = this->visibleSectors;
	int erasedSectorPositions[9] = { -1 };
	int idx = 0;
	for (unsigned int i = 0; i < 9; i++, sectors++) {
		auto sector = *sectors;
		if (sector != nullptr) {
			if (visibleSectors.count(sector->getId()) == 0) {
				this->removeSector(sector);
				*sectors = nullptr;
				erasedSectorPositions[idx++] = i;
			}
			else {
				visibleSectors.erase(sector->getId());
			}
		}
		else {
			erasedSectorPositions[idx++] = i;
		}
	}
	idx = 0;
	for (auto it = visibleSectors.begin(); it != visibleSectors.end(); it++) {
		Map::Sector* sector = it->second;
		this->visibleSectors[erasedSectorPositions[idx++]] = sector;
		this->addSector(sector);
	}
}

Entity* Visuality::find(const word_t localId) {
	auto sectors = this->getVisibleSectors();
	for (unsigned int i = 0; i < Visuality::MAXIMUM_VISIBLE_SECTORS; i++, sectors++) {
		Map::Sector* sector = *sectors;
		if (!sector) {
			continue;
		}
		for (auto entityIt = sector->beginEntities(); entityIt != sector->endEntities(); entityIt++) {
			auto entity = entityIt->second;
			if (entity->getBasicInformation()->getLocalId() == localId) {
				return entity;
			}
		}
	}
	return nullptr;
}

void Visuality::forceClear() {
	auto sectors = this->getVisibleSectors();
	for (unsigned int i = 0; i < Visuality::MAXIMUM_VISIBLE_SECTORS;i++, sectors++) {
		Map::Sector* sector = *sectors;
		this->removeSector(sector);
		(*sectors) = nullptr;
	}
}

Entity* Combat::getTarget() const {
	return this->target.getValue();
}

void Combat::setTarget(Entity* target) {
	this->target = target;
}

bool Entity::sendNewTarget() {
	Entity* newTarget = this->getCombatInformation()->getTarget();
	if (newTarget && !newTarget->isNPC()) {
		if (this->isMonster()) {
			this->getStats()->getStance()->setRunningStance();
		}

		Packet pak(PacketID::World::Response::INIT_BASIC_ATTACK);
		pak.addWord(this->getBasicInformation()->getLocalId());
		pak.addWord(newTarget->getBasicInformation()->getLocalId());
		pak.addWord(0x00);
		pak.addPosition(newTarget->getPositionInformation()->getCurrent());
		return this->sendToVisible(pak);
	}
	return true;
}

void Entity::setAttackMotion() {
	auto& attackTimer = this->getCombatInformation()->getAttackTimer();
	if (this->currentAnimation != nullptr) {
		WrappingTriggerClock wtc(false, currentAnimation->getDefaultPlayoutTime());
		auto& trigger = currentAnimation->getTrigger();
		for (auto it = trigger.cbegin(); it != trigger.cend(); it++) {
			auto& pair = (*it);
			wtc.addTrigger(pair.first, [&]() {
				auto func = ZMOTRIGGER_TO_ID.getProcessingFunction(pair.second);
				if (func != nullptr) {
					(this->*func)();
				}
			});
		}
		wtc.adjustTriggerTimes((this->getStats()->getAttackSpeed() - 100) / 100.0f);

		attackTimer.copyTrigger(wtc);
	}
	else {
		attackTimer.softStop();
	}
}

word_t Combat::getTargetId() const {
	return (this->getTarget() == nullptr ? 0x00 : this->getTarget()->getBasicInformation()->getLocalId());
}

bool Entity::sendToVisible(const Packet& pak) {
	return this->sendToVisible(pak, nullptr);
}

bool Entity::sendToVisible(const Packet& pak, Entity* exception) {
	bool result = true;
	auto visibleSectors = this->getVisuality()->getVisibleSectors();
	for (unsigned int i = 0; i < Visuality::MAXIMUM_VISIBLE_SECTORS; i++, visibleSectors++) {
		Map::Sector *sector = *visibleSectors;
		if (!sector) {
			continue;
		}
		std::for_each(sector->beginPlayer(), sector->endPlayer(), [&](std::pair<const word_t, Entity*> otherPair) {
			Player *player = static_cast<Player*>(otherPair.second);
			if (player->isActive() && player->getBasicInformation()->isIngame() && player != exception) {
				result &= player->sendPacket(pak);
			}
		});
	}
	return result;
}

bool Entity::sendCurrentStance() {
	Packet pak(PacketID::World::Response::CHANGE_STANCE);
	pak.addWord(this->getBasicInformation()->getLocalId());
	pak.addByte(this->getStats()->getStance()->getId());
	pak.addWord(this->getStats()->getMovementSpeed());
	return this->sendToVisible(pak);
}


const Combat::Type Combat::Type::NONE = Combat::Type(0);
const Combat::Type Combat::Type::NORMAL = Combat::Type(1);
const Combat::Type Combat::Type::SKILL = Combat::Type(2);