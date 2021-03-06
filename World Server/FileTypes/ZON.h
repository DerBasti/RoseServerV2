#ifndef __ROSE_ZON__
#define __ROSE_ZON__

#ifdef _MSC_VER
#pragma once
#endif

#include "VFS.h"
#include "..\..\Common\datatypes.h"

class ZON {
public:
	class EventInformation {
		private:
			byte_t id;
			String name;
			Position position;
		public:
			EventInformation() : EventInformation(0, String()) {}
			EventInformation(const byte_t id, const String& eventName);
			EventInformation(const byte_t id, const String& eventName, const Position& pos);

			__inline byte_t getId() const {
				return this->id;
			}
			__inline String getName() const {
				return this->name;
			}

			__inline Position getPosition() const{
				return this->position;
			}

			bool isValid() const;
	};
	class Center {
		private:
			Position center;
			struct _ifoCenter {
				byte_t x;
				byte_t y;
			} ifoCenter;
		public:
			Center();
			explicit Center(const Position& newCenter);
			Center(const Position& newCenter, const byte_t centerIfoX, const byte_t centerIfoY);

			Position getPosition() const;
			void setPosition(const Position& pos);

			const byte_t getIfoCenterX() const {
				return this->ifoCenter.x;
			}
			const byte_t getIfoCenterY() const {
				return this->ifoCenter.y;
			}

			void setIfoCenter(const byte_t newX, const byte_t newY) {
				this->ifoCenter.x = newX;
				this->ifoCenter.y = newY;
			}
	};

private:
	void loadEconomyInfos(BufferedFileReader& bfr);
	void loadEventInfos(BufferedFileReader& bfr);
	void loadZoneInfos(BufferedFileReader& bfr);

	Center mapCenter;
	std::vector<EventInformation*> events;

	dword_t sectorSize;

	String path;
public:
	ZON(const char* pathInVFS, const SharedArrayPtr<char>& data) : ZON(String(pathInVFS), data) {}
	ZON(const String& pathInVFS, const SharedArrayPtr<char>& data);

	__inline const EventInformation* getEvent(const char* name) const {
		return this->getEvent(String(name));
	}
	const EventInformation* getEvent(const String& name) const;

	virtual ~ZON();

	__inline dword_t getSectorSize() const {
		return this->sectorSize;
	}

	__inline void setSectorSize(const dword_t size) {
		this->sectorSize = size;
	}
	__inline const Center& getMapCenterData() const {
		return this->mapCenter;
	}
};

#endif