#include "Player.h"
#include "..\..\Common\PacketIDs.h"

FunctionBinder<Player, unsigned long, bool(Player::*)()> Player::PACKET_FUNCTIONS = {
	{ PacketID::World::Request::GET_ID, &Player::pakAssignId },
	{ PacketID::World::Request::IDENFITY, &Player::pakIdentify },
	{ PacketID::World::Request::MOVEMENT_PLAYER, &Player::pakMovement },
	{ PacketID::World::Request::TELEGATE, &Player::pakTeleport },
	{ PacketID::World::Request::LOCAL_CHAT, &Player::pakLocalChat },
	{ PacketID::World::Request::CHANGE_STANCE, &Player::pakChangeStance }
};

/*
template<class _T> Player::getValueOfType(const byte_t type) {
	switch (type) {
		case AbilityTypes::ACCURACY:
			return static_cast<_T>(this->getStats()->getAccuracy());
		break;
		case AbilityTypes::ATTACK_POWER:
			return static_cast<_T>(this->getStats()->getAttackPower());
		break;
		case AbilityTypes::ATTACK_SPEED:
			return static_cast<_T>(this->getStats()->setAttackSpeed());
		break;
	}
}
template<class _T> Player::setValueOfType(const byte_t type, const _T& value);
*/