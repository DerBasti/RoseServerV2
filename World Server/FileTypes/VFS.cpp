#include <windows.h>
#include "VFS.h"

VFS* VFS::instance = nullptr;

VFS::VFS(const String& gameFolder, bool excludeDifferentFileEndings, std::vector<String> fileEndingsToLoad) {
	this->gameFolder = gameFolder;
	if (File(gameFolder).isFolder() && (this->vfsDLLHandle = (void*)::LoadLibraryA((this->gameFolder + String("TriggerVFS.dll")).toConstChar())) != nullptr) {
		HMODULE triggerVFS = static_cast<HMODULE>(this->vfsDLLHandle);
		this->OpenVFS = reinterpret_cast<OpenVFS_FUNCPTR>(GetProcAddress(triggerVFS, "_OpenVFS@8"));
		this->CloseVFS = reinterpret_cast<CloseVFS_FUNCPTR>(GetProcAddress(triggerVFS, "_CloseVFS@4"));

		this->GetVFSCount = reinterpret_cast<GetVFSCount_FUNCPTR>(GetProcAddress(triggerVFS, "_VGetVfsCount@4"));
		this->GetVFSNames = reinterpret_cast<GetVFSNames_FUNCPTR>(GetProcAddress(triggerVFS, "_VGetVfsNames@16"));

		this->GetFileCount = reinterpret_cast<GetFileCount_FUNCPTR>(GetProcAddress(triggerVFS, "_VGetFileCount@8"));
		this->GetFileNames = reinterpret_cast<GetFileNames_FUNCPTR>(GetProcAddress(triggerVFS, "_VGetFileNames@20"));
		this->OpenFile = reinterpret_cast<OpenFile_FUNCPTR>(GetProcAddress(triggerVFS, "_VOpenFile@8"));
		//this->RemoveFile = reinterpret_cast<RemoveFile_FUNCPTR>(GetProcAddress(triggerVFS, "_VRemoveFile@8"));
		this->CloseFile = reinterpret_cast<CloseFile_FUNCPTR>(GetProcAddress(triggerVFS, "_VCloseFile@4"));
		this->ReadFile = reinterpret_cast<ReadFile_FUNCPTR>(GetProcAddress(triggerVFS, "_vfread@16"));
		this->GetFileSize = reinterpret_cast<GetFileSize_FUNCPTR>(GetProcAddress(triggerVFS, "_vfgetsize@4"));

		VFS::instance = this;
		this->loadedFileEndings = fileEndingsToLoad;

		this->vfsHandle = this->OpenVFS((this->gameFolder + "data.idx").toConstChar(), "r");
		std::vector<String> vfsNames = extractVFSNames(this->GetVFSCount(this->getVFSHandle()) - 1);
		this->extractFilePaths(vfsNames, fileEndingsToLoad, excludeDifferentFileEndings);
		this->extractFileContent(fileEndingsToLoad);
	}
}
		
VFS::~VFS() {
	if(this->vfsDLLHandle) {
		::FreeLibrary(reinterpret_cast<HMODULE>(this->vfsDLLHandle));
	}
	VFS::instance = nullptr;
	this->vfsDLLHandle = nullptr;
}

std::vector<String> VFS::extractVFSNames(unsigned long vfsCount) {
	char** tmpVFSNames = new char*[vfsCount];
	for (dword_t i = 0; i < vfsCount; i++) {
		tmpVFSNames[i] = new char[0x100]; //256
	}
	this->GetVFSNames(this->vfsHandle, tmpVFSNames, vfsCount, 0x100);

	std::vector<String> vfsNames;
	for (dword_t i = 0; i < vfsCount; i++) {
		vfsNames.push_back(String(tmpVFSNames[i]));
		delete[] tmpVFSNames[i];
		tmpVFSNames[i] = nullptr;
	}
	delete[] tmpVFSNames;
	tmpVFSNames = nullptr;

	return vfsNames;
}

void VFS::extractFilePaths(std::vector<String> vfsNames, std::vector<String> fileEndingsToLoad, bool excludeDifferentFileEndings) {
	const unsigned short MAX_LEN = 0x100;
	for (unsigned long i = 0; i < vfsNames.size(); i++) {
		unsigned long fileCount = this->GetFileCount(this->getVFSHandle(), vfsNames[i].toConstChar());
		char **file = new char*[fileCount];
		for (unsigned int j = 0; j < fileCount; j++) {
			file[j] = new char[MAX_LEN];
		}
		this->GetFileNames(this->getVFSHandle(), vfsNames[i].toConstChar(), file, fileCount, MAX_LEN);

		for (unsigned int j = 0; j < fileCount; j++) {
			String f(file[j]);

			delete[] file[j];
			file[j] = nullptr;

			if (excludeDifferentFileEndings) {
				bool allowInsert = false;
				for (unsigned int k = 0; k < fileEndingsToLoad.size(); k++) {
					if (f.endsWith(fileEndingsToLoad[k])) {
						allowInsert = true;
						break;
					}
				}
				if (!allowInsert) {
					continue;
				}
			}
			this->content[f] = Entry(f);

		}
		delete[] file;
		file = nullptr;
	}
}

SharedArrayPtr<char> VFS::readFile(const String& path) {
	this->currentFileHandle = this->OpenFile(path.toConstChar(), this->getVFSHandle());
	unsigned long fileLength = this->GetFileSize(this->currentFileHandle);

	SharedArrayPtr<char> tempBuf(new char[fileLength], fileLength);
	memset(tempBuf.get(), 0x00, fileLength);
	unsigned int readLength = this->ReadFile(tempBuf.get(), 1, fileLength, this->currentFileHandle);
	this->CloseFile(this->currentFileHandle);
	this->currentFileHandle = 0x00;
	if (readLength != fileLength) {
		return SharedArrayPtr<char>();
	}
	return tempBuf;
}

void VFS::extractFileContent(std::vector<String> fileEndingsToLoad) {
	std::for_each(this->begin(), this->end(), [&](std::pair<const String, Entry>& p) {
		for (unsigned int i = 0; i < fileEndingsToLoad.size(); i++) {
			if (p.first.endsWith(fileEndingsToLoad[i])) {
				p.second.loadContent();
				break;
			}
		}
	});
}