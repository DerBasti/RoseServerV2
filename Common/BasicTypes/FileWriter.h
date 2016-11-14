#ifndef __FILE_WRITER__
#define __FILE_WRITER__

#include "BasicObject.h"
#include "File.h"

class FileWriter : public BasicObject {
	private:
		bool appendTimestamp;
		bool appendMode;
    protected:
        File file;
        virtual __inline FILE* getHandle() const {
        	return this->getFile()->getHandle();
        }
    public:
		FileWriter(const char* path) : FileWriter(File(path), false) {}
		explicit FileWriter(const File& f) : FileWriter(f, false) {
		}
		explicit FileWriter(const File& f, bool appendMode) : FileWriter(f, appendMode, false) {
		}
		explicit FileWriter(const File& f, bool appendMode, bool appendTimestamp) {
			this->file = f;
			this->appendMode = appendMode;
			this->file.openWithMode((this->appendMode ? "a+" : "wb+"));
			this->appendTimestamp = appendTimestamp;
		}

        virtual ~FileWriter() {
			this->close();
        }
		__inline void close() {
			if (this->getFile()) {
				this->file.close();
			}
		}

		__inline void reopen() {
			this->file.openWithMode((this->appendMode ? "a+" : "wb+"));
		}

        template<class _T> __inline bool write(const _T& data) const {
        	return this->getFile()->exists() && (fwrite(&data, sizeof(_T), 1, this->getHandle()) == sizeof(_T));
        }

        __inline bool writeString(const char* data) const {
        	return this->getFile()->exists() && (fwrite(data, 1, strlen(data), this->getHandle()) == strlen(data));
        }

        __inline bool writeString(const String& s) const {
        	return this->getFile()->exists() && (fwrite(s.toConstChar(), 1,s.length(), this->getHandle()) == s.length());
		}
		__inline bool writeLine(const char *s) const {
			return this->writeLine(String(s));
		}
		__inline bool writeLine(const String& s) const {
			return this->writeString((this->appendTimestamp ? (TimeUnits::GetCurrentDayTimeWithMillis() + String(" ")) : String()) + s + String("\n"));
		}
		__inline void appendTimeToLines(bool v) {
			this->appendTimestamp = v;
		}

        const File* getFile() const {
        	return &this->file;
        }
};

#endif
