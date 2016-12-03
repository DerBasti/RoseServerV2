#include "QSD.h"
#include "..\..\Common\BasicTypes\FunctionBinder.h"
#include "..\Entities\Entity.h"
#include "..\Entities\Player.h"
#include "..\WorldServer.h"

const QSD::Condition::ConditionFunctionPtr QSD::Condition::ConditionMapping[] = {
	&QSD::Condition::selectQuestId,
	&QSD::Condition::checkQuestVar
};

const QSD::Reward::RewardFunctionPtr QSD::Reward::RewardMapping[] = {
	&QSD::Reward::rewardNewQuest
};

QSD::QSD(const word_t id, const String& path, VFS::Entry& entry) : QSD(id, path, entry.getContent()) {

}

QSD::QSD(const word_t id, const String& path, const SharedArrayPtr<char>& data) {
	BufferedFileReader bfr(data, data.getSize());
	dword_t fileVersion = bfr.readDWord();

	this->entryAmount = bfr.readDWord();
	word_t strLen = bfr.readWord();
	String internalName = bfr.readString(strLen);
	this->entries = new Entry*[this->entryAmount];
	for (unsigned int i = 0; i < this->entryAmount; i++) {
		dword_t recordCount = bfr.readDWord();

		strLen = bfr.readWord();
		bfr.addToCaret(strLen);
		this->entries[i] = new QSD::Entry(this, recordCount);
		QSD::Record* previousRecord = nullptr;
		QSD::Record* next = nullptr;
		for (unsigned int j = 0; j < recordCount; j++) {
			bool checkNextTrigger = bfr.readByte() > 0;
			dword_t conditionCount = bfr.readDWord();
			dword_t rewardCount = bfr.readDWord();

			strLen = bfr.readWord();
			String questName = bfr.readString(strLen);

			QSD::Condition** conditions = new QSD::Condition*[conditionCount];
			for (unsigned int k = 0; k < conditionCount; k++) {
				dword_t conditionLength = bfr.readDWord();
				bfr.setCaret(bfr.getCaret() - sizeof(dword_t));

				SharedArrayPtr<char> data = bfr.readBinary(conditionLength);

				conditions[k] = new QSD::Condition(data);
			}
			QSD::Reward** rewards = new QSD::Reward*[rewardCount];
			for (unsigned int k = 0; k < rewardCount; k++) {
				dword_t rewardLength = bfr.readDWord();
				bfr.setCaret(bfr.getCaret() - sizeof(dword_t));
				SharedArrayPtr<char> data = bfr.readBinary(rewardLength);

				rewards[k] = new QSD::Reward(data);
			}
			QSD::Record* newRecord = new QSD::Record(questName, conditions, conditionCount, rewards, rewardCount);
			newRecord->setCheckNextTrigger(checkNextTrigger);
			newRecord->setPreviousRecord(previousRecord);

			if (previousRecord && previousRecord->isNextTriggerRequired()) {
				previousRecord->setNextRecord(newRecord);
			}

			this->entries[i]->getRecords()[j] = previousRecord = newRecord;
		}
	}
}

QSD::~QSD() {
	for (unsigned int i = 0; i < this->getQuestEntryAmount(); i++) {
		delete this->entries[i];
		this->entries[i] = nullptr;
	}
	delete[] this->entries;
	this->entries = nullptr;

	this->entryAmount = 0x00;
}

bool QSD::fireTrigger(const dword_t questHash, Entity* triggerCauser) {
	QuestInformationTransfer qit(triggerCauser);

	auto quest = ROSEServer::getServer<WorldServer>()->getQuestByHash(questHash);
	auto initQuest = quest;
	bool result = true;
	for (byte_t i = 0; i < 2 && result; i++) {
		bool testRun = (i == 0);
		quest = initQuest;
		while (quest && result) {
			//Testrun: check conditions and test whether rewards are applicable
			if (testRun) {
				QSD::Condition** conditions = quest->getConditions();
				for (dword_t j = 0; j < quest->getConditionAmount() && result; j++, conditions++) {
					QSD::Condition* cond = *conditions;
					result &= cond->isConditionFulfilled(&qit);
				}
				if (!result) {
					break;
				}
			}
			QSD::Reward** rewards = quest->getRewards();
			for (unsigned int j = 0; j < quest->getRewardAmount() && result; j++, rewards++) {
				QSD::Reward* reward = *rewards;
				result &= reward->applyReward(&qit, i == 1);
			}
			quest = quest->getNextRecord();
		}
	}
	return quest != nullptr && result;
}

dword_t QSD::createQuestHash(const char* questName) {
	//Thanks to Drakia / Brett19 / ExJam for this particular codepiece
	const unsigned long keys[256] = {
		0x697A5, 0x6045C, 0xAB4E2, 0x409E4, 0x71209, 0x32392, 0xA7292, 0xB09FC, 0x4B658, 0xAAAD5, 0x9B9CF, 0xA326A, 0x8DD12, 0x38150, 0x8E14D, 0x2EB7F,
		0xE0A56, 0x7E6FA, 0xDFC27, 0xB1301, 0x8B4F7, 0xA7F70, 0xAA713, 0x6CC0F, 0x6FEDF, 0x2EC87, 0xC0F1C, 0x45CA4, 0x30DF8, 0x60E99, 0xBC13E, 0x4E0B5,
		0x6318B, 0x82679, 0x26EF2, 0x79C95, 0x86DDC, 0x99BC0, 0xB7167, 0x72532, 0x68765, 0xC7446, 0xDA70D, 0x9D132, 0xE5038, 0x2F755, 0x9171F, 0xCB49E,
		0x6F925, 0x601D3, 0x5BD8A, 0x2A4F4, 0x9B022, 0x706C3, 0x28C10, 0x2B24B, 0x7CD55, 0xCA355, 0xD95F4, 0x727BC, 0xB1138, 0x9AD21, 0xC0ACA, 0xCD928,
		0x953E5, 0x97A20, 0x345F3, 0xBDC03, 0x7E157, 0x96C99, 0x968EF, 0x92AA9, 0xC2276, 0xA695D, 0x6743B, 0x2723B, 0x58980, 0x66E08, 0x51D1B, 0xB97D2,
		0x6CAEE, 0xCC80F, 0x3BA6C, 0xB0BF5, 0x9E27B, 0xD122C, 0x48611, 0x8C326, 0xD2AF8, 0xBB3B7, 0xDED7F, 0x4B236, 0xD298F, 0xBE912, 0xDC926, 0xC873F,
		0xD0716, 0x9E1D3, 0x48D94, 0x9BD91, 0x5825D, 0x55637, 0xB2057, 0xBCC6C, 0x460DE, 0xAE7FB, 0x81B03, 0x34D8F, 0xC0528, 0xC9B59, 0x3D260, 0x6051D,
		0x93757, 0x8027F, 0xB7C34, 0x4A14E, 0xB12B8, 0xE4945, 0x28203, 0xA1C0F, 0xAA382, 0x46ABB, 0x330B9, 0x5A114, 0xA754B, 0xC68D0, 0x9040E, 0x6C955,
		0xBB1EF, 0x51E6B, 0x9FF21, 0x51BCA, 0x4C879, 0xDFF70, 0x5B5EE, 0x29936, 0xB9247, 0x42611, 0x2E353, 0x26F3A, 0x683A3, 0xA1082, 0x67333, 0x74EB7,
		0x754BA, 0x369D5, 0x8E0BC, 0xABAFD, 0x6630B, 0xA3A7E, 0xCDBB1, 0x8C2DE, 0x92D32, 0x2F8ED, 0x7EC54, 0x572F5, 0x77461, 0xCB3F5, 0x82C64, 0x35FE0,
		0x9203B, 0xADA2D, 0xBAEBD, 0xCB6AF, 0xC8C9A, 0x5D897, 0xCB727, 0xA13B3, 0xB4D6D, 0xC4929, 0xB8732, 0xCCE5A, 0xD3E69, 0xD4B60, 0x89941, 0x79D85,
		0x39E0F, 0x6945B, 0xC37F8, 0x77733, 0x45D7D, 0x25565, 0xA3A4E, 0xB9F9E, 0x316E4, 0x36734, 0x6F5C3, 0xA8BA6, 0xC0871, 0x42D05, 0x40A74, 0x2E7ED,
		0x67C1F, 0x28BE0, 0xE162B, 0xA1C0F, 0x2F7E5, 0xD505A, 0x9FCC8, 0x78381, 0x29394, 0x53D6B, 0x7091D, 0xA2FB1, 0xBB942, 0x29906, 0xC412D, 0x3FCD5,
		0x9F2EB, 0x8F0CC, 0xE25C3, 0x7E519, 0x4E7D9, 0x5F043, 0xBBA1B, 0x6710A, 0x819FB, 0x9A223, 0x38E47, 0xE28AD, 0xB690B, 0x42328, 0x7CF7E, 0xAE108,
		0xE54BA, 0xBA5A1, 0xA09A6, 0x9CAB7, 0xDB2B3, 0xA98CC, 0x5CEBA, 0x9245D, 0x5D083, 0x8EA21, 0xAE349, 0x54940, 0x8E557, 0x83EFD, 0xDC504, 0xA6059,
		0xB85C9, 0x9D162, 0x7AEB6, 0xBED34, 0xB4963, 0xE367B, 0x4C891, 0x9E42C, 0xD4304, 0x96EAA, 0xD5D69, 0x866B8, 0x83508, 0x7BAEC, 0xD03FD, 0xDA122
	};
	register unsigned long result = 0xDEADC0DE;
	register unsigned long tkey = 0x7FED7FED;
	register char c = 0;
	while (*(questName))
	{
		c = ::toupper(*questName);
		result += tkey;
		tkey *= 0x21;
		result ^= keys[c];
		tkey += result + c + 3;
		questName++;
	}
	return result;
}

QSD::Record::Record(const String& questName, Condition** cond, const dword_t conditionAmount, Reward** rewards, const dword_t rewardAmount) {
	this->previous = nullptr;
	this->questName = questName;
	this->questHash = QSD::createQuestHash(questName.toConstChar());
	this->conditions = cond;
	this->condAmount = conditionAmount;
	this->rewards = rewards;
	this->rewardAmount = rewardAmount;
}



bool QSD::Condition::selectQuestId(const QSD::Condition* condition, QuestInformationTransfer* qit) {
	if (!qit->getTriggerCauser()->isPlayer()) {
		return false;
	}
	DataInterpreter<char> reader(condition->getData(), condition->getLength());
	word_t questId = static_cast<word_t>(reader.get<dword_t>());

	Player* player = static_cast<Player*>(qit->getTriggerCauser());
	QuestJournal::Entry* questEntry = player->getQuestJournal()->getQuestEntry(questId);

	qit->setSelectedQuest(questEntry);

	return questEntry != nullptr;
}

bool QSD::Condition::checkQuestVar(const QSD::Condition* condition, QuestInformationTransfer* qit) {
	if (!qit->getTriggerCauser()->isPlayer()) {
		return false;
	}
	DataInterpreter<char> reader(condition->getData(), condition->getLength());
	Player* player = static_cast<Player*>(qit->getTriggerCauser());

	dword_t varAmount = reader.get<dword_t>();
	for (dword_t i = 0; i < varAmount; i++) {
		auto data = GetQuestVarData(reader);
		word_t questVar = player->getQuestJournal()->getVarByType(data.varType, data.varNum);
		if (!OperationService::checkOperation(questVar, data.amount, data.operation)) {
			return false;
		}
	}
	return true;
}



bool QSD::Reward::rewardNewQuest(const QSD::Reward* reward, QuestInformationTransfer* qit, bool dryRun) {
	if (!qit->getTriggerCauser()->isPlayer()) {
		return false;
	}
	DataInterpreter<char> reader(reward->getData(), reward->getLength());
	Player* player = static_cast<Player*>(qit->getTriggerCauser());

	word_t questId = static_cast<word_t>(reader.get<dword_t>());
	byte_t operation = reader.get<byte_t>();

	switch (operation) {
		case 0x00: //Delete quest
			if (dryRun) {
				return qit->getSelectedQuest() != nullptr;
			}
			if (!qit->getSelectedQuest()) {
				return false;
			}
			qit->getSelectedQuest()->reset();
		break;
		case 0x01: //Start new quest
			if (dryRun) {
				qit->setDryRunQuestFlag();
				return player->getQuestJournal()->getEmptyEntry() != nullptr;
			}
			player->getQuestJournal()->getEmptyEntry()->reset(questId);
		break;
		case 0x02: //Continue with stats from previous quest
			if (dryRun) {
				qit->setDryRunQuestFlag();
				return true;
			}
			qit->getSelectedQuest()->replaceQuestId(questId);
		break;
		case 0x03://Replace entirely
			
			if (dryRun) {
				qit->setDryRunQuestFlag();
				return true;
			}
			qit->getSelectedQuest()->reset(questId);
		break;
		case 0x04: //Select quest
			if (dryRun) {
				if (qit->isDryRunQuestFlagSet()) {
					return true;
				} 
				return player->getQuestJournal()->selectQuest(questId, true);
			}
			if (!player->getQuestJournal()->selectQuest(questId)) {
				return false;
			}
			qit->setSelectedQuest(player->getQuestJournal()->getCurrentlySelectedQuest());
		break;
	}
	return true;
}
bool QSD::Reward::rewardQuestItem(const QSD::Reward* reward, QuestInformationTransfer* qit, bool dryRun) {
	return false;
}