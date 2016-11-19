#ifndef __ROSE_STL__
#define __ROSE_STL__

#include "..\..\Common\BasicTypes\StringWrapper.h"
#include "..\..\Common\BasicTypes\SharedArrayPointer.h"
#include "..\..\Common\BasicTypes\FileHeader.h"

class STL {
public:
	class Entry {
		public:
			typedef String Language;
			const static unsigned long ENGLISH = 0x01;
		private:
			unsigned long id;
			String idName;
			std::vector<STL::Entry::Language> names;
			std::vector<STL::Entry::Language> descriptions;
		public:
			Entry(const String& content, const unsigned long id) {
				this->id = id;
				this->idName = content;
			}

			__inline String getName(unsigned long langId) const {
				return this->names[langId];
			}
			__inline String getDescription(const unsigned long langId) const {
				return this->descriptions[langId];
			}

			__inline void addLanguage(const String& name, const String& desc) {
				this->names.push_back(name);
				this->descriptions.push_back(desc);
			}

			__inline String getIdName() const {
				return this->idName;
			}
			__inline unsigned long getId() const {
				return this->id;
			}
	};
private:
	std::vector<STL::Entry> entries;
public:
	STL(const SharedArrayPtr<char> data) {
		BufferedFileReader bfr(data, data.getSize(), false);
		unsigned char len = bfr.readByte();
		String hasDescriptionString = bfr.readString(len);

		bool hasDescriptions = hasDescriptionString.contentEquals("ITST01");

		//Amount of entries
		unsigned long entryAmount = bfr.readDWord();
		this->entries.reserve(entryAmount);
		for (unsigned int i = 0; i < entryAmount; i++) {
			len = bfr.readByte();
			String content = bfr.readLine(len);
			unsigned long id = bfr.readDWord();
			this->entries.push_back(STL::Entry(content, id));
		}
		unsigned long amountOfLanguages = bfr.readDWord();
		std::vector<unsigned long> languageOffsets;
		for (unsigned long i = 0; i < amountOfLanguages; i++) {
			languageOffsets.push_back(bfr.readDWord());
		}
		for (unsigned long i = 0; i < amountOfLanguages; i++) {
			bfr.setCaret(languageOffsets[i]);
			std::vector<unsigned long> stringOffsets;
			for (unsigned long j = 0; j < entryAmount; j++) {
				stringOffsets.push_back(bfr.readDWord());
			}
			for (unsigned int j = 0; j < entryAmount; j++) {
				bfr.setCaret(stringOffsets[j]);
				word_t len = bfr.readByte();
				if (len > 0x7F) {
					len = bfr.readByte() << 8 | len;
				}
				String content = bfr.readString(len);
				String description = String();
				if (hasDescriptions) {
					len = bfr.readByte();
					if (len > 0x7F) {
						len = bfr.readByte() << 8 | len;
					}
					description = bfr.readString(len);
				}
				this->entries[j].addLanguage(content, description);
			}
		}
	}
	__inline STL::Entry getEntry(const unsigned long langId) const {
		return this->entries[langId];
	}
	__inline unsigned long getEntryAmount() const {
		return static_cast<unsigned long>(this->entries.size());
	}
};

#endif 