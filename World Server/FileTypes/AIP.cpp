#include "AIP.h"
#include "..\Entities\NPC.h"
#include "..\Entities\Monster.h"
#include "..\Entities\Player.h"
#include "..\..\Common\BasicTypes\Randomizer.h"
#include "..\..\Common\BasicTypes\FunctionBinder.h"

const AI::Action::ActionFunctionPtr AI::Action::ActionMapping[] = {
	&AI::Action::StopAction,
	&AI::Action::SetEmote,
	&AI::Action::SayBubbledMessage,
	&AI::Action::SetRandomPositionFromCurrent,
	&AI::Action::SetRandomPositionFromSpawn,
	&AI::Action::SetRandomPositionFromTarget,
	&AI::Action::AttackTarget,
	&AI::Action::SetSpecialAttack,
	&AI::Action::MoveToTarget,
	&AI::Action::Convert,
	&AI::Action::SpawnPet,
	&AI::Action::CallAlliesForAttack,
	&AI::Action::AttackNearestTarget,
	&AI::Action::AttackFoundTarget,
	&AI::Action::CallEntireFamilyForAttack,
	&AI::Action::AttackDesignatedTarget,
	&AI::Action::RunAway,
	&AI::Action::DropRandomItem,
	&AI::Action::CallFewAlliesForAttack,
	&AI::Action::AttackNearestTarget,
	&AI::Action::SpawnPetAtPosition,
	&AI::Action::LogAction,
	&AI::Action::LogAction,
	&AI::Action::Suicide,
	&AI::Action::CastSkill,
	&AI::Action::ChangeNPCVar,
	&AI::Action::ChangeGlobalVar,
	&AI::Action::ChangeEconomyVar,
	&AI::Action::SayMessage,
	&AI::Action::MoveToOwner,
	&AI::Action::SetQuestTrigger,
	&AI::Action::AttackOwnersTarget,
	&AI::Action::SetMapAsPVPArea,
	&AI::Action::SetMapAsPVEArea,
	&AI::Action::GiveItemToOwner,
	&AI::Action::SetAIVar,
	&AI::Action::SpawnPetAtMyPosition,
	&AI::Action::SpawnPetAtGivenPosition,
	&AI::Action::LogAction,
	&AI::Action::LogAction,
	&AI::Action::LogAction,
	&AI::Action::LogAction,
	&AI::Action::LogAction,
	&AI::Action::LogAction,
	&AI::Action::LogAction
};

const AI::Condition::ConditionFunctionPtr AI::Condition::ConditionMapping[] = {
	&AI::Condition::FightOrDelay,
	&AI::Condition::HasEnoughDamageReceived,
	&AI::Condition::HasEnoughTargets,
	&AI::Condition::CheckDistanceFromSpawn,
	&AI::Condition::CheckDistanceToTarget,
	&AI::Condition::CheckAbilityDifference,
	&AI::Condition::CheckPercentHP,
	&AI::Condition::CheckRandomPercentage,
	&AI::Condition::FindNearestSuitableTarget,
	&AI::Condition::HasTargetChanged,
	&AI::Condition::CheckAbility,
	&AI::Condition::HasEnoughStats,
	&AI::Condition::HasDaytimeArrived,
	&AI::Condition::HasBuff,
	&AI::Condition::IsObjectVarValid,
	&AI::Condition::IsWorldVarValid,
	&AI::Condition::IsEconomyValid,
	&AI::Condition::IsNPCNearby,
	&AI::Condition::CheckDistanceToOwner,
	&AI::Condition::CheckZoneTime,
	&AI::Condition::CheckAreOwnStatsEnough,
	&AI::Condition::HasNoOwner,
	&AI::Condition::HasOwner,
	&AI::Condition::CheckWorldTime,
	&AI::Condition::CheckWeekDate,
	&AI::Condition::CheckMonthDate,
	&AI::Condition::CheckUnknown,
	&AI::Condition::CheckSurroundingLevelDifference,
	&AI::Condition::CheckAIVar,
	&AI::Condition::IsTargetClanMaster,
	&AI::Condition::CheckClanCreationTime,
	&AI::Condition::LogCondition,
	&AI::Condition::LogCondition,
	&AI::Condition::LogCondition,
	&AI::Condition::LogCondition,
	&AI::Condition::LogCondition,
	&AI::Condition::LogCondition,
	&AI::Condition::LogCondition,
	&AI::Condition::LogCondition,
	&AI::Condition::LogCondition,
	&AI::Condition::LogCondition,
	&AI::Condition::LogCondition,
	&AI::Condition::LogCondition,
	&AI::Condition::LogCondition,
	&AI::Condition::LogCondition
};

FunctionBinder<Stats, word_t> STATS_MAPPER = {
	{ 0x00, &Stats::getLevel },
	{ 0x01, &Stats::getAttackPower },
	{ 0x02, &Stats::getPhysicalDefense },
	{ 0x03, &Stats::getMagicalDefense },
	{ 0x04, nullptr }
};

void AI::doRoutine(NPC* npc, const byte_t blockType, AIP* aiData) {
	AIP::State* currentState = aiData->getState(blockType);
	if (!currentState) {
		return;
	}
	AI::InformationTransfer trans(blockType, npc);
	auto recordPtr = currentState->getRecords();
	for (unsigned int i = 0; i < currentState->getRecordAmount(); i++, recordPtr++) {
		auto record = *recordPtr;
		if (record != nullptr) {
			bool conditionsFulfilled = true;
			auto conds = record->getConditions();
			for (unsigned int j = 0; j < record->getConditionAmount(); j++, conds++) {
				auto cond = *conds;
				if (!cond->isConditionFulfilled(npc, &trans)) {
					conditionsFulfilled = false;
					break;
				}
			}
			if (conditionsFulfilled) {
				auto actions = record->getActions();
				for (unsigned int j = 0; j < record->getActionAmount(); j++, actions++) {
					auto action = *actions;
					action->doAction(npc, &trans);
				}
				break;
			}
		}
	}
	npc->getAI()->updateTimer();
}

bool AI::Condition::LogCondition(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	String message = String("[CONDITION] Opcode: ") + String::fromHex(condition->getOpCode()) + String(" with data-length: ") + String::fromHex(condition->getLength());
	std::cout << message << "\n";
	return false;
}

bool AI::Condition::FightOrDelay(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::HasEnoughDamageReceived(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	DataInterpreter<const char> reader(condition->getData(), condition->getLength());
	dword_t dmgAmountNecessary = reader.get<dword_t>();
	byte_t isActionOnDeal = reader.get<dword_t>() > 0;
	if (isActionOnDeal) {
		return false;
	}
	dword_t dmgReceived = entity->getStats()->getMaxHP() - entity->getStats()->getHP();

	return dmgReceived >= dmgAmountNecessary;
}

bool AI::Condition::HasEnoughTargets(NPC* npc, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	DataInterpreter<const char> reader(condition->getData(), condition->getLength());
	dword_t distance = reader.get<dword_t>();
	bool needsAlliedEntity = reader.get<word_t>() > 0;
	word_t levelMin = reader.get<word_t>();
	word_t levelMax = reader.get<word_t>();
	word_t necessaryAmount = reader.get<word_t>();

	float nearest = 999999.0f;
	word_t foundAmount = 0x00;

	Entity* target = nullptr;
	auto visibleSectors = npc->getVisuality()->getVisibleSectors();
	for (unsigned int i = 0; i < Visuality::MAXIMUM_VISIBLE_SECTORS; i++, visibleSectors++) {
		Map::Sector* sector = *visibleSectors;
		if (sector == nullptr) {
			continue;
		}
		for (auto it = sector->beginEntities(); it != sector->endEntities(); it++) {
			Entity* currentEntity = it->second;
			if (!currentEntity->getBasicInformation()->isIngame()) {
				continue;
			}
			short levelDiff = currentEntity->getStats()->getLevel() - npc->getStats()->getLevel();
			if (levelMin >= levelDiff && levelDiff <= levelMax && currentEntity->isEnemyOf(npc) != needsAlliedEntity) {
				foundAmount++;
				float distance = npc->getPositionInformation()->getCurrent().distanceTo(currentEntity->getPositionInformation()->getCurrent());
				if (distance <= nearest) {
					nearest = distance;
					target = currentEntity;
				}
				if (foundAmount >= necessaryAmount) {
					transfer->setFoundTarget(target);
					return true;
				}
			}
		}
	}
	return false;
}

bool AI::Condition::CheckDistanceFromSpawn(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	if (!entity->isMonster()) {
		return false;
	}
	Monster* mon = static_cast<Monster*>(entity);
	DataInterpreter<const char> reader(condition->getData(), condition->getLength());
	float allowedDistance = static_cast<float>(reader.get<dword_t>() * 100.0f);
	float currentDistance = entity->getPositionInformation()->getCurrent().distanceTo(mon->getSpawn()->getSpawnData().getPosition());
	return currentDistance >= allowedDistance;
}

bool AI::Condition::CheckDistanceToTarget(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	if (entity->getCombatInformation()->getTarget() == nullptr) {
		return false;
	}
	return false;
}

bool AI::Condition::CheckAbilityDifference(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	if (!entity->getCombatInformation()->getTarget()) {
		return false;
	}
	DataInterpreter<const char> reader(condition->getData(), condition->getLength());
	byte_t abilityType = static_cast<byte_t>(reader.get<dword_t>());
	long difference = reader.get<long>();
	byte_t operation = (reader.get<byte_t>() > 0 ? OperationService::OPERATION_SMALLER_EQUAL : OperationService::OPERATION_BIGGER_EQUAL);
	
	word_t abilityValue = 0x00;
	if (abilityType == 0x04 && !entity->getCombatInformation()->getTarget()->isPlayer()) {
		return false;
	}
	if (abilityType == 0x04) {
		abilityValue = static_cast<Player*>(entity->getCombatInformation()->getTarget())->getAttributes()->getTotalCharm();
	}
	else {
		auto func = STATS_MAPPER.getGetterFunction(abilityType);
		abilityValue = (entity->getStats()->*func)();
	}
	return OperationService::checkOperation(abilityValue, difference, operation);
}

bool AI::Condition::CheckPercentHP(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	DataInterpreter<const char> reader(condition->getData(), condition->getLength());
	dword_t wantedPercentage = reader.get<dword_t>();
	byte_t operation = reader.get<byte_t>() > 0 ? OperationService::OPERATION_BIGGER_EQUAL : OperationService::OPERATION_SMALLER_EQUAL;
	byte_t percentHealth = static_cast<byte_t>(entity->getStats()->getHP() * 100 / entity->getStats()->getMaxHP());
	return OperationService::checkOperation(percentHealth, wantedPercentage, operation);
}

bool AI::Condition::CheckRandomPercentage(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	DataInterpreter<const char> reader(condition->getData(), condition->getLength());
	byte_t chance = reader.get<byte_t>();
	dword_t rolled = Randomize::getUInt(0, 100);
	return chance <= rolled;
}

bool AI::Condition::FindNearestSuitableTarget(NPC* npc, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	DataInterpreter<const char> reader(condition->getData(), condition->getLength());
	float distance = static_cast<float>(reader.get<dword_t>()) * 100.0f;
	short levelDiffMin = reader.get<short>();
	short levelDiffMax = reader.get<short>();
	bool needsAlly = reader.get<byte_t>() > 0;
	auto visibleSectors = npc->getVisuality()->getVisibleSectors();
	for (unsigned int i = 0; i < Visuality::MAXIMUM_VISIBLE_SECTORS; i++, visibleSectors++) {
		Map::Sector* sector = *visibleSectors;
		if (!sector) {
			continue;
		}
		for (auto& it = sector->beginEntities(); it != sector->endEntities(); it++) {
			Entity* entity = it->second;
			short currentLevelDiff = npc->getStats()->getLevel() - entity->getStats()->getLevel();
			float currentDistance = npc->getPositionInformation()->getCurrent().distanceTo(entity->getPositionInformation()->getCurrent());
			if (currentLevelDiff >= levelDiffMin && currentLevelDiff <= levelDiffMax && currentDistance <= distance) {
				transfer->setFoundTarget(entity);
				transfer->setNearestTarget(entity);
			}
		}
	}
	return transfer->getNearestTarget() != nullptr;
}

bool AI::Condition::HasTargetChanged(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return entity->getCombatInformation()->getTarget() != transfer->getDesignatedTarget();
}

bool AI::Condition::CheckAbility(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::HasEnoughStats(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::HasDaytimeArrived(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::HasBuff(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::IsObjectVarValid(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	DataInterpreter<const char> data(condition->getData(), condition->getLength());
	byte_t idx = static_cast<byte_t>(data.get<dword_t>());
	int value = data.get<int>();
	byte_t operation = data.get<byte_t>();
	return OperationService::checkOperation(entity->getAI()->getObjectVar(idx), value, operation);
}

bool AI::Condition::IsWorldVarValid(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::IsEconomyValid(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::IsNPCNearby(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckDistanceToOwner(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckZoneTime(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckAreOwnStatsEnough(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	DataInterpreter<const char> data(condition->getData(), condition->getLength());
	byte_t abilityType = static_cast<byte_t>(data.get<dword_t>());
	int value = data.get<int>();
	byte_t operation = data.get<byte_t>();

	auto function = STATS_MAPPER.getGetterFunction(abilityType);

	return OperationService::checkOperation((entity->getStats()->*function)(), value, operation);

}

bool AI::Condition::HasNoOwner(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return true;

}

bool AI::Condition::HasOwner(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckWorldTime(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckWeekDate(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckMonthDate(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckUnknown(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckSurroundingLevelDifference(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckAIVar(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	DataInterpreter<const char> data(condition->getData(), condition->getLength());
	byte_t idx = static_cast<byte_t>(data.get<dword_t>());
	int value = data.get<int>();
	byte_t operation = data.get<byte_t>();

	return OperationService::checkOperation(entity->getAI()->getAIVar(idx), value, operation);

}

bool AI::Condition::IsTargetClanMaster(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckClanCreationTime(NPC* entity, const AI::Condition* condition, AI::InformationTransfer* transfer) {
	return false;

}





void AI::Action::LogAction(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) {
	String message = String("[ACTION] Opcode: ") + String::fromHex(action->getOpCode()) + String(" with data-length: ") + String::fromHex(action->getLength());
	std::cout << message << "\n";
}

void AI::Action::StopAction(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 
	entity->getPositionInformation()->setDestination(entity->getPositionInformation()->getCurrent());
}

void AI::Action::SetEmote(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::SayBubbledMessage(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::SetRandomPositionFromCurrent(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 
	DataInterpreter<const char> reader(action->getData(), action->getLength());
	float distance = static_cast<float>(reader.get<dword_t>() * 100);
	Position newDestination = entity->getPositionInformation()->getCurrent() + 
		Position(Randomize::GetFloat(-distance, distance), Randomize::GetFloat(-distance, distance));

	byte_t stance = reader.get<byte_t>();
	entity->getStats()->getStance()->setId(stance);
	entity->getPositionInformation()->setDestination(newDestination);
}

void AI::Action::SetRandomPositionFromSpawn(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 
	if (!entity->isMonster()) {
		return;
	}
	Monster* mon = static_cast<Monster*>(entity);
	if (mon->getSpawn() == nullptr) {
		return;
	}
	DataInterpreter<const char> reader(action->getData(), action->getLength());
	float distance = static_cast<float>(reader.get<dword_t>());
	Position newDestination = mon->getSpawn()->getSpawnData().getPosition() + 
		Position(Randomize::GetFloat(-distance, distance), Randomize::GetFloat(-distance, distance));

	byte_t stance = reader.get<byte_t>();
	entity->getStats()->getStance()->setId(stance);
	entity->getPositionInformation()->setDestination(newDestination);
}

void AI::Action::SetRandomPositionFromTarget(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 
	if (transfer->getFoundTarget() == nullptr) {
		return;
	}
	Position targetPosition = transfer->getFoundTarget()->getPositionInformation()->getCurrent();
	Position newPosition = targetPosition + Position(Randomize::GetFloat(-400.0f, 400.0f), Randomize::GetFloat(-400.0f, 400.0f));
	byte_t stance = action->getData()[0];

	entity->getStats()->getStance()->setId(stance);
	entity->getPositionInformation()->setDestination(newPosition);
}

void AI::Action::AttackTarget(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}

void AI::Action::SetSpecialAttack(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::MoveToTarget(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 
	if (entity->getCombatInformation()->getTarget() == nullptr && transfer->getFoundTarget() == nullptr) {
		return;
	}
	DataInterpreter<const char> reader(action->getData(), action->getLength());
	float wantedDistance = static_cast<float>(reader.get<dword_t>());
	byte_t stance = reader.get<byte_t>();
	auto target = (transfer->getFoundTarget() == nullptr ? entity->getCombatInformation()->getTarget() : transfer->getFoundTarget());
	auto currentPos = target->getPositionInformation()->getCurrent();
	Position positionRatios = (currentPos - entity->getPositionInformation()->getCurrent()).normalize();

	Position destination(currentPos.getX() + (Randomize::GetFloat(-wantedDistance, wantedDistance) * positionRatios.getX()),
		currentPos.getY() + (Randomize::GetFloat(-wantedDistance, wantedDistance) * positionRatios.getY()));

	entity->getStats()->getStance()->setId(stance);
	entity->getPositionInformation()->setDestination(destination);
}

void AI::Action::Convert(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::SpawnPet(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::CallAlliesForAttack(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::AttackNearestTarget(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::AttackFoundTarget(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::CallEntireFamilyForAttack(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::AttackDesignatedTarget(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::RunAway(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::DropRandomItem(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::CallFewAlliesForAttack(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::SpawnPetAtPosition(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::Suicide(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) {
	entity->addDamage(entity, entity->getStats()->getHP());
}

void AI::Action::CastSkill(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::ChangeNPCVar(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::ChangeGlobalVar(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::ChangeEconomyVar(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::SayMessage(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::MoveToOwner(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::SetQuestTrigger(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::AttackOwnersTarget(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::SetMapAsPVPArea(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::SetMapAsPVEArea(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) {

}
void AI::Action::GiveItemToOwner(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) { 

}
void AI::Action::SetAIVar(NPC* entity, const AI::Action* action, AI::InformationTransfer* transfer) {
	DataInterpreter<const char> reader(action->getData(), action->getLength());
	byte_t idx = static_cast<byte_t>(reader.get<dword_t>());
	int value = reader.get<int>();
	byte_t operation = reader.get<byte_t>();

	int currentValue = entity->getAI()->getAIVar(idx);
	entity->getAI()->setAIVar(idx, OperationService::resultOperation(currentValue, value, operation));
}

void AI::Action::SpawnPetAtMyPosition(NPC* entity, const AI::Action* action, InformationTransfer* transfer) {
	DataInterpreter<const char> reader(action->getData(), action->getLength());
	word_t monId = reader.get<word_t>();
	bool assignAsMaster = reader.get<byte_t>() > 0;
	std::cout << "Spawn monster " << monId << ". Spawn as pet: " << assignAsMaster << "\n";
}

void AI::Action::SpawnPetAtGivenPosition(NPC* entity, const AI::Action* action, InformationTransfer *transfer) {
	DataInterpreter<const char> reader(action->getData(), action->getLength());
	word_t monId = reader.get<word_t>();
	byte_t position = static_cast<byte_t>(reader.get<word_t>());
	float distance = static_cast<float>(reader.get<dword_t>()) * 100.0f;
	bool assignAsMaster = reader.get<byte_t>() > 0;

	Position p = entity->getPositionInformation()->getCurrent();
	switch (position) {
		case 0x00:
			//current
		break;
		case 0x01:
			if (transfer->getDesignatedTarget() != nullptr) {
				p = transfer->getDesignatedTarget()->getPositionInformation()->getCurrent();
			}
		break;
		case 0x02:
			if (entity->getCombatInformation()->getTarget() != nullptr) {
				p = entity->getCombatInformation()->getTarget()->getPositionInformation()->getCurrent();
			}
		break;
	}
	MonsterSpawn* spawn = (entity->isMonster() ? static_cast<Monster*>(entity)->getSpawn() : nullptr);
	Position spawnPosition(p.getX() + Randomize::GetFloat(-distance, distance), p.getY() + Randomize::GetFloat(-distance, distance));
	Entity* owner = nullptr;
	if (assignAsMaster) {
		owner = entity;
	}
	Monster* mon = new Monster(monId, entity->getPositionInformation()->getMap()->getId(), spawnPosition, spawn, owner);
}