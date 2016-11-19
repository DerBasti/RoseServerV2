#ifndef __ROSE_WORLDSERVER__
#define __ROSE_WORLDSERVER__

#ifdef _MSC_VER
#pragma once
#endif

#include "..\Common\ROSESocketServer.h"
#include "FileTypes\STB.h"
#include "FileTypes\VFS.h"
#include "Map.h"

class WorldServer : public ROSEServer {
	private:
		std::shared_ptr<VFS> vfs;
		std::shared_ptr<NPCSTB> npcFile;
		std::shared_ptr<AISTB> aiFile;
		std::shared_ptr<SkillSTB> skillFile;
		std::shared_ptr<StatusSTB> statusFile;
		std::shared_ptr<STB> questFile;
		std::shared_ptr<STB> equipmentFile[15]; //0 = NOT VALID
		std::shared_ptr<STB> craftingFile;
		std::shared_ptr<STB> sellFile;
		std::shared_ptr<ZoneSTB> zoneFile;
		std::shared_ptr<STB> motionFile;
		std::shared_ptr<STB> dropFile;
		std::shared_ptr<STB> warpFile;

		std::vector<Map*> maps;
		std::map<word_t, Telegate*> telegates;

		template<class _T> void loadSTB(std::shared_ptr<_T>& ptr, const char *path, bool applySTL = true) {
			this->logger.info(String("Loading STB: ") + String(path));
			String pathAsString = String(path);	
			if (!pathAsString.isEmpty()) {
				auto entry = VFS::get()->getEntry(pathAsString);
				ptr = std::shared_ptr<_T>(new _T(String(pathAsString), entry.getContent()));
			}
		}

		void loadZoneData();
		void loadIFOs(Map *currentMap, std::vector<VFS::Entry>& ifoFiles);
	public:
		WorldServer(const char* IP, unsigned short port, MYSQL* mysql) : WorldServer(String(IP), port, mysql) {}
		WorldServer(const String& IP, unsigned short port, MYSQL* mysql);

		NetworkClient* onClientConnected(NetworkInterface *iFace);


		__inline void loadEncryption() {
			GenerateCryptTables(this->getCryptInfo().table, ROSEServer::DEFAULT_ENCRYPTION_KEY);
		}

		__inline NPCSTB* getNPCSTB() const {
			return this->npcFile.get();
		}

		__inline Map* getMap(const byte_t mapId) {
			return (this->maps.size() > mapId ? this->maps[mapId] : nullptr);
		}

		__inline Telegate* getTelegate(const word_t id) {
			return (this->telegates.count(id) > 0 ? this->telegates[id] : nullptr);
		}

		virtual ~WorldServer();

		void onRequestsFinished();
		void onClientDisconnect(NetworkInterface* iFace);
};

#endif 