#include "Entity.h"
#include "Player.h"
#include "..\..\Common\PacketIDs.h"
#include "..\..\Common\BasicTypes\Randomizer.h"

Entity::Entity() {
	basicIngameInformation = new EntityInfo();
	positions = new PositionInformation();
	stats = new Stats();
	visuality = new Visuality();
	combat = new Combat();
	this->getCombatInformation()->getAttackTimer().addTrigger(450, [this]() {
		this->attackRoutine();
	});

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
	auto target = this->getCombatInformation()->getTarget();
	bool isTargetValid = target != nullptr;

	const Position& current = this->getPositionInformation()->getCurrent();
	const Position& dest = (isTargetValid ? target->getPositionInformation()->getDestination() : this->getPositionInformation()->getDestination());

	float distance = (current - dest).toLength();
	float threshold = (isTargetValid ? this->getStats()->getAttackRange() : 10);

	distance -= threshold; //distance gets reduced by the actual reach (e.g. weapon range)
	auto& attackTimer = this->getCombatInformation()->getAttackTimer();
	if (distance <= 0.1f) {
		if (attackTimer.isRunning()) {
			attackTimer.getDuration();
		}
		else {
			//Assume we are in reach of the enemy (if there is one).
			if (isTargetValid) {
				attackTimer.start();
			}
		}
		this->getPositionInformation()->updateDuration();
		return;
	}
	else if (attackTimer.isRunning() && distance > 0.1f) {
		attackTimer.softStop();
		this->getPositionInformation()->updateDuration();
		return;
	}

	float timePassed = static_cast<float>(this->getPositionInformation()->updateDuration()); //should never take longer than MAX_FLOAT
	float necessaryTime = (distance / static_cast<float>(this->getStats()->getMovementSpeed())) * 1000.0f;
	if (timePassed >= necessaryTime) {
		timePassed = (necessaryTime + 1.0f);
		if (isTargetValid) {
			attackTimer.start();
		}
	}
	float pathTraveled = timePassed * static_cast<float>(this->getStats()->getMovementSpeed()) / 1000.0f;
	Position ratio = (dest - current).normalize();
	ratio *= pathTraveled;
	this->getPositionInformation()->setCurrent(Position(current.getX() + ratio.getX(), current.getY() + ratio.getY()));
	return;
}

void Entity::doAction() {
}

void Entity::attackRoutine() {
	auto target = this->getCombatInformation()->getTarget();
	if (!target) {
		return;
	}
	word_t damage = Randomize::getUInt(10, 30);

	word_t flag = 0x00;
	if (damage >= target->getStats()->getHP()) {
		target->getStats()->setHP(0);
		flag |= 0x8000; //Death
		target->onDeath();

		this->getCombatInformation()->clear();
	}
	else {
		target->getStats()->setHP(target->getStats()->getHP() - damage);
	}
	Packet pak(PacketID::World::Response::BASIC_ATTACK);
	pak.addWord(this->getBasicInformation()->getLocalId());
	pak.addWord(target->getBasicInformation()->getLocalId());
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

word_t Combat::getTargetId() const {
	return (this->getTarget() == nullptr ? 0x00 : this->getTarget()->getBasicInformation()->getLocalId());
}

bool Entity::sendToVisible(const Packet& pak) {
	bool result = true;
	auto visibleSectors = this->getVisuality()->getVisibleSectors();
	for (unsigned int i = 0; i < Visuality::MAXIMUM_VISIBLE_SECTORS;i++, visibleSectors++) {
		Map::Sector *sector = *visibleSectors;
		if (!sector) {
			continue;
		}
		std::for_each(sector->beginPlayer(), sector->endPlayer(), [&](std::pair<const word_t, Entity*> otherPair) {
			Player *player = static_cast<Player*>(otherPair.second);
			if (player->isActive() && player->getBasicInformation()->isIngame()) {
				result &= player->getNetworkInterface()->sendPacket(pak);
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

void Entity::addDamage(Entity* dmgDealer, const dword_t damage) {
	if (this->isNPC()) {
		return;
	}
	word_t flag = 0x00;
	if (damage >= this->getStats()->getHP()) {
		this->getStats()->setHP(0);
		flag = 0x8000;

		this->onDeath();
	}
	else {
		this->getStats()->setHP(this->getStats()->getHP() - damage);
	}
	Packet pak(PacketID::World::Response::BASIC_ATTACK);
	pak.addWord(dmgDealer->getBasicInformation()->getLocalId());
	pak.addWord(this->getBasicInformation()->getLocalId());
	pak.addWord((damage & 0x7FF) | flag);
	if (flag & 0x8000) {
		this->sendToVisible(pak);
	}
}


const Combat::Type Combat::Type::NONE = Combat::Type(0);
const Combat::Type Combat::Type::NORMAL = Combat::Type(1);
const Combat::Type Combat::Type::SKILL = Combat::Type(2);