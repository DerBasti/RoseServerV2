#pragma once

#ifndef __COMMON_DATATYPES__
#define __COMMON_DATATYPES__

#include <iostream>
#include <string>
#include <xutility>
#include <xmemory>
#include <vector>
#include "..\Common\BasicTypes\BasicObject.h"
#include "..\Common\DatabaseQueries.h"
#include "..\Common\BasicTypes\Database.h"
#include "..\Common\BasicTypes\Observable.h"

#include "Definitions.h"

class Packet {
public:
	const static dword_t DEFAULT_DATA_MAX = 0x400;
	const static dword_t DEFAULT_HEADER_OFFSET = 0x06;
private:
	word_t length;
	word_t command;
	word_t notUsed;
	char data[Packet::DEFAULT_DATA_MAX];
public:
	Packet() {
		memset(this->data, 0x00, Packet::DEFAULT_DATA_MAX);
		this->length = DEFAULT_HEADER_OFFSET;
	}

	Packet(std::nullptr_t) = delete;
	explicit Packet(const char* packetData) {
		if (packetData != nullptr) {
			memcpy(&this->length, packetData, sizeof(word_t));
			memcpy(&this->command, &packetData[2], sizeof(word_t));
			memcpy(&this->notUsed, &packetData[4], sizeof(word_t));

			unsigned int lengthWithoutHeader = (this->length - Packet::DEFAULT_HEADER_OFFSET);
			if (lengthWithoutHeader > 0) {
				memcpy(this->data, &packetData[Packet::DEFAULT_HEADER_OFFSET],
					lengthWithoutHeader < Packet::DEFAULT_DATA_MAX ? lengthWithoutHeader : Packet::DEFAULT_DATA_MAX);
			}
		}
	}

	Packet(word_t command, word_t newSize = Packet::DEFAULT_HEADER_OFFSET) {
		this->newPacket(command, newSize);
	}
	Packet& operator=(std::nullptr_t) {
		this->length = this->command = this->notUsed = 0;
		memset(this->data, 0x00, Packet::DEFAULT_DATA_MAX);
		return (*this);
	}
	Packet& operator=(const Packet& p) {
		this->length = p.length;
		this->command = p.command;
		this->notUsed = p.notUsed;
		memcpy(this->data, p.data, this->length - Packet::DEFAULT_HEADER_OFFSET);
		return (*this);
	}

	__inline const char* toConstChar() {
		return reinterpret_cast<const char*>(this); 
	}
	__inline unsigned char* toUnsignedChar() {
		return reinterpret_cast<unsigned char*>(this);
	}

	const unsigned char* fromConst() const { return reinterpret_cast<const unsigned char*>(this); }

	void newPacket(word_t command, word_t newSize = Packet::DEFAULT_HEADER_OFFSET) {
		this->command = command;
		this->length = newSize;
		this->notUsed = 0x00;

		memset(this->data, 0x00, Packet::DEFAULT_DATA_MAX);
	}
	__inline void adjustLength(const unsigned int len) {
		this->length = len;
	}
	void addByte(const byte_t toAdd) {
		this->data[this->length - Packet::DEFAULT_HEADER_OFFSET] = toAdd;
		this->length++;
	}
	byte_t getByte(const word_t position) {
		if (position < this->getLength())
			return this->data[position];
		return static_cast<BYTE>(-1);
	}

	void addWord(const word_t toAdd) {
		*reinterpret_cast<WORD*>(&this->data[this->length - Packet::DEFAULT_HEADER_OFFSET]) = toAdd;
		this->length += sizeof(WORD);
	}
	word_t getWord(const word_t position) {
		if (position < this->getLength())
			return *(reinterpret_cast<WORD*>(&this->data[position]));
		return static_cast<WORD>(-1);
	}

	void addDWord(const dword_t toAdd) {
		*reinterpret_cast<DWORD*>(&this->data[this->length - Packet::DEFAULT_HEADER_OFFSET]) = toAdd;
		this->length += sizeof(DWORD);
	}
	dword_t getDWord(const word_t position) {
		if (position < this->getLength())
			return *(reinterpret_cast<DWORD*>(&this->data[position]));
		return static_cast<DWORD>(-1);
	}

	void addQWord(const qword_t toAdd) {
		*reinterpret_cast<qword_t*>(&this->data[this->length - Packet::DEFAULT_HEADER_OFFSET]) = toAdd;
		this->length += sizeof(qword_t);
	}
	qword_t getQWord(const word_t position) {
		if (position < this->getLength())
			return *(reinterpret_cast<qword_t*>(&this->data[position]));
		return static_cast<qword_t>(-1);
	}

	void addFloat(const float toAdd) {
		*reinterpret_cast<float*>(&this->data[this->length - Packet::DEFAULT_HEADER_OFFSET]) = toAdd;
		this->length += sizeof(float);
	}
	float getFloat(const word_t position) {
		if (position  < this->getLength())
			return *(reinterpret_cast<float*>(&this->data[position]));
		return static_cast<float>(-1);
	}

	void addString(const String& toAdd) {
		return this->addString(toAdd.toConstChar());
	}
	void addString(const char* toAdd) {
		if (!toAdd) {
			return;
		}
		dword_t len = strlen(toAdd);
		for (unsigned int i = 0; i < len; i++) {
			this->addByte(toAdd[i]);
		}
		this->addByte(0x00); //Termination of string
	}

	char* getString(word_t position) { return &this->data[position]; }

	word_t getLength() const { return this->length; }
	word_t getCommand() const { return this->command; }

	const char* getData() const { return this->data; }
};


class ItemType {
	private:
		ItemType() { };
		~ItemType() { };

		//Defined at WorldServer.cpp
		static const char *typeNames[];
	public:
		const static byte_t FACE = 1;
		const static byte_t HEADGEAR = 2;
		const static byte_t ARMOR = 3;
		const static byte_t GLOVES = 4;
		const static byte_t SHOES = 5;
		const static byte_t BACK = 6;
		const static byte_t JEWELRY = 7;
		const static byte_t WEAPON = 8;
		const static byte_t SHIELD = 9;
		const static byte_t CONSUMABLES = 10;
		const static byte_t JEWELS = 11;
		const static byte_t OTHER = 12;
		const static byte_t QUEST = 13;
		const static byte_t PAT = 14;
		const static byte_t MONEY = 31; //6 Bits
};

class Item {
private:
	byte_t type;
	word_t id;
	bool appraiseFlag;
	bool socketFlag;
	word_t lifespan;
	Observable<dword_t> amount;
	byte_t durability;
	word_t refine;
	word_t gem;
	word_t stats;
public:
	Item() : Item(0, 0) {}
	Item(const byte_t type, const word_t id) : Item(type, id, 1) {}
	Item(const byte_t type, const word_t id, const dword_t amount) {
		this->clear();
		this->amount = amount;
		this->amount.setOnNewValueAssigned([&](const dword_t) {
			if (this->amount.getValue() == 0) {
				this->clear();
			}
		});
		this->type = type;
		this->id = id;
	}

	const dword_t getVisualityData() const {
		dword_t basicResult = (this->id | this->refine * 0x10000);
		if (this->gem != 0) {
			basicResult |= 0xd0000 + ((this->gem - 320) * 0x400);
		}
		return basicResult;
	}

	const word_t getPacketHeader() const {
		if (this->amount == 0x00) {
			return 0;
		}
		word_t result = static_cast<word_t>((this->id << 5) & 0xFFE0);
		return static_cast<word_t>(result | (this->type & 0x1F));
	}
	const dword_t getPacketData() const {
		if ((this->getType() >= ItemType::CONSUMABLES && this->getType() <= ItemType::QUEST) || this->getType() == ItemType::MONEY || this->amount.getValue() == 0) {
			return this->amount.getValue();
		}

		//0101 1111 1001 0000
		dword_t refinePart = (this->refine >> 4) << 28;
		dword_t appraisePart = this->isAppraised() << 27;
		dword_t socketPart = this->isSocketed() << 26;
		dword_t lifeSpanPart = this->getLifeSpan() << 16;
		dword_t durabilityPart = this->getDurability() << 9;
		dword_t stats = this->stats;
		dword_t gem = this->gem;
		if (gem != 0x00) {
			stats = 0x00;
		}

		return (refinePart | appraisePart | socketPart | lifeSpanPart | durabilityPart | stats | gem);
	}

	__inline bool isValid() const {
		return this->getType() != 0 && this->getId() > 0 && this->getAmount() > 0;
	}

	__inline word_t getId() const {
		return this->id;
	}
	__inline byte_t getType() const {
		return this->type;
	}
	__inline bool isAppraised() const {
		return this->appraiseFlag;
	}
	__inline void setIsAppraised(bool flag) {
		this->appraiseFlag = flag;
	}
	__inline bool isSocketed() const {
		return this->socketFlag;
	}
	__inline void setIsSocketed(bool flag) {
		this->socketFlag = flag;
	}

	__inline word_t getLifeSpan() const {
		return this->lifespan;
	}
	__inline void setLifeSpan(const word_t newSpan) {
		this->lifespan = newSpan;
	}
	__inline dword_t getAmount() const {
		return this->amount.getValue();
	}
	__inline void setAmount(const dword_t newAmount) {
		this->amount = newAmount;
	}
	__inline byte_t getDurability() const {
		return this->durability;
	}
	__inline void setDurability(const byte_t newDura) {
		this->durability = newDura;
	}

	void clear() {
		this->type = 0x00;
		this->id = 0x00;
		this->appraiseFlag = this->socketFlag = false;
		this->durability = 0x30;
		this->lifespan = 1000;
		this->refine = this->gem = this->stats = 0x00;
		if (this->amount > 0) {
			this->amount = 0x00;
		}
	}

};

#endif //__COMMON_DATATYPES__