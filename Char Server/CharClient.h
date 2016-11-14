#pragma once

#ifndef __ROSE_CHAR_CLIENT__
#define __ROSE_CHAR_CLIENT__

#include "..\Common\ROSESocketClient.h"
#include "D:\Programmieren\QuickInfos\LinkedList.h"

struct Item {
	DWORD owner;
	BYTE itemType;
	WORD itemId;
	BYTE refineLevel;
	BYTE durability;
	BYTE lifespan;

	Item() {
		this->owner = 0x00;
		this->itemId = 0x00;
		this->itemType = this->refineLevel = this->durability = this->lifespan = 0x00;
	}
	Item(unsigned int itemType, unsigned int itemId) {
		this->itemType = itemType;
		this->itemId = itemId;
		this->durability = 30;
	}

	__inline bool operator==(const Item& rhs) const {
		return owner == rhs.owner &&
			itemType == rhs.itemType &&
			itemId == rhs.itemId &&
			refineLevel == rhs.refineLevel &&
			durability == rhs.durability &&
			lifespan == rhs.lifespan;
	}
	bool operator!=(const Item& rhs) {
		return !(operator==(rhs));
	}

	__inline unsigned long toUniqueId() const {
		return (this->itemType * 10000) + this->itemId;
	}
};

struct Character {
	DWORD id;
	String name;
	WORD level;
	WORD classId;
	DWORD faceStyle;
	DWORD hairStyle;
	BYTE sex;
	DWORD deleteTime;

	Item equipment[10];

	Character& operator=(const Character& rhs) {
		this->id = rhs.id;
		this->name = rhs.name;
		this->level = rhs.level;
		this->classId = rhs.classId;
		this->faceStyle = rhs.faceStyle;
		this->hairStyle = rhs.hairStyle;
		this->sex = rhs.sex;
		this->deleteTime = rhs.deleteTime;
		for (unsigned int i = 0; i < 10; i++)
			this->equipment[i] = rhs.equipment[i];
		
		return (*this);
	}

	bool operator==(const Character& rhs) {
		if (this->id == rhs.id &&
			this->name.contentEquals(rhs.name) &&
			this->level == rhs.level &&
			this->classId == rhs.classId &&
			this->faceStyle == rhs.faceStyle &&
			this->hairStyle == rhs.hairStyle &&
			this->sex == rhs.sex &&
			this->deleteTime == rhs.deleteTime) {
				
			for (unsigned int j = 1; j < 10; j++) {
				if (this->equipment[j] != rhs.equipment[j])
					return false;
			}
			return true;
		}
		return false;
	}
	bool operator!=(const Character& rhs) {
		return !(operator==(rhs));
	}
};


class CharClient : public ROSESocketClient {
	private:
		struct _accountInfo {
			DWORD id;
			String userName;
			String password;
			bool isLoggedIn;
		} accountInfo;
		struct _characterInfo {
			DWORD id;
			String name;

			_characterInfo() {
				id = 0x00;
				name = String();
			}

		} selectedCharacter;
		FixedArray<Character> characters;

		bool pakIdentifyAccount();
		bool pakGetCharacters();
		void getCharactersFromResult(ResultSet* rs);
		bool pakCreateCharacter();
		bool pakDeleteCharacter();
		bool pakGetWorldserverIp();
		bool pakUnknown();
	public:
		CharClient(NetworkInterface* iFace, const CryptInfo& cryptInfo) : ROSESocketClient(iFace, cryptInfo) {
			this->characters.reserve(5);
		}
		~CharClient();

		bool handlePacket();
};

#endif //__ROSE_CHAR_CLIENT__