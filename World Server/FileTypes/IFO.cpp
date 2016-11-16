#include "IFO.h"

#ifndef PI
#define PI 3.14159f
#endif

IFO::IFO(const String& path, const SharedArrayPtr<char>& data) {
	BufferedFileReader bfr(data, data.getSize());

	dword_t blockAmount = bfr.readDWord();
	for (dword_t i = 0; i < blockAmount; i++) {
		dword_t type = bfr.readDWord();
		dword_t offset = bfr.readDWord();
		if (type != IFO::Entry::NPC_ENTRY && type != IFO::Entry::SPAWN_ENTRY && type != IFO::Entry::TELEGATE_ENTRY) {
			continue;
		}
		dword_t oldOffset = bfr.getCaret();
		bfr.setCaret(offset);

		dword_t typeAmount = bfr.readDWord();
		for (dword_t j = 0; j < typeAmount; j++) {
			switch (type) {
				case IFO::Entry::NPC_ENTRY:
					this->npcs.push_back(IFO::NPC(bfr));
				break;
				case IFO::Entry::SPAWN_ENTRY:
					this->spawns.push_back(IFO::Spawn(bfr));
				break;
				case IFO::Entry::TELEGATE_ENTRY:
					this->telegates.push_back(IFO::Telegate(bfr));
				break;
			}
		}

		bfr.setCaret(oldOffset);
	}
}

IFO::~IFO() {

}

IFO::Entry::Entry(BufferedFileReader& bfr) {
	byte_t len = bfr.readByte();
	String str = bfr.readString(len);
	bfr.setCaret(bfr.getCaret() + (sizeof(word_t)* 2)); //Unknown + EventId
	this->objectType = bfr.readDWord();
	this->objectId = bfr.readDWord();
	bfr.setCaret(bfr.getCaret() + (sizeof(dword_t)* 2)); //MapPosX + MapPosY
	float quats[4] = { 0.0f };
	for (dword_t i = 0; i < 4; i++) {
		quats[i] = bfr.readFloat();
	}
	float pos[3] = { 0.0f };
	for (dword_t i = 0; i < 3; i++) {
		pos[i] = bfr.readFloat();
	}
	for (dword_t i = 0; i < 3; i++) {
		//Scale
		bfr.readFloat();
	}

	float qw2 = pow(quats[0], 2);
	float qx2 = pow(quats[1], 2);
	float qy2 = pow(quats[2], 2);
	float qz2 = pow(quats[3], 2);
	float rad = atan2(quats[3] * quats[2], qz2 - qy2);

	this->direction = rad * 180.0f / PI;
	if (this->direction < 0.0f) {
		this->direction += 360.0f;
	}

	this->position = Position(pos[0] + 520000.0f, pos[1] + 520000.0f);
}


IFO::Entry::~Entry() {}


IFO::NPC::NPC(BufferedFileReader& bfr) : IFO::Entry(bfr) {
	dword_t unknown = bfr.readDWord();

	byte_t strLen = bfr.readByte();
	this->conFile = bfr.readString(strLen);
}
IFO::NPC::~NPC() {}


IFO::Spawn::Spawn(BufferedFileReader& bfr) : IFO::Entry(bfr) {
	byte_t len = bfr.readByte();
	this->name = bfr.readString(len);

	//First Basic spawns, then tactical ones
	for (unsigned int i = 0; i < 2; i++) {
		dword_t amount = bfr.readDWord();
		for (dword_t j = 0; j < amount; j++) {
			len = bfr.readByte();
			String roundName = bfr.readString(len);
			dword_t mobId = bfr.readDWord();
			dword_t amount = bfr.readDWord();

			Round newRound(roundName, static_cast<word_t>(mobId), static_cast<word_t>(amount), i == 1);

			this->allRounds.push_back(newRound);
		}
	};

	this->respawnInterval = bfr.readDWord();
	this->maxAmount = bfr.readDWord();
	this->allowedDistance = static_cast<float>(bfr.readDWord()) * 100.0f;
	this->tacticalPoints = bfr.readDWord();
}

IFO::Spawn::~Spawn() {}

