#include "CHR.h"

CHR::CHR(const String& path, const SharedArrayPtr<char>& data) {
	BufferedFileReader bfr(data, data.getSize());
	word_t skeletonAmount = bfr.readWord();
	for (word_t i = 0; i < skeletonAmount; i++) {
		bfr.readString();
	}
	word_t motionAmount = bfr.readWord();
	for (word_t i = 0; i < motionAmount; i++) {
		String motionPath = bfr.readString(); 
		this->zmoFiles.push_back(new ZMO(motionPath, VFS::get()->getEntry(motionPath)));
	}
	word_t effectAmount = bfr.readWord();
	for (word_t i = 0; i < effectAmount; i++) {
		bfr.readString();
	}
	word_t npcAmount = bfr.readWord();
	for (word_t i = 0; i < npcAmount; i++) {
		bool isEnabled = bfr.readByte() > 0;
		NPCAnimationData* nad = new NPCAnimationData(isEnabled);
		if (isEnabled) {
			nad->setNPCId(bfr.readWord());
			nad->setName(bfr.readString());
			word_t objectAmount = bfr.readWord();
			for (word_t j = 0; j < objectAmount; j++) {
				bfr.readWord(); //...
			}
			word_t animationAmount = bfr.readWord();
			for (word_t j = 0; j < animationAmount; j++) {
				word_t type = bfr.readWord();
				word_t animId = bfr.readWord();
				nad->addAnimationByMotion(type, animId);
			}
			word_t effectAmount = bfr.readWord();
			for (word_t j = 0; j < effectAmount; j++) {
				bfr.readWord(); //...
			}
		}
		this->npcAnimationData.push_back(nad);
	}
}

CHR::~CHR() {
	for (unsigned int i = 0; i < this->npcAnimationData.size(); i++) {
		delete this->npcAnimationData[i];
		this->npcAnimationData[i] = nullptr;
	}
	this->npcAnimationData.clear();

	for (unsigned int i = 0; i < this->zmoFiles.size(); i++) {
		delete this->zmoFiles[i];
		this->zmoFiles[i] = nullptr;
	}
	this->zmoFiles.clear();
}

ZMO* CHR::getAttackMotion(const word_t npcId) const {
	return this->npcAnimationData[npcId]->isEnabled() ? this->zmoFiles[this->npcAnimationData[npcId]->getAnimationByMotion(CHR::MotionType::ATTACK)] : nullptr;
}

ZMO* CHR::getCastingMotion(const word_t npcId) const {
	return this->npcAnimationData[npcId]->isEnabled() ? this->zmoFiles[this->npcAnimationData[npcId]->getAnimationByMotion(CHR::MotionType::CASTING)] : nullptr;
}

ZMO* CHR::getSkillMotion(const word_t npcId) const {
	return this->npcAnimationData[npcId]->isEnabled() ? this->zmoFiles[this->npcAnimationData[npcId]->getAnimationByMotion(CHR::MotionType::SKILL)] : nullptr;
}

ZMO* CHR::getMotion(const word_t npcId, const byte_t motionType) const {
	return this->npcAnimationData[npcId]->isEnabled() ? this->zmoFiles[this->npcAnimationData[npcId]->getAnimationByMotion(motionType)] : nullptr;
}