#ifndef __ROSE_STB__
#define __ROSE_STB__

#include "..\..\Common\BasicTypes\StringWrapper.h"
#include "..\..\Common\BasicTypes\SharedArrayPointer.h"
#include "..\..\Common\BasicTypes\FileHeader.h"

#include "VFS.h"
#include "STL.h"

class STB {
	public:
		class Entry {
			private:
				std::vector<String> content;
			public:
				Entry(BufferedFileReader& bfr, unsigned long columnAmount) {
					this->content.reserve(columnAmount);
					for (unsigned int i = 0; i < columnAmount; i++) {
						unsigned short length = bfr.readWord();
 						this->content.push_back(bfr.readString(length));
					}
				}
				virtual ~Entry() { }
				__inline String get(const unsigned long columnId) const {
					return String(this->content[columnId]);
				}
				__inline void setName(const String& str) {
					this->content[0] = str;
				}
		};
	private:
		std::vector<STB::Entry*> entries;
		String filePath;
	public:
		typedef std::vector<STB::Entry*>::iterator iterator;

		STB(const String& pathInVFS, const SharedArrayPtr<char>& data) : STB(pathInVFS, data, true) {}
		STB(const String& pathInVFS, const SharedArrayPtr<char>& data, bool applySTL) {
			this->filePath = pathInVFS;
			BufferedFileReader bfr(data.get(), data.getSize(), false);
			bfr.setCaret(0x04);

			unsigned long offset = bfr.readDWord();
			unsigned long entryCount = bfr.readDWord() - 1;
			this->entries.reserve(entryCount);

			unsigned long columnCount = bfr.readDWord() - 1;
			bfr.setCaret(offset);
			for (unsigned int i = 0; i < entryCount; i++) {
				this->entries.push_back(new STB::Entry(bfr, columnCount));
			}
			if (applySTL) {
				String STLPath = this->getFilePath().substring(0, this->getFilePath().findLastOf(".STB"));
				STLPath += String("_S.STL");
				auto res = VFS::get()->getEntry(STLPath);
				STL stl(res.getContent());
				for (unsigned int i = 0; i < stl.getEntryAmount(); i++) {
					STL::Entry stlEntry = stl.getEntry(i);
					this->entries[stlEntry.getId()]->setName(stlEntry.getName(STL::Entry::ENGLISH));
				}
			}
		}

		virtual ~STB() {
			std::for_each(this->entries.begin(), this->entries.end(), [&](STB::Entry* entry) {
				delete entry;
				entry = nullptr;
			});
			this->entries.clear();
		}

		iterator begin() {
			return this->entries.begin();
		}

		iterator end() {
			return this->entries.end();
		}

		__inline Entry* getEntry(const unsigned int id) const {
			return (this->entries.size() > id ? this->entries.at(id) : nullptr);
		}
		__inline String getFilePath() const {
			return this->filePath;
		}
		__inline unsigned long getEntryAmount() const {
			return static_cast<unsigned long>(this->entries.size());
		}
};

class NPCSTB : public STB {
private:
	const static word_t NAME_COLUMN = 0x00;
	const static word_t WALKSPEED_COLUMN = 0x02;
	const static word_t SPRINTSPEED_COLUMN = 0x03;
	const static word_t SIZE_COLUMN = 0x04;
	const static word_t LEVEL_COLUMN = 0x07;
	const static word_t HP_COLUMN = 0x08;
	const static word_t ATTACKPOWER_COLUMN = 0x09;
	const static word_t HITRATE_COLUMN = 0x0A;
	const static word_t DEFENSE_COLUMN = 0x0B;
	const static word_t MAGICDEFENSE_COLUMN = 0x0C;
	const static word_t DODGERATE_COLUMN = 0x0D;
	const static word_t ATTACKSPEED_COLUMN = 0x0E;
	const static word_t AI_COLUMN = 0x10;
	const static word_t EXPERIENCE_COLUMN = 0x11;
	const static word_t DROPTABLE_COLUMN = 0x12;
	const static word_t DROPCHANCE_COLUMN = 0x14;
	const static word_t ATTACKRANGE_COLUMN = 0x1A;
	const static word_t AGGRO_COLUMN = 0x1B;
	const static word_t QUEST_STRING_COLUMN = 0x29;
public:
	NPCSTB(const String& pathInVFS, const SharedArrayPtr<char>& data) : NPCSTB(pathInVFS, data, true) {
	}
	NPCSTB(const String& pathInVFS, const SharedArrayPtr<char>& data, bool applySTL) : STB(pathInVFS, data, applySTL) {}
	__inline String getName(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::NAME_COLUMN);
	}
	__inline word_t getWalkSpeed(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::WALKSPEED_COLUMN).toShort();
	}
	__inline word_t getSprintSpeed(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::SPRINTSPEED_COLUMN).toShort();
	}
	__inline word_t getSize(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::SIZE_COLUMN).toShort();
	}
	__inline byte_t getLevel(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::LEVEL_COLUMN).toByte();
	}
	__inline word_t getHPperLevel(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::HP_COLUMN).toShort();
	}
	__inline word_t getAttackpower(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::ATTACKPOWER_COLUMN).toShort();
	}
	__inline word_t getHitrate(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::HITRATE_COLUMN).toShort();
	}
	__inline word_t getDefense(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::DEFENSE_COLUMN).toShort();
	}
	__inline word_t getMagicDefense(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::MAGICDEFENSE_COLUMN).toShort();
	}
	__inline word_t getDodgerate(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::DODGERATE_COLUMN).toShort();
	}
	__inline word_t getAttackspeed(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::ATTACKSPEED_COLUMN).toShort();
	}
	__inline word_t getExperience(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::EXPERIENCE_COLUMN).toShort();
	}
	__inline word_t getDroptableId(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::DROPTABLE_COLUMN).toShort();
	}
	__inline word_t getMoneyChance(const word_t row) const {
		return 100 - this->getEntry(row)->get(NPCSTB::DROPCHANCE_COLUMN).toShort();
	}
	__inline word_t getDropChance(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::DROPCHANCE_COLUMN).toShort();
	}
	__inline word_t getAttackrange(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::ATTACKRANGE_COLUMN).toShort();
	}
	__inline word_t getAIFileId(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::AI_COLUMN).toShort();
	}
	__inline bool isNPCEntry(const word_t row) const {
		return (this->getEntry(row)->get(NPCSTB::AGGRO_COLUMN).toInt() == 999);
	}
	__inline String getQuestName(const word_t row) const {
		return this->getEntry(row)->get(NPCSTB::QUEST_STRING_COLUMN);
	}
};

class ZoneSTB : public STB {
private:
	const static word_t ZONE_COLUMN = 0x01;
	const static word_t NIGHT_ONLY_COLUMN = 0x04;
	const static word_t DAYCYCLE_LENGTH = 0x0D;
	const static word_t MORNING_BEGIN = 0x0E;
	const static word_t NOON_BEGIN = 0x0F;
	const static word_t EVENING_BEGIN = 0x10;
	const static word_t NIGHT_BEGIN = 0x11;
	const static word_t QUEST_STRING = 0x16;
	const static word_t ZONESIZE_COLUMN = 0x19;
public:
	ZoneSTB(const String& pathInVFS, const SharedArrayPtr<char>& data) : ZoneSTB(pathInVFS, data, true) {
	}
	ZoneSTB(const String& pathInVFS, const SharedArrayPtr<char>& data, bool applySTL) : STB(pathInVFS, data, applySTL) {}

	__inline word_t getId(const WORD row) const {
		return row;
	}
	__inline String getZoneFile(const word_t row) const {
		return this->getEntry(row)->get(ZoneSTB::ZONE_COLUMN);
	}
	__inline bool getIsNightOnly(const word_t row) const {
		return this->getEntry(row)->get(ZoneSTB::NIGHT_ONLY_COLUMN).toInt() > 0;
	}
	__inline word_t getDayLength(const word_t row) const {
		return this->getEntry(row)->get(ZoneSTB::DAYCYCLE_LENGTH).toShort();
	}
	__inline word_t getMorningTime(const word_t row) const {
		return this->getEntry(row)->get(ZoneSTB::MORNING_BEGIN).toShort();
	}
	__inline word_t getNoonTime(const word_t row) const {
		return this->getEntry(row)->get(ZoneSTB::NOON_BEGIN).toShort();
	}
	__inline word_t getEveningTime(const word_t row) const {
		return this->getEntry(row)->get(ZoneSTB::EVENING_BEGIN).toShort();
	}
	__inline word_t getNight(const word_t row) const {
		return this->getEntry(row)->get(ZoneSTB::NIGHT_BEGIN).toShort();
	}
	__inline String getQuestString(const word_t row) const {
		return this->getEntry(row)->get(ZoneSTB::QUEST_STRING);
	}

	dword_t getZoneSize(const word_t row) const {
		unsigned long length = this->getEntry(row)->get(ZoneSTB::ZONESIZE_COLUMN).toInt();
		if (length == 0) {
			length = 4000;
		}
		return length;
	}
};

class EquipmentSTB : public STB {
private:
	const static word_t TYPE = 0x04;
	const static word_t PRICE = 0x05;
	const static word_t PRICE_RATE = 0x06;
	const static word_t WEIGHT = 0x07;
	const static word_t QUALITY = 0x08;
	const static word_t REQUIRED_LEVEL = 0x0D;
	const static word_t CRAFTING_STB_REFERENCE = 0x0E;
	const static word_t UNION_POINTS = 0x0F;
	const static word_t STAT_FIRST_TYPE = 0x18;
	const static word_t STAT_FIRST_AMOUNT = 0x19;
	const static word_t STAT_SECOND_TYPE = 0x1A;
	const static word_t STAT_SECOND_AMOUNT = 0x1B;
	const static word_t DURABILITY_MINIMUM = 0x1D;
	const static word_t DEFENSE_PHYISCAL = 0x1F;
	const static word_t DEFENSE_MAGICAL = 0x20;
	const static word_t ATTACK_RANGE = 0x21;
	const static word_t MOVEMENT_SPEED = 0x21;
	const static word_t MOTION_COLUMN = 0x22;
	const static word_t ATTACK_POWER_PHYSICAL = 0x23;
	const static word_t ATTACK_SPEED = 0x24;
	const static word_t ATTACK_POWER_MAGICAL = 0x25;
public:
	EquipmentSTB(const String& pathInVFS, const SharedArrayPtr<char>& data) : EquipmentSTB(pathInVFS, data, true) {
	}
	EquipmentSTB(const String& pathInVFS, const SharedArrayPtr<char>& data, bool applySTL) : STB(pathInVFS, data, applySTL) {}

	__inline unsigned short getType(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::TYPE).toShort();
	}
	__inline unsigned long getPrice(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::PRICE).toInt();
	}
	__inline unsigned short getPriceRate(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::PRICE_RATE).toShort();
	}
	__inline unsigned short getWeight(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::WEIGHT).toShort();
	}
	__inline unsigned short getQuality(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::QUALITY).toShort();
	}
	__inline unsigned short getRequiredLevel(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::REQUIRED_LEVEL).toShort();
	}
	__inline unsigned short getCraftingReferenceId(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::CRAFTING_STB_REFERENCE).toShort();
	}
	__inline unsigned short getUnionPoints(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::UNION_POINTS).toShort();
	}
	__inline unsigned short getFirstStatType(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::STAT_FIRST_TYPE).toShort();
	}
	__inline unsigned short getFirstStatValue(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::STAT_FIRST_AMOUNT).toShort();
	}
	__inline unsigned short getSecondStatType(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::STAT_SECOND_TYPE).toShort();
	}
	__inline unsigned short getSecondStatValue(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::STAT_SECOND_AMOUNT).toShort();
	}
	__inline unsigned short getDurabilityMinimum(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::DURABILITY_MINIMUM).toShort();
	}
	__inline unsigned short getPhysicalDefense(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::DEFENSE_PHYISCAL).toShort();
	}
	__inline unsigned short getMagicalDefense(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::DEFENSE_MAGICAL).toShort();
	}
	__inline unsigned short getAttackRange(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::ATTACK_RANGE).toShort();
	}
	__inline unsigned short getMovementSpeed(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::MOVEMENT_SPEED).toShort();
	}
	__inline unsigned short getMotionId(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::MOTION_COLUMN).toShort();
	}
	__inline unsigned short getPhysicalAttackPower(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::ATTACK_POWER_PHYSICAL).toShort();
	}
	__inline unsigned short getAttackSpeed(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::ATTACK_SPEED).toShort();
	}
	__inline unsigned short getMagicalAttackPower(unsigned short row) const {
		return this->getEntry(row)->get(EquipmentSTB::ATTACK_POWER_MAGICAL).toShort();
	}
};

class SkillCost {
private:
	unsigned short type[2];
	unsigned short amount[2];
	__inline unsigned short getType(bool second) const {
		return this->type[static_cast<unsigned long>(second)];
	}
	__inline unsigned short getAmount(bool second) const {
		return this->amount[static_cast<unsigned long>(second)];
	}
public:
	SkillCost() : SkillCost(0, 0, 0, 0) {}
	SkillCost(unsigned short type1, unsigned short type2, unsigned short amount1, unsigned short amount2) {
		this->type[0] = type1;
		this->type[1] = type2;

		this->amount[0] = amount1;
		this->amount[1] = amount2;
	}

	__inline unsigned short getFirstCostType() const {
		return this->getType(false);
	}
	__inline unsigned short getSecondCostType() const {
		return this->getType(true);
	}
	__inline unsigned short getFirstCostAmount() const {
		return this->getAmount(false);
	}
	__inline unsigned short getSecondCostAmount() const {
		return this->getAmount(true);
	}
};

class SkillBuff {
private:
	unsigned short buffType;
	unsigned short flatValue;
	unsigned short percentValue;

public:
	const static byte_t MAX_PER_SKILL = 3;
	SkillBuff() : SkillBuff(0, 0, 0) {}
	SkillBuff(unsigned short type, unsigned short flat, unsigned short percent) {
		this->buffType = type;
		this->flatValue = flat;
		this->percentValue = percent;
	}

	__inline unsigned short getType() const {
		return this->buffType;
	}
	__inline unsigned short getFlatValue() const {
		return this->flatValue;
	}
	__inline unsigned short getPercentValue() const {
		return this->percentValue;
	}
};

class SkillRequirement {
private:
	unsigned short skillBasicId;
	unsigned char skillLevel;
public:
	SkillRequirement() : SkillRequirement(0, 0) {}
	SkillRequirement(unsigned short basicId, unsigned skillLevel) {
		this->skillBasicId = basicId;
		this->skillLevel = skillLevel;
	}
	__inline unsigned short getBasicId() const {
		return this->skillBasicId;
	}
	__inline unsigned char getLevel() const {
		return this->skillLevel;
	}
	__inline bool isValid() const {
		return this->getBasicId() != 0 && this->getLevel() > 0;
	}

};

class SkillSTB : public STB {
private:
	const static byte_t CONDITIONS_MAX_NUM = 0x02;
	const static byte_t COSTS_MAX_NUM = 0x02;
	const static byte_t BUFF_MAX_NUM = 0x03;
	const static byte_t REQUIRED_SKILL_MAX_NUM = 0x03;
	const static byte_t CLASS_MAX_NUM = 0x04;
	const static byte_t WEAPONS_MAX_NUM = 0x05;

	const static byte_t COLUMN_BASIC_ID = 0x01;
	const static byte_t COLUMN_LEVEL = 0x02;
	const static byte_t COLUMN_REQUIRED_POINTS_PER_LEVELUP = 0x03;
	const static byte_t COLUMN_SKILLTYPE = 0x04;
	const static byte_t COLUMN_INITRANGE = 0x06;
	const static byte_t COLUMN_TARGETTYPE = 0x07;
	const static byte_t COLUMN_AOERANGE = 0x08;
	const static byte_t COLUMN_ATTACKPOWER = 0x09;
	const static byte_t COLUMN_DOESHARM = 0x0A;
	const static byte_t COLUMN_STATUS_FIRST = 0x0B;
	const static byte_t COLUMN_STATUS_SECOND = 0x0C;
	const static byte_t COLUMN_SUCCESSRATE = 0x0D;
	const static byte_t COLUMN_DURATION = 0x0E;
	const static byte_t COLUMN_COST_TYPE_FIRST = 0x10;
	const static byte_t COLUMN_COST_TYPE_SECOND = 0x12;
	const static byte_t COLUMN_COST_AMOUNT_FIRST = 0x11;
	const static byte_t COLUMN_COST_AMOUNT_SECOND = 0x13;
	const static byte_t COLUMN_COOLDOWN = 0x14;
	const static byte_t COLUMN_BUFF_TYPE_FIRST = 0x15;
	const static byte_t COLUMN_BUFF_TYPE_SECOND = 0x18;
	const static byte_t COLUMN_BUFF_TYPE_LAST = 0x1B;
	const static byte_t COLUMN_BUFF_FLATVALUE_FIRST = 0x16;
	const static byte_t COLUMN_BUFF_FLATVALUE_SECOND = 0x19;
	const static byte_t COLUMN_BUFF_FLATVALUE_LAST = 0x1C;
	const static byte_t COLUMN_BUFF_PERCENTVALUE_FIRST = 0x17;
	const static byte_t COLUMN_BUFF_PERCENTVALUE_SECOND = 0x1A;
	const static byte_t COLUMN_BUFF_PERCENTVALUE_LAST = 0x1D;
	const static byte_t COLUMN_WEAPONS_BEGIN = 0x1E;
	const static byte_t COLUMN_WEAPONS_END = 0x22;
	const static byte_t COLUMN_CLASS_BEGIN = 0x23;
	const static byte_t COLUMN_CLASS_END = 0x26;
	const static byte_t COLUMN_REQUIRED_SKILL_ID_FIRST = 0x27;
	const static byte_t COLUMN_REQUIRED_SKILL_ID_SECOND = 0x29;
	const static byte_t COLUMN_REQUIRED_SKILL_ID_LAST = 0x2B;

	const static byte_t COLUMN_REQUIRED_SKILL_LEVEL_FIRST = 0x28;
	const static byte_t COLUMN_REQUIRED_SKILL_LEVEL_SECOND = 0x2A;
	const static byte_t COLUMN_REQUIRED_SKILL_LEVEL_LAST = 0x2C;
	const static byte_t REQUIRED_SKILL_MAX = 0x03;

	const static byte_t COLUMN_REQUIRED_CONDITION_TYPE_FIRST = 0x2D;
	const static byte_t COLUMN_REQUIRED_CONDITION_TYPE_LAST = 0x30;
	const static byte_t COLUMN_REQUIRED_CONDITION_AMOUNT_FIRST = 0x2E;
	const static byte_t COLUMN_REQUIRED_CONDITION_AMOUNT_LAST = 0x31;

public:
	SkillSTB(const String& pathInVFS, const SharedArrayPtr<char>& data) : SkillSTB(pathInVFS, data, true) {
	}
	SkillSTB(const String& pathInVFS, const SharedArrayPtr<char>& data, bool applySTL) : STB(pathInVFS, data, applySTL) {}

	__inline unsigned short getBasicId(const word_t row) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_BASIC_ID).toShort();
	}
	__inline byte_t getLevel(const word_t row) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_LEVEL).toByte();
	}
	__inline unsigned short getRequiredPointsPerLevelup(const word_t row) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_REQUIRED_POINTS_PER_LEVELUP).toShort();
	}
	__inline unsigned short getSkillType(const word_t row) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_SKILLTYPE).toShort();
	}
	__inline unsigned long getInitRange(const word_t row) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_INITRANGE).toInt();
	}
	__inline unsigned short getTargetType(const word_t row) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_TARGETTYPE).toShort();
	}
	__inline unsigned long getAOERange(const word_t row) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_AOERANGE).toInt();
	}
	__inline unsigned short getAttackPower(const word_t row) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_ATTACKPOWER).toShort();
	}
	__inline bool isHarmfulEffect(const word_t row) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_DOESHARM).toShort() > 0;
	}
	__inline unsigned short getStatusId(const word_t row, bool secondInsteadOfFirstStatus) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_STATUS_FIRST + static_cast<unsigned long>(secondInsteadOfFirstStatus)).toShort();
	}
	__inline unsigned short getSuccessRate(const word_t row) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_SUCCESSRATE).toShort();
	}
	__inline unsigned short getDuration(const word_t row) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_DURATION).toShort();
	}
	__inline SkillCost getSkillCost(const word_t row) const {
		return SkillCost(this->getEntry(row)->get(COLUMN_COST_TYPE_FIRST).toShort(),
			this->getEntry(row)->get(COLUMN_COST_TYPE_SECOND).toShort(),
			this->getEntry(row)->get(COLUMN_COST_AMOUNT_FIRST).toShort(),
			this->getEntry(row)->get(COLUMN_COST_AMOUNT_SECOND).toShort());
	}
	//buffOffset can be between 0 and 2
	SkillBuff getBuff(const word_t row, const byte_t buffOffset) const {
		unsigned short colId = COLUMN_BUFF_TYPE_FIRST + (buffOffset * 3);
		return SkillBuff(this->getEntry(row)->get(colId).toShort(),
			this->getEntry(row)->get(colId + 1).toShort(),
			this->getEntry(row)->get(colId + 2).toShort());
	}
	//weaponOffset can be between 0 and 4
	__inline unsigned short getAllowedWeapons(const word_t row, const byte_t weaponOffset) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_WEAPONS_BEGIN + weaponOffset).toShort();
	}
	//classOffset can be between 0 and 4
	__inline unsigned short getAllowedClasses(const word_t row, const byte_t classOffset) const {
		return this->getEntry(row)->get(SkillSTB::COLUMN_CLASS_BEGIN + classOffset).toShort();
	}
	//reqOffset can be between 0 and 2
	__inline SkillRequirement getSkillingRequirements(const word_t row, const byte_t reqOffset) const {
		return SkillRequirement(this->getEntry(row)->get(SkillSTB::COLUMN_REQUIRED_SKILL_ID_FIRST + (reqOffset * 2)).toShort(),
			this->getEntry(row)->get(SkillSTB::COLUMN_REQUIRED_SKILL_LEVEL_FIRST + (reqOffset * 2)).toShort());
	}
};

class ConsumableSTB : public STB {
private:
	const static word_t EXECUTION_TYPE = 0x07;
	const static word_t STAT_TYPE_NEEDED = 0x11;
	const static word_t STAT_VALUE_NEEDED = 0x12;

	const static word_t STAT_TYPE_ADD = 0x13;
	const static word_t STAT_VALUE_ADD = 0x14;
	const static word_t STATUS_STB_REFERENCE = 0x18;
public:
	ConsumableSTB(const String& pathInVFS, const SharedArrayPtr<char>& data) : ConsumableSTB(pathInVFS, data, true) {
	}
	ConsumableSTB(const String& pathInVFS, const SharedArrayPtr<char>& data, bool applySTL) : STB(pathInVFS, data, applySTL) {}

	__inline unsigned short getExecutionType(const word_t row) const {
		return this->getEntry(row)->get(ConsumableSTB::EXECUTION_TYPE).toShort();
	}
	__inline unsigned short getNecessaryStatType(const word_t row) const {
		return this->getEntry(row)->get(ConsumableSTB::STAT_TYPE_NEEDED).toShort();
	}
	__inline unsigned short getNecessaryStatAmount(const word_t row) const {
		return this->getEntry(row)->get(ConsumableSTB::STAT_VALUE_NEEDED).toShort();
	}
	__inline unsigned short getStatTypeAddition(const word_t row) const {
		return this->getEntry(row)->get(ConsumableSTB::STAT_TYPE_ADD).toShort();
	}
	__inline unsigned short getStatValueAddition(const word_t row) const {
		return this->getEntry(row)->get(ConsumableSTB::STAT_VALUE_ADD).toShort();
	}
	__inline unsigned short getStatusReference(const word_t row) const {
		return this->getEntry(row)->get(ConsumableSTB::STATUS_STB_REFERENCE).toShort();
	}
};


class StatusSTB : public STB {
private:
	const static word_t COLUMN_IS_STATUS_STACKABLE = 0x02;
	const static word_t COLUMN_BUFF_OR_DEBUFF = 0x03;
	const static word_t COLUMN_VALUE_INCREASE_FIRST = 0x06;
	const static word_t COLUMN_VALUE_INCREASE_SECOND = 0x08;
public:
	StatusSTB(const String& pathInVFS, const SharedArrayPtr<char>& data) : StatusSTB(pathInVFS, data, true) {
	}
	StatusSTB(const String& pathInVFS, const SharedArrayPtr<char>& data, bool applySTL) : STB(pathInVFS, data, applySTL) {}

	__inline bool isStatusStackable(const word_t row) const {
		return this->getEntry(row)->get(StatusSTB::COLUMN_IS_STATUS_STACKABLE).toInt() > 0;
	}
	__inline bool isBuff(const word_t row) const {
		return this->getEntry(row)->get(StatusSTB::COLUMN_BUFF_OR_DEBUFF).toInt() == 0;
	}
	__inline unsigned long getFirstValueIncrease(const word_t row) const {
		return this->getEntry(row)->get(StatusSTB::COLUMN_VALUE_INCREASE_FIRST).toInt();
	}
	__inline unsigned long getSecondValueIncrease(const word_t row) const {
		return this->getEntry(row)->get(StatusSTB::COLUMN_VALUE_INCREASE_SECOND).toInt();
	}
};

class AISTB : public STB {
private:
	const static word_t PATH_COLUMN = 0x00;
public:
	AISTB(const String& pathInVFS, const SharedArrayPtr<char>& data) : AISTB(pathInVFS, data, true) {
	}
	AISTB(const String& pathInVFS, const SharedArrayPtr<char>& data, bool applySTL) : STB(pathInVFS, data, applySTL) {}

	__inline String getAIPath(const word_t row) const {
		return this->getEntry(row)->get(AISTB::PATH_COLUMN);
	}
};

#endif 