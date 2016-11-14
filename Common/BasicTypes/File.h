#ifndef __FILE_WRAPPER__
#define __FILE_WRAPPER__

#include "BasicObject.h"
#include "SharedArrayPointer.h"
#include <stdio.h>
#include <vector>
#ifndef _MSC_VER
#include <dirent.h>
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
		unsigned long fileFlags;

#ifdef _WIN32
#define INVALID_FILE_ATTRIBS ((unsigned long)-1)
#define FOLDER_ATTRIBUTE 0x00000010 
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

		unsigned long getFileAttributes();

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
			this->fileFlags = this->getFileAttributes();
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

		String getAbsolutePath() const;

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

		static String GetProcessFileNameWithPath();

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

		unsigned long long getLastModificationTime() const;

		static SharedArrayPtr<String> GetFiles(const char *path) {
			return GetFiles(path, false);
		}

		static SharedArrayPtr<String> GetFiles(const char *path, bool includeDirToFileName);

		__inline unsigned long long getFileSize() const {
			return this->fileSize;
		}

		static void GetFilesEx(const char *path, SharedArrayPtr<String>* p) {
			(*p) = File::GetFiles(path);
		}

		bool createSubdirectory(const String& dirName) const;

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

#endif
