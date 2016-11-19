#include "File.h"

#ifdef _WIN32
#include <windows.h>
#endif

unsigned long File::getFileAttributes() {
	unsigned long result = 0x00;
#ifdef _MSC_VER
	result = ::GetFileAttributesA(this->getPath().toConstChar());
#else

#endif
	return result;
}

String File::getAbsolutePath() const {
	String result = String();
#ifdef _WIN32
	char buf[0x201] = { 0x00 };
	_fullpath(buf, this->path.toConstChar(), 0x200);
	result = String(buf);
#else

#endif
	return result;
}

String File::GetProcessFileNameWithPath() {
	char buffer[0x200] = { 0x00 };
#ifdef _MSC_VER
	GetModuleFileNameA(NULL, buffer, 0x200);
#else

#endif
	return String(buffer);
}

unsigned long long File::getLastModificationTime() const {
	unsigned long long result = 0x00;
#ifdef _WIN32
	HANDLE hFile = CreateFileA(this->getPath().toConstChar(), GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if (hFile) {
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

SharedArrayPtr<String> File::GetFiles(const char *path, bool includeDirToFileName) {
	std::vector<String> tmpResult;
#ifndef _MSC_VER
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path)) != NULL) {
		/* print all the files and directories within directory */

		String strPath = (includeDirToFileName ? String(path) : String("."));
		if (!strPath.endsWith("\\")) {
			strPath += '\\';
		}
		while ((ent = readdir(dir)) != NULL) {
			tmpResult.push_back(strPath + String(ent->d_name));
		}
		closedir(dir);
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
	if (tmpResult.size() == 0) {
		return SharedArrayPtr<String>();
	}
	SharedArrayPtr<String> result(new String[tmpResult.size() - 2], static_cast<unsigned long>(tmpResult.size() - 2));
	for (unsigned int i = 2; i<tmpResult.size(); i++) {
		result[i - 2] = tmpResult.at(i);
	}
	return result;
}

bool File::createSubdirectory(const String& dirName) const {
	String currentPath = this->path;
	if (this->isFolder() && !currentPath.endsWith("\\")) {
		currentPath += "\\";
	}
	int lastPosition = currentPath.findLastOf("\\") + 1;
	String newPath = currentPath.substring(0, lastPosition);
#ifdef _WIN32
	return CreateDirectoryA((newPath + dirName).toConstChar(), NULL) != 0;
#else
	return false;
#endif
}