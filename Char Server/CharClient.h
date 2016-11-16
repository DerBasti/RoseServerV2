#pragma once

#ifndef __ROSE_CHAR_CLIENT__
#define __ROSE_CHAR_CLIENT__

#include "..\Common\ROSESocketClient.h"

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

		byte_t charAmount;
		Character* characters;

		bool pakIdentifyAccount();
		bool pakGetCharacters();
		void getCharactersFromResult(ResultSet* rs);
		bool pakCreateCharacter();
		bool pakDeleteCharacter();
		bool pakGetWorldserverIp();
		bool pakUnknown();

		__inline byte_t getCharacterAmount() const {
			return this->charAmount;
		}
	public:
		CharClient(NetworkInterface* iFace, const CryptInfo& cryptInfo) : ROSESocketClient(iFace, cryptInfo) {
			this->characters = new Character[5];
		}
		~CharClient();

		bool handlePacket();
};

#endif //__ROSE_CHAR_CLIENT__