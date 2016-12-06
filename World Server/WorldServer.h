#ifndef __ROSE_WORLDSERVER__
#define __ROSE_WORLDSERVER__

#ifdef _MSC_VER
#pragma once
#endif

#include "..\Common\ROSESocketServer.h"
#include "FileTypes\STB.h"
#include "FileTypes\VFS.h"
#include "FileTypes\CHR.h"
#include "FileTypes\AIP.h"
#include "FileTypes\QSD.h"
#include "Map.h"

class WorldServer : public ROSEServer {
	private:
		std::shared_ptr<VFS> vfs;
		std::shared_ptr<NPCSTB> npcFile;
		std::shared_ptr<AISTB> aiFile;
		std::shared_ptr<SkillSTB> skillFile;
		std::shared_ptr<StatusSTB> statusFile;
		std::shared_ptr<STB> questFile;
		std::shared_ptr<EquipmentSTB> equipmentFile[15]; //0 = NOT VALID
		std::shared_ptr<STB> craftingFile;
		std::shared_ptr<STB> sellFile;
		std::shared_ptr<ZoneSTB> zoneFile;
		std::shared_ptr<STB> motionFile;
		std::shared_ptr<STB> dropFile;
		std::shared_ptr<STB> warpFile;

		std::shared_ptr<CHR> chrFile;
		std::vector<ZMO*> playerAnimations;

		std::vector<Map*> maps;
		std::map<word_t, AIP*> aiData;

		std::vector<QSD*> questFiles;
		std::map<dword_t, QSD::Record*> questData;

		bool multiThreadedStart;
		std::vector<std::thread> runningThreads;
		std::map<SOCKET, class Player*> interfaceMapping;

		template<class _T> void loadSTB(std::shared_ptr<_T>& ptr, const char *path, bool applySTL = true) {
			this->logger.info(String("Loading STB: ") + String(path));
			String pathAsString = String(path);	
			if (!pathAsString.isEmpty()) {
				auto entry = VFS::get()->getEntry(pathAsString);
				ptr = std::shared_ptr<_T>(new _T(String(pathAsString), entry.getContent(), applySTL));
			}
		}

		void loadAI();
		void loadZoneData();
		void loadIFOs(Map *currentMap, std::vector<VFS::Entry>& ifoFiles);
		void loadQuestData();
		void loadAnimationData();

		void doMapActions(Map* map);
	public:
		WorldServer(const char* IP, unsigned short port, MYSQL* mysql) : WorldServer(String(IP), port, mysql) {}
		WorldServer(const String& IP, unsigned short port, MYSQL* mysql);

		virtual ~WorldServer();

		void onRequestsFinished();
		void onClientDisconnect(NetworkInterface* iFace);
		void onStartup();
		NetworkClient* onClientConnected(NetworkInterface *iFace);

		__inline void loadEncryption() {
			GenerateCryptTables(this->getCryptInfo().table, ROSEServer::DEFAULT_ENCRYPTION_KEY);
		}

		__inline void setMultiThreaded() {
			this->multiThreadedStart = true;
		}

		__inline EquipmentSTB* getEquipmentSTB(const byte_t itemType) {
			return (itemType == 0 || itemType > ItemType::PAT) ? nullptr : this->equipmentFile[itemType].get();
		}

		__inline CHR* getCHRFile() const {
			return this->chrFile.get();
		}

		__inline ZMO* getNPCAnimation(const word_t typeId, const byte_t motionType) const {
			return this->chrFile->getMotion(typeId, motionType);
		}

		__inline ZMO* getPlayerMotion(const word_t animationId, const byte_t motionType) const {
			word_t motionId = this->motionFile->getEntry(animationId)->get(motionType).toUShort();
			return this->playerAnimations[motionId];
		}

		__inline NPCSTB* getNPCSTB() const {
			return this->npcFile.get();
		}

		__inline Map* getMap(const byte_t mapId) {
			return (this->maps.size() > mapId ? this->maps[mapId] : nullptr);
		}

		__inline AIP* getAIData(const word_t id) {
			return (this->aiData.count(id) > 0 ? this->aiData[id] : nullptr);
		}

		__inline QSD::Record* getQuestByHash(const dword_t hash) const {
			return this->questData.count(hash) > 0 ? this->questData.at(hash) : nullptr;
		}
};

#endif 