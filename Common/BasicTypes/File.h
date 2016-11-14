#ifndef __FILE_WRAPPER__
#define __FILE_WRAPPER__

#include "BasicObject.h"
#include "SharedArrayPointer.h"
#include <stdio.h>
#include <vector>
#ifndef _MSC_VER
#include <dirent.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

class FileType : public BasicObject {
private:
	FileType() {}
	~FileType() {}
public:
};

class File : public BasicObject {
	private:
		friend class FileWriter;
		friend class FileReader;
		FILE *handle;
		DWORD fileFlags;

#ifdef _WIN32
#define INVALID_FILE_ATTRIBS INVALID_FILE_ATTRIBUTES
#define FOLDER_ATTRIBUTE FILE_ATTRIBUTE_DIRECTORY
#else
		INVALID_FILE_ATTRIBS 0x00
#define FOLDER_ATTRIBUTE 0x00
#endif

#ifndef _MSC_VER
#define File_SeekOffset fseeko64
#define File_TellOffset ftello64
#else
#define File_SeekOffset _fseeki64
#define File_TellOffset _ftelli64
#endif

	protected:
		String path;
		unsigned long long fileSize;
		unsigned long long currentPosition;

		__inline FILE *getHandle() const {
			return this->handle;
		}

		void close(bool keepMetaData) {
			if(this->getHandle()) {
				fclose(this->getHandle());
			}
			this->handle = NULL;
			this->currentPosition = 0;
			if(!keepMetaData) {
				this->fileSize = this->fileFlags = 0;
			}
		}

		__inline void close() {
			this->close(false);
		}

		bool openWithMode(const char* rights) {
			this->fileFlags = GetFileAttributesA(path.toConstChar());
			if (!this->getHandle() && fopen_s(&this->handle, this->path.toConstChar(), rights) == 0) {
				fseek(this->handle, 0, SEEK_END);
				this->fileSize = File_TellOffset(this->handle);
				rewind(this->handle);
			}
			if (this->isFolder() && !this->path.endsWith("\\")) {
				this->path += "\\";
			}
			return this->getHandle() != NULL;
		}
		File() {
			this->handle = NULL;
			this->fileSize = this->currentPosition = 0;
			this->fileFlags = INVALID_FILE_ATTRIBS;
		}
	public:
		File(const char *filePath) : File(String(filePath)) {
		}
		File(const String& filePath) {
			this->fileSize = this->currentPosition = 0;
			this->fileFlags = INVALID_FILE_ATTRIBS;
			this->path = filePath;
			this->handle = NULL;
			if (this->openWithMode("rb")) {
				this->close(true);
			}
		}

		File(const File& f) {
			this->fileSize = this->currentPosition = this->fileFlags = 0;
			this->handle = NULL;
			this->operator=(f);
		}

		~File() {
			this->close();
		}

		File& operator=(const File& f) {
			this->close();
			this->path = f.path;
			this->fileSize = f.fileSize;
			this->fileFlags = f.fileFlags;

			return (*this);
		}

		__inline String getPath() const {
			return this->path;
		}

		__inline String getAbsolutePath() const {
			String result = String();
#ifdef _WIN32
			char buf[0x201] = {0x00};
			_fullpath(buf, this->path.toConstChar(), 0x200);
			result = String(buf);
#else

#endif
			return result;
		}

		__inline static bool Exists(const String& s) {
			return File::Exists(s.toConstChar());
		}

		__inline static bool Exists(const char *path) {
			return File(path).exists();
		}


		__inline bool exists() const {
			return this->fileFlags != INVALID_FILE_ATTRIBS;
		}

		static bool IsFile(const char *path) {
			return File(path).isFile();
		}

		static String GetProcessFileNameWithPath() {
			char buffer[0x200] = { 0x00 };
			GetModuleFileNameA(NULL, buffer, 0x200);
			return String(buffer);
		}

		static String GetProcessFileName() {
			String result = GetProcessFileNameWithPath();
			return result.substring(result.lastPositionOf("\\")+1);
		}

		static String GetWorkingDirectory() {
			String result = GetProcessFileNameWithPath();
			return result.substring(0, result.lastPositionOf("\\") + 1);
		}

		__inline bool isFile() const {
			return (this->exists() && (this->fileFlags & FOLDER_ATTRIBUTE) == 0);
		}

		__inline bool isFolder() const {
			return (this->exists() && (this->fileFlags & FOLDER_ATTRIBUTE) != 0);
		}

		unsigned long long getLastModificationTime() const {
			unsigned long long result = 0x00;
#ifdef _WIN32
			HANDLE hFile = CreateFileA(this->getPath().toConstChar(), GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, 0, NULL);
			if(hFile) {
				FILETIME ftCreate, ftAccess, ftWrite;
				SYSTEMTIME stUTC;
				if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
					return 0x00;
				}
				FileTimeToSystemTime(&ftWrite, &stUTC); 

				std::tm tm;
				tm.tm_sec = stUTC.wSecond;
				tm.tm_min = stUTC.wMinute;
				tm.tm_hour = stUTC.wHour;
				tm.tm_mday = stUTC.wDay;
				tm.tm_mon = stUTC.wMonth - 1;
				tm.tm_year = stUTC.wYear - 1900;
				tm.tm_isdst = -1;
				result = std::mktime(&tm);
			}
#else

#endif
			return result;
		}

		static SharedArrayPtr<String> GetFiles(const char *path) {
			return GetFiles(path, false);
		}

		static SharedArrayPtr<String> GetFiles(const char *path, bool includeDirToFileName) {
			std::vector<String> tmpResult;
#ifndef _MSC_VER
			DIR *dir;
			struct dirent *ent;
			if ((dir = opendir (path)) != NULL) {
				/* print all the files and directories within directory */

				String strPath = (includeDirToFileName ? String(path) : String("."));
				if(!strPath.endsWith("\\")) {
					strPath += '\\';
				}
				while ((ent = readdir (dir)) != NULL) {
					tmpResult.push_back(strPath + String(ent->d_name));
				}
				closedir (dir);
			}
#else
			WIN32_FIND_DATAA fileData;
			HANDLE handle = FindFirstFileA(path, &fileData);
			if (handle != INVALID_HANDLE_VALUE) {
				do {
					tmpResult.push_back(String(fileData.cFileName));
				} while (FindNextFileA(handle, &fileData));
				FindClose(handle);
			}
#endif
			if(tmpResult.size() == 0) {
				return SharedArrayPtr<String>();
			}
			SharedArrayPtr<String> result(new String[tmpResult.size()-2], static_cast<unsigned long>(tmpResult.size()-2));
			for(unsigned int i=2;i<tmpResult.size();i++) {
				result[i-2] = tmpResult.at(i);
			}
			return result;
		}

		__inline unsigned long long getFileSize() const {
			return this->fileSize;
		}

		static void GetFilesEx(const char *path, SharedArrayPtr<String>* p) {
			(*p) = File::GetFiles(path);
		}

		bool createSubdirectory(const String& dirName) const {
			String currentPath = this->path;
			if (this->isFolder() && !currentPath.endsWith("\\")) {
				currentPath += "\\";
			}
			int lastPosition = currentPath.lastPositionOf("\\")+1;
			String newPath = currentPath.substring(0, lastPosition);
#ifdef _WIN32
			return CreateDirectoryA((newPath + dirName).toConstChar(), NULL) != 0;
#else
			return false;
#endif
		}

		__inline bool createSubdirectory(const char *dirName) const {
			return this->createSubdirectory(String(dirName));
		}

		__inline bool subdirectoryExists(const char *folderName) const {
			return this->subdirectoryExists(String(folderName));
		}
		bool subdirectoryExists(const String& folderName) const {
			if (this->exists()) {
				String currentFolder = this->path.substring(0, this->path.lastPositionOf("\\") + 1);
				String totalPath = currentFolder + folderName;
				File subdir(totalPath);
				return subdir.exists() && subdir.isFolder();
			}
			return false;
		}

};

#include "FileReader.h"
#include "FileWriter.h"

#endif
