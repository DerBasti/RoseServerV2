#include "Player.h"
#include "..\..\Common\PacketIDs.h"
#include "..\WorldServer.h"

FunctionBinder<Player, unsigned long, bool(Player::*)(const Packet&)> Player::PACKET_FUNCTIONS = {
	{ PacketID::World::Request::GET_ID, &Player::pakAssignId },
	{ PacketID::World::Request::IDENFITY, &Player::pakIdentify },
	{ PacketID::World::Request::MOVEMENT_PLAYER, &Player::pakMovement },
	{ PacketID::World::Request::TELEGATE, &Player::pakTeleport },
	{ PacketID::World::Request::LOCAL_CHAT, &Player::pakLocalChat },
	{ PacketID::World::Request::CHANGE_STANCE, &Player::pakChangeStance },
	{ PacketID::World::Request::INIT_BASIC_ATTACK, &Player::pakInitBasicAttack }
};


StaticFunctionBinder < String, void(*)(Player* cmdExecutor, SharedArrayPtr<String>& cmdAsTokens)> GM_FUNCTIONS = {
	{ String("/tele"), &GMService::teleport },
	{ String("/where"), &GMService::currentPosition },
	{ String("/exp"), &GMService::setExp },
	{ String("/level"), &GMService::setLevel },
	{ String("/target"), &GMService::targetInformation }
};



bool Player::pakAssignId(const Packet& receivedPacket) {
	this->updateStats();
	this->getPositionInformation()->getMap()->addEntity(this);

	Packet pak(PacketID::World::Response::ASSIGN_ID);
	pak.addWord(this->getBasicInformation()->getLocalId());
	pak.addWord(this->getStats()->getHP());
	pak.addWord(this->getStats()->getMP());
	pak.addDWord(this->getCharacter()->getExperience());
	pak.addDWord(0x00); //UNUSED

	pak.addWord(0x64);
	pak.addDWord(0x0C1F4B79);
	pak.addWord(0x64);
	pak.addDWord(0x3232cd50);
	pak.addDWord(0x32323235);
	pak.addDWord(0x35323232);

	//PVP-MAP (0 = false, 1 = true)
	pak.addDWord(0x00);

	//MAP TIME
	pak.addDWord(0x00);

	//White icon (friendly)
	pak.addDWord(this->getBasicInformation()->getTeamId());
	this->getBasicInformation()->setIngameFlag(true);

	return this->sendPacket(pak) && this->sendWeightPercentage() && this->sendCurrentStance();
}
bool Player::pakIdentify(const Packet& receivedPacket) {
	this->getAccountInfo()->setId(receivedPacket.getDWord(0x00));
	if (!this->sendEncryption() || !this->loadEntireCharacter()) {
		return false;
	}

	return this->sendPlayerInformation() && this->sendInventory() && this->sendQuestData() && this->sendGamingPlan();
}

bool Player::pakMovement(const Packet& receivedPacket) {
	word_t targetId = receivedPacket.getWord(0x00);
	float newX = receivedPacket.getFloat(0x02);
	float newY = receivedPacket.getFloat(0x06);

	this->getCombatInformation()->getAttackTimer().softStop();

	Entity* target = nullptr;
	if (targetId != 0x00) {
		target = this->getVisuality()->find(targetId);
	}
	this->getCombatInformation()->setTarget(target);

	this->getPositionInformation()->setDestination(Position(newX, newY));

	return true;
}

bool Player::pakTeleport(const Packet& receivedPacket) {
	word_t teleGateId = receivedPacket.getWord(0x00);
	auto gate = this->getPositionInformation()->getMap()->getGate(teleGateId);
	if (gate == nullptr) {
		return false;
	}
	return this->sendTeleport(gate->getDestination().getMapId(), gate->getDestination().getPosition());
}

bool Player::pakLocalChat(const Packet& receivedPacket) {
	String msg = receivedPacket.getString(0x00);
	if (msg[0] == '/') {
		unsigned long amount = 0x00;
		String* ptr = msg.split(' ', &amount);
		SharedArrayPtr<String> split(ptr, amount);
		auto function = GM_FUNCTIONS.getProcessingFunction(split.at(0));
		if (function) {
			function(this, split);
		}
		return true;
	}
	Packet pak(PacketID::World::Response::LOCAL_CHAT);
	pak.addWord(this->getBasicInformation()->getLocalId());
	pak.addString(msg);
	return this->sendToVisible(pak);
}

bool Player::pakChangeStance(const Packet& receivedPacket) {
	byte_t caseId = receivedPacket.getByte(0x00);
	Stance* stance = this->getStats()->getStance();
	switch (caseId) {
	case 0x00:
		if (stance->isWalking()) {
			stance->setRunningStance();
		}
		else if (stance->isRunning()) {
			stance->setWalkingStance();
		}
		break;
	case 0x01:
		if (stance->isSitting()) {
			stance->setRunningStance();
		}
		else if (!stance->isDriving() && !stance->isSitting()) {
			stance->setSittingStance();
		}
	}

	return this->sendCurrentStance();
}

bool Player::pakInitBasicAttack(const Packet& pak) {
	word_t localId = pak.getWord(0x00);

	if (this->getBasicInformation()->getLocalId() == localId) {
		return true; //Do nothing
	}
	auto allEntities = this->getPositionInformation()->getMap()->getAllEntitiesOnMap();
	if (allEntities.count(localId) == 0) {
		this->logger.info(this->getCharacter()->getName() + String(" tried to attack an invalid target."));
		return true;
	}
	Entity* target = allEntities[localId];
	if (target->isMonster()) {
		this->sendMonsterHP(target);
	}
	this->getCombatInformation()->setType(Combat::Type::NORMAL);
	this->getCombatInformation()->setTarget(target);
	return true;
}




void GMService::teleport(Player* executingPlayer, SharedArrayPtr<String>& splitCmd) {
	if (splitCmd.getSize() < 2) {
		return;
	}
	auto worldServer = ROSEServer::getServer<WorldServer>();
	unsigned char mapId = splitCmd.at(1).toByte();
	Map *map = worldServer->getMap(mapId);
	if (map) {
		float x = 520000.0f;
		float y = 520000.0f;
		if (splitCmd.getSize() >= 4) {
			x = static_cast<float>(splitCmd.at(2).toInt()) * 100;
			y = static_cast<float>(splitCmd.at(3).toInt()) * 100;
		}
		executingPlayer->sendTeleport(mapId, Position(x, y));
	}
}

void GMService::currentPosition(Player* cmdExecutor, SharedArrayPtr<String>& cmdAsTokens) {
	const Position& current = cmdExecutor->getPositionInformation()->getCurrent();
	String message = String("Position: ") + String::fromFloat(current.getX() / 100.0f) + String(", ") + String::fromFloat(current.getY() / 100.0f);
	auto sector = cmdExecutor->getBasicInformation()->getSector();
	if (cmdExecutor->getBasicInformation()->getSector() != nullptr) {
		message += String(" @Sector(") + String::fromInt(sector->getId()) + String("): ") + String::fromFloat(sector->getPosition().getX() / 100.0f) + String(", ") +
			String::fromFloat(sector->getPosition().getY() / 100.0f);
	}
	else {
		message += String(" @invalid Sector.");
	}
	cmdExecutor->sendDebugMessage(message);
}

void GMService::setExp(Player* cmdExecutor, SharedArrayPtr<String>& cmdAsTokens) {
	if (cmdAsTokens.getSize() < 2) {
		return;
	}
	dword_t newExp = cmdAsTokens.at(1).toUInt();
	cmdExecutor->getCharacter()->setExperience(newExp);
}

void GMService::setLevel(Player* cmdExecutor, SharedArrayPtr<String>& cmdAsTokens) {
	if (cmdAsTokens.getSize() < 2) {
		return;
	}
	word_t newLevel = cmdAsTokens.at(1).toUShort();
	if (newLevel > 250) {
		newLevel = 250;
	}
	cmdExecutor->getStats()->setLevel(newLevel);
	cmdExecutor->getCharacter()->setExperience(0);
}

void GMService::targetInformation(Player* cmdExecutor, SharedArrayPtr<String>& cmdAsTokens) {
	cmdExecutor->getDebuggingFlags().setPrintTargetPosition(true);
}