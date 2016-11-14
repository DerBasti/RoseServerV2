#ifndef __BUFFERED_FILE_READER__
#define __BUFFERED_FILE_READER__

#include "FileReader.h"

class BufferedFileReader : public FileReader {
    private:
        char *data;
        unsigned long long size;
        
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
        BufferedFileReader(const char *d, unsigned long long len) {
            this->size = len;
            this->data = new char[this->getFileSize()];
            memcpy(this->data, d, this->getFileSize());
		}
#ifdef _MSC_VER
#pragma warning(default:4244)
#endif

		virtual ~BufferedFileReader() {
			delete[] this->data;
			this->data = nullptr;
		}


        virtual __inline unsigned long long getFileSize() const {
            return this->size;
        }
            
};

#endif //__BUFFERED_FILE_READER__
