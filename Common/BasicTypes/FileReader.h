#ifndef __FILE_READER__
#define __FILE_READER__

#include <functional>
#include "BasicObject.h"
#include "File.h"
#include "SharedArrayPointer.h"

class FileReader : public BasicObject {
    private:  
        const static unsigned char STRING_READ_ABORT = 0x01;
        const static unsigned char STRING_READ_SKIP = 0x02;
		const static unsigned char STRING_MAX_LENGTH = 0x04;
    protected:
        File file;
        unsigned long long currentPos;
		bool ignoreCarriageReturnFlag;

        virtual bool isInInvalidState() const {
			if (!this->getFile()->exists()) {
				return true;
			}
            bool invalidState = feof(this->getFile()->getHandle()) != 0;
            invalidState |= ferror(this->getFile()->getHandle()) != 0;
            invalidState |=  this->getCaret() >= this->getFileSize();
            return invalidState;
        }
        virtual void internalRead(void *storagePtr, unsigned long len) {
            if(this->isInInvalidState()) {
                memset(storagePtr, 0x00, len);
                return;
            }
            fread(storagePtr, len, 0x01, this->getFile()->getHandle());
        }
        virtual __inline void updateSize() {
        	unsigned long long pos = this->getCaret();
        	this->getFile()->close();
        	this->getFile()->openWithMode("rb");
        	this->setCaret(pos);
        }
        String readStringWithPredicate(std::function<unsigned char(unsigned char, unsigned long long)> f) {
            String result = String();
            unsigned char c = 0x00;
			unsigned totalReadBytes = 0x00;
            while(!this->isInInvalidState()) {
				c = this->readByte(); totalReadBytes++;
				result += c;
				unsigned char predResult = f(c, totalReadBytes);
				if (predResult & (STRING_READ_ABORT | STRING_READ_SKIP)) {
					result = result.getLength() == 1 ? String() : result.substring(0, result.getLength() - 1);
				}
				if (predResult & (STRING_READ_ABORT | STRING_MAX_LENGTH)) {
					break;
				}
            }
            return result;
        }
		FileReader() {
			this->file = nullptr;
			this->currentPos = 0;
			this->ignoreCarriageReturnFlag = false;
		}
    public:
        explicit FileReader(const File& f) {
            this->file = f;
            this->currentPos = 0;
            this->file.openWithMode("rb");
			this->ignoreCarriageReturnFlag = false;
        }
        FileReader(const char *path) : FileReader(File(path)) {}
        explicit FileReader(const String& s) : FileReader(s.toConstChar()) {}
        
#define READ_FUNC(name, type) type name() { \
            type b = 0x00; \
            this->internalRead((void*)&b, sizeof(type)); \
            this->addToCaret(sizeof(type)); \
            return b; \
        }
        
        READ_FUNC(readByte, byte_t);
        READ_FUNC(readWord, word_t);
		READ_FUNC(readDWord, dword_t);
		READ_FUNC(readQWord, qword_t);
		READ_FUNC(readFloat, float);
		READ_FUNC(readDouble, double);
        
#undef READ_FUNC
        
        
        
        String readLine() {
            return this->readLine(static_cast<unsigned long long>(-1));
        }
        String readLine(unsigned long long maxLen) {
			if (maxLen == 0x00) {
				return String();
			}
            std::function<unsigned char(unsigned char, unsigned long long)> f = [&](unsigned char c, unsigned long long len) -> unsigned char {
				unsigned char result = 0x00;
				if (len >= maxLen) {
					result |= STRING_MAX_LENGTH;
				} 
				if (c == '\n') {
					result |= STRING_READ_ABORT;
                }
				if(!this->ignoreCarriageReturn() && c == '\r') {
					result |= STRING_READ_SKIP;
				}
				return result;
            };
            
            return readStringWithPredicate(f);
        }
#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
        void readLineEx(unsigned long long maxLen, char* p) {
            memcpy(p, this->readString(maxLen).toConstChar(), maxLen);
        }
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif
		SharedArrayPtr<char> readBinary(unsigned long maxLen) {
			SharedArrayPtr<char> result(new char[maxLen], maxLen);
			for (unsigned long i = 0; i < maxLen && !this->isInInvalidState();i++) {
				result[i] = this->readByte();
			}
			return result;
		}
        
        String readString() {
            return this->readString(static_cast<unsigned long long>(-1));
        }
		String readString(unsigned long long maxLen) {
			if (maxLen == 0x00) {
				return String();
			}
            std::function<unsigned char(unsigned char, unsigned long long)> f = [&](unsigned char c, unsigned long long len) -> unsigned char {
				unsigned char result = 0x00;
				if (len >= maxLen) {
					result |= STRING_MAX_LENGTH;
				} 
				if (c == 0x00) {
					result |= STRING_READ_ABORT;
                }
                if(!this->ignoreCarriageReturn() && c == '\r') {
					result |= STRING_READ_SKIP;
                } 
				return result;
            };
            return this->readStringWithPredicate(f);
		}
#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
        void readStringEx(unsigned long long maxLen, char* p) {
            memcpy(p, this->readString(maxLen).toConstChar(), maxLen);
		}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

		__inline bool ignoreCarriageReturn() const {
			return ignoreCarriageReturnFlag;
		}

		__inline bool setIgnoreCarriageReturn(bool flag) {
			this->ignoreCarriageReturnFlag = flag;
		}
        
        __inline File* getFile() const {
            return (File *)&this->file;
        }
        
        __inline unsigned long long getCaret() const {
            return this->currentPos;
        }
        
        void setCaret(unsigned long long newPos) {
            this->currentPos = newPos;
            if(this->getFile()->getHandle() != NULL) {
				File_SeekOffset(this->getFile()->getHandle(), newPos, SEEK_SET);
            }
        }
        
        __inline bool hasInvalidState() const {
        	return this->isInInvalidState();
        }

        __inline void addToCaret(unsigned long long addPos) {
            this->setCaret(this->getCaret() + addPos);
        }
        
        virtual __inline unsigned long long getFileSize() const {
            return this->getFile()->getFileSize();
        }
};

#endif
