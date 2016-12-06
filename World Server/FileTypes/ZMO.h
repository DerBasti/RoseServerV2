#ifndef __ROSE_ZMO__
#define __ROSE_ZMO__

#ifdef _MSC_VER
#pragma once
#endif

#include "VFS.h"
#include "..\..\Common\BasicTypes\StringWrapper.h"
#include <map>

class ZMO {
private:
	String pathInVFS;
	std::map<dword_t, word_t> trigger;

	//For debug purposes
	dword_t totalAmountOfFrames;
	dword_t framesPerSecond;
	dword_t defaultPlayoutTime; //in ms
public:
	ZMO(const String& path, const VFS::Entry& entry) : ZMO(path, entry.getContent()) {}
	ZMO(const String& path, const SharedArrayPtr<char>& data) {
		this->pathInVFS = path;

		BufferedFileReader bfr(data, data.getSize());
		String fileType = bfr.readString(8);
		if (fileType.contentEquals("ZMO0002")) {
			this->framesPerSecond = bfr.readDWord();
			this->totalAmountOfFrames = bfr.readDWord();
			this->defaultPlayoutTime = (this->totalAmountOfFrames * 1000 / this->framesPerSecond);

			bfr.setCaret(data.getSize() - 14 - sizeof(word_t)*this->totalAmountOfFrames);
			for (unsigned int i = 0; i < this->totalAmountOfFrames; i++) {
				word_t frameType = bfr.readWord();
				if (i < 4 || !(frameType >= 31 && frameType <= 39)) {
					continue;
				}
				trigger[(i * 1000 / this->framesPerSecond)] = frameType;
			}
		}
	}
	const std::map<dword_t, word_t>& getTrigger() const {
		return this->trigger;
	}
	__inline dword_t getDefaultPlayoutTime() const {
		return this->defaultPlayoutTime;
	}
};

#endif