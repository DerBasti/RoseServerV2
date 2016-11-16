#ifndef __BUFFERED_FILE_READER__
#define __BUFFERED_FILE_READER__

#include "FileReader.h"

class BufferedFileReader : public FileReader {
    private:
        const char *data;
        unsigned long long size;
		bool dataCopied;
        
        void internalRead(void *storagePtr, unsigned long len) {
            if(this->isInInvalidState()) {
                memset(storagePtr, 0x00, len);
                return;
            }
			unsigned char* castedPtr = reinterpret_cast<unsigned char*>(storagePtr);
			for (unsigned i = 0; i < len; i++) {
				castedPtr[i] = this->data[this->getCaret() + i];
			}
        }
        __inline void updateSize() { }
        
        virtual bool isInInvalidState() const {
            return this->getCaret() >= this->getFileSize();
        }
    public:
#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif
        BufferedFileReader(const char *d, unsigned long long len) : BufferedFileReader(d, len, true) {}
		BufferedFileReader(const char *d, unsigned long long len, bool copyData) {
			this->size = len;
			this->dataCopied = copyData;
			if (this->dataCopied) {
				char *cpyData = new char[this->getFileSize()];
				memcpy(cpyData, d, this->getFileSize());
				this->data = cpyData;
			}
			else {
				this->data = d;
			}
		}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

		virtual ~BufferedFileReader() {
			if (this->dataCopied) {
				delete[] this->data;
				this->data = nullptr;
			}
		}


        virtual __inline unsigned long long getFileSize() const {
            return this->size;
        }
            
};

#endif //__BUFFERED_FILE_READER__
