#ifndef __ROSE_VFS__
#define __ROSE_VFS__

#ifdef _MSC_VER
#pragma once
#endif

#include "..\..\Common\BasicTypes\StringWrapper.h"
#include "..\..\Common\BasicTypes\SharedArrayPointer.h"
#include "..\..\Common\BasicTypes\FileHeader.h"
#include <vector>
#include <map>

class VFS {
	public:
		class Entry {
		private:
			String pathInVFS;
			SharedArrayPtr<char> content;
		public:
			Entry() : Entry(nullptr) {}
			Entry(const char* pathInVFS) : Entry(String(pathInVFS)) { }
			Entry(const String& pathInVFS) {
				this->pathInVFS = pathInVFS;
			}
			virtual ~Entry() {
				this->pathInVFS.clear();
			}
			Entry& operator=(const Entry& other) {
				this->pathInVFS = other.pathInVFS;
				this->content = other.content;
				return (*this);
			}
			__inline void loadContent() {
				if (this->content.getSize() == 0 && !this->pathInVFS.isEmpty()) {
					this->content = VFS::get()->readFile(this->pathInVFS);
				}
			}
			__inline SharedArrayPtr<char> getContent() const {
				return this->content;
			}
			__inline String getPathInVFS() const {
				return this->pathInVFS;
			}
		};
	private:
		void *vfsDLLHandle;
		String gameFolder;
		unsigned long vfsHandle;
		unsigned long currentFileHandle;
		std::map<String, Entry> content;
		std::vector<String> loadedFileEndings;
		static VFS* instance;

		typedef dword_t(__stdcall* OpenVFS_FUNCPTR)(const char*, const char*);
		typedef dword_t(__stdcall* CloseVFS_FUNCPTR)(dword_t fileHandle);
		typedef dword_t(__stdcall* GetVFSCount_FUNCPTR)(dword_t fileHandle);
		typedef void(__stdcall* GetVFSNames_FUNCPTR)(dword_t fileHandle, char **array, dword_t arrayLen, word_t lengthOfSingleArrayItem);
		typedef dword_t(__stdcall* GetFileCount_FUNCPTR)(dword_t fileHandle, const char* vfsName);
		typedef void(__stdcall* GetFileNames_FUNCPTR)(dword_t fileHandle, const char* vfsName, char **fileNameArray, DWORD lengthOfArray, dword_t maxStringLength);
		typedef dword_t(__stdcall* OpenFile_FUNCPTR)(const char* fileName, dword_t fileHandle);
		typedef dword_t(__stdcall* RemoveFile_FUNCPTR)(dword_t handle, const char* fileName);
		typedef void(__stdcall* CloseFile_FUNCPTR)(dword_t fileHandle);
		typedef dword_t(__stdcall* ReadFile_FUNCPTR)(void* buffer, dword_t size, dword_t count, dword_t fileHandle);
		typedef dword_t(__stdcall* GetFileSize_FUNCPTR)(dword_t fileHandle);

		OpenVFS_FUNCPTR OpenVFS;
		CloseVFS_FUNCPTR CloseVFS;
		//VFSCount is too high (e.g. 5 VFS exists, but it returns 6 for the data.idx)
		GetVFSCount_FUNCPTR GetVFSCount;
		GetVFSNames_FUNCPTR GetVFSNames;
		GetFileCount_FUNCPTR GetFileCount;
		GetFileNames_FUNCPTR GetFileNames;
		OpenFile_FUNCPTR OpenFile;
		RemoveFile_FUNCPTR RemoveFile;
		CloseFile_FUNCPTR CloseFile;
		ReadFile_FUNCPTR ReadFile;
		GetFileSize_FUNCPTR GetFileSize;

		std::vector<String> extractVFSNames(unsigned long vfsCount);
		__inline void extractFilePaths(std::vector<String> vfsNames, std::vector<String> fileEndingsToLoad) {
			this->extractFilePaths(vfsNames, fileEndingsToLoad, false);
		}
		void extractFilePaths(std::vector<String> vfsNames, std::vector<String> fileEndingsToLoad, bool excludeDifferentFileEndings);
		SharedArrayPtr<char> readFile(const String& path);
		void extractFileContent(std::vector<String> fileEndingsToLoad);
	protected:
		__inline unsigned long getVFSHandle() const {
			return this->vfsHandle;
		}
	public:
		explicit VFS(const char *gameFolder) : VFS(String(gameFolder)) {}
		explicit VFS(const String& gameFolder) : VFS(gameFolder, false) {}
		VFS(const char* gameFolder, bool excludeDifferentFileEndings) : VFS(String(gameFolder), excludeDifferentFileEndings) {}
		VFS(const String& gameFolder, bool excludeDifferentFileEndings) : VFS(gameFolder, excludeDifferentFileEndings, { String("AIP"), String("CHR"), String("IFO"), String("QSD"), String("STB"), String("STL"), String("ZMO"), String("ZON") }) {}
		VFS(const String& gameFolder, bool excludeDifferentFileEndings, std::vector<String> fileEndingsToLoad);
		virtual ~VFS();
		
		__inline std::map<String, Entry>::iterator begin() {
			return this->content.begin();
		}
		__inline std::map<String, Entry>::iterator end() {
			return this->content.end();
		}
		__inline VFS::Entry getEntry(const char *path) const {
			return this->getEntry(String(path));
		}
		VFS::Entry getEntry(const String& path) const {
			auto result = this->content.find(path);
			if (result == this->content.cend()) {
				return VFS::Entry();
			}
			return (*result).second;
		}

		__inline static VFS* get() {
			return VFS::instance;
		}
};

#endif