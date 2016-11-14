#include "AIP.h"
#include "..\Entities\NPC.h"
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
	&AI::Action::SpawnPetAtPosition,
	&AI::Action::KillNPC,
	&AI::Action::CastSkill,
	&AI::Action::ChangeNPCVar,
	&AI::Action::ChangeGlobalVar,
	&AI::Action::ChangeEconomyVar,
	&AI::Action::SayMessage,
	&AI::Action::MoveToOwner,
	&AI::Action::SetQuestTrigger,
	&AI::Action::AttackOwnersTarget,
	&AI::Action::SetMapAsPVPArea,
	&AI::Action::GiveItemToOwner,
	&AI::Action::SetAIVar,
	nullptr,
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
	nullptr
};


bool AI::Condition::FightOrDelay(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::HasEnoughDamageReceived(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	DataInterpreter<const char> reader(condition.getData(), condition.getLength());
	dword_t dmgAmountNecessary = reader.get<dword_t>();
	byte_t isActionOnDeal = reader.get<dword_t>() > 0;
	if (isActionOnDeal) {
		return false;
	}
	dword_t dmgReceived = entity->getStats()->getMaxHP() - entity->getStats()->getHP();

	return dmgReceived >= dmgAmountNecessary;
}

bool AI::Condition::HasEnoughTargets(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	DataInterpreter<const char> reader(condition.getData(), condition.getLength());
	dword_t distance = reader.get<dword_t>();
	word_t isAlliedEntity = reader.get<word_t>();
	word_t levelMin = reader.get<word_t>();
	word_t levelMax = reader.get<word_t>();
	word_t necessaryAmuont = reader.get<word_t>();

	float nearest = 999999.0f;

	Entity* target = nullptr;
	//....todo
	return false;
}

bool AI::Condition::CheckDistanceFromSpawn(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::CheckDistanceToTarget(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::CheckAbilityDifference(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::CheckPercentHP(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::CheckRandomPercentage(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::FindNearestSuitableTarget(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::HasTargetChanged(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::CheckAbility(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::HasEnoughStats(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::HasDaytimeArrived(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::HasBuff(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::IsObjectVarValid(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::IsWorldVarValid(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;
}

bool AI::Condition::IsEconomyValid(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::IsNPCNearby(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckDistanceToOwner(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckZoneTime(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckAreOwnStatsEnough(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::HasNoOwner(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::HasOwner(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckWorldTime(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckWeekDate(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckMonthDate(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckUnknown(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckSurroundingLevelDifference(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckAIVar(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::IsTargetClanMaster(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

bool AI::Condition::CheckClanCreationTime(NPC* entity, const AI::Condition& condition, AI::InformationTransfer* transfer) {
	return false;

}

void AI::Action::StopAction(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::SetEmote(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::SayBubbledMessage(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer){ }
void AI::Action::SetRandomPositionFromCurrent(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::SetRandomPositionFromSpawn(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::SetRandomPositionFromTarget(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::AttackTarget(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::SetSpecialAttack(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::MoveToTarget(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::Convert(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::SpawnPet(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::CallAlliesForAttack(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::AttackNearestTarget(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::AttackFoundTarget(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::CallEntireFamilyForAttack(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::AttackDesignatedTarget(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::RunAway(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::DropRandomItem(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::CallFewAlliesForAttack(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::SpawnPetAtPosition(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::KillNPC(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::CastSkill(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::ChangeNPCVar(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::ChangeGlobalVar(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::ChangeEconomyVar(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::SayMessage(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::MoveToOwner(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::SetQuestTrigger(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::AttackOwnersTarget(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::SetMapAsPVPArea(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::GiveItemToOwner(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }
void AI::Action::SetAIVar(NPC* entity, const AI::Action& action, AI::InformationTransfer* transfer) { }