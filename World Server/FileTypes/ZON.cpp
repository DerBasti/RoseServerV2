#include "ZON.h"
#include "..\Entities\Entity.h"

ZON::EventInformation::EventInformation(const byte_t id, const String& eventName) : ZON::EventInformation(id, eventName, Position()) {}
ZON::EventInformation::EventInformation(const byte_t id, const String& eventName, const Position& pos) {
	this->id = id;
	this->name = eventName;
	this->position = pos;
}


ZON::Center::Center() : ZON::Center(Position()) {}
ZON::Center::Center(const Position& newCenter) : ZON::Center(newCenter, 32, 32) {}
ZON::Center::Center(const Position& newCenter, const byte_t centerIfoX, const byte_t centerIfoY) {
	this->center = newCenter;
	this->ifoCenter.x = centerIfoX;
	this->ifoCenter.y = centerIfoY;
}

Position ZON::Center::getPosition() const {
	return this->center;
}
void ZON::Center::setPosition(const Position& pos) {
	this->center = pos;
}

ZON::ZON(const String& pathInVFS, const SharedArrayPtr<char>& data) {
	this->path = pathInVFS;

	BufferedFileReader bfr(data, data.getSize());
	dword_t infoCount = bfr.readDWord();
	struct BlockType {
		std::vector<dword_t> type;
		std::vector<dword_t> offset;
	} blocks;

	for (dword_t i = 0; i < infoCount; i++) {
		blocks.type.push_back(bfr.readDWord());
		blocks.offset.push_back(bfr.readDWord());
	}

	for (dword_t i = 0; i < infoCount; i++) {
		bfr.setCaret(blocks.offset[i]);
		switch (blocks.type[i]) {
			case 0x00:
				this->loadZoneInfos(bfr);
			break;
			case 0x01:
				this->loadEventInfos(bfr);
			break;
			case 0x02:
				this->loadEconomyInfos(bfr);
			break;
		}
	}
	auto eventInfo = this->getEvent("start");
	if (eventInfo->isValid()) {
		this->mapCenter.setPosition(eventInfo->getPosition());
	}
}

ZON::~ZON() {
	std::for_each(this->events.begin(), this->events.end(), [](EventInformation* event) {
		delete event;
		event = nullptr;
	});
	this->events.clear();
}

void ZON::loadEconomyInfos(BufferedFileReader& bfr) {
	byte_t strLength = bfr.readByte();
	String unknown = bfr.readString(strLength);

	bool isDungeon = bfr.readDWord() > 0;

	strLength = bfr.readByte();
	String music = bfr.readString(strLength);

	strLength = bfr.readByte();
	String model = bfr.readString(strLength);

	dword_t townCounter = bfr.readDWord();
	dword_t populationCounter = bfr.readDWord();
	dword_t developmentCounter = bfr.readDWord();

	for (byte_t i = 1; i < 0x0B; i++) {
		bfr.readDWord(); //consum rate
	}
}

void ZON::loadEventInfos(BufferedFileReader& bfr) {
	dword_t entryAmount = bfr.readDWord();
	for (dword_t i = 0; i < entryAmount; i++) {
		float x = bfr.readFloat() + 520000.0f;
		bfr.readFloat(); //Z coordinate, unimportant
		float y = bfr.readFloat() + 520000.0f;

		byte_t eventId = static_cast<byte_t>(i);
		byte_t strLen = bfr.readByte();
		String eventName = bfr.readString(strLen);

		EventInformation *newEvent = new EventInformation(eventId, eventName, Position(x, y));
		this->events.push_back(newEvent);
	}
}
void ZON::loadZoneInfos(BufferedFileReader& bfr) {
	bfr.setCaret(bfr.getCaret() + sizeof(dword_t));

	dword_t mapHeight = bfr.readDWord();
	dword_t mapWidth = bfr.readDWord();
	dword_t patchGridSize = bfr.readDWord();

	float gridSize = bfr.readFloat();
	float patchSize = gridSize * patchGridSize;

	byte_t centerIfoX = static_cast<byte_t>(bfr.readDWord());
	byte_t centerIfoY = static_cast<byte_t>(bfr.readDWord());

	this->mapCenter.setIfoCenter(centerIfoX, centerIfoY);
}

const ZON::EventInformation* ZON::getEvent(const String& name) const {
	auto resultIterator = std::find_if(this->events.begin(), this->events.end(), [&](ZON::EventInformation* eventInfo) {
		return eventInfo != nullptr && eventInfo->getName().contentEquals(name);
	});
	return (resultIterator == this->events.end() ? nullptr : (*resultIterator));
}


bool ZON::EventInformation::isValid() const {
	return this->getId() != 0 || !this->getName().isEmpty() || this->getPosition().getX() != 0.0f || this->getPosition().getY() != 0.0f;
}