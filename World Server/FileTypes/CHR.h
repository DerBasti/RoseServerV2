#ifndef __ROSE_CHR__
#define __ROSE_CHR__

#ifdef _MSC_VER
#pragma once
#endif

#include <vector>
#include "ZMO.h"

class CHR {
public:
	class NPCAnimationData {
		private:
			bool enabledFlag;
			word_t npcId;
			String name;

			std::map<word_t, word_t> animationIds;
		public:
			NPCAnimationData(bool enabled) {
				this->enabledFlag = enabled;
			}
			~NPCAnimationData() {}

			__inline bool isEnabled() const {
				return this->enabledFlag;
			}
			__inline word_t getNPCId() const {
				return this->npcId;
			}
			__inline void setNPCId(const word_t id) {
				this->npcId = id;
			}
			__inline const String& getName() const {
				return this->name;
			}
			__inline void setName(const String& name) {
				this->name = name;
			}
			word_t getAnimationByMotion(const word_t motion) const {
				return this->animationIds.at(motion);
			}
			void addAnimationByMotion(const word_t motionType, const word_t idOfAnimation) {
				this->animationIds[motionType] = idOfAnimation;
			}
	};
private:
	std::vector<NPCAnimationData*> npcAnimationData;
	std::vector<ZMO*> zmoFiles;
	String pathInVFS;
public:
	class MotionType {
		private:
			MotionType() {}
			~MotionType() {}
		public:
			const static byte_t WARNING = 0;
			const static byte_t WALK = 1;
			const static byte_t ATTACK = 2;
			const static byte_t HIT = 3;
			const static byte_t DIE = 4;
			const static byte_t RUN = 5;
			const static byte_t STOP = 6;
			const static byte_t SKILL = 7;
			const static byte_t CASTING = 8;
	};
	CHR(const String& path, const VFS::Entry& entry) : CHR(path, entry.getContent()) {}
	CHR(const String& path, const SharedArrayPtr<char>& data);

	virtual ~CHR();

	ZMO* getAttackMotion(const word_t npcId) const;
	ZMO* getCastingMotion(const word_t npcId) const;
	ZMO* getSkillMotion(const word_t npcId) const;

	ZMO* getMotion(const word_t npcId, const byte_t motionType) const;
};

#endif