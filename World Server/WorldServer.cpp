#include "WorldServer.h"
#include "Entities\Player.h"

WorldServer::WorldServer(const String& IP, unsigned short port, MYSQL* mysql) : ROSEServer(IP, port, mysql) {
	vfs = std::shared_ptr<VFS>(new VFS(this->getConfig().get("GameFolder"), true));

	this->loadSTB<NPCSTB>(npcFile, "3DDATA\\STB\\LIST_NPC.STB");
	this->loadSTB<AISTB>(aiFile, "3DDATA\\STB\\FILE_AI.STB");
	this->loadSTB<SkillSTB>(skillFile, "3DDATA\\STB\\LIST_SKILL.STB");
	this->loadSTB<StatusSTB>(statusFile, "3DDATA\\STB\\LIST_STATUS.STB");
	this->loadSTB<STB>(questFile, "3DDATA\\STB\\LIST_QUESTDATA.STB", false);

	const char* equipFileNames[] = { nullptr, "3DDATA\\STB\\LIST_FACEITEM.STB",
		"3DDATA\\STB\\LIST_CAP.STB", "3DDATA\\STB\\LIST_BODY.STB",
		"3DDATA\\STB\\LIST_ARMS.STB", "3DDATA\\STB\\LIST_FOOT.STB",
		"3DDATA\\STB\\LIST_BACK.STB", "3DDATA\\STB\\LIST_JEWEL.STB",
		"3DDATA\\STB\\LIST_WEAPON.STB", "3DDATA\\STB\\LIST_SUBWPN.STB",
		"3DDATA\\STB\\LIST_USEITEM.STB", "3DDATA\\STB\\LIST_JEMITEM.STB",
		"3DDATA\\STB\\LIST_NATURAL.STB", "3DDATA\\STB\\LIST_QUESTITEM.STB",
		"3DDATA\\STB\\LIST_PAT.STB"
	};
	for (unsigned int i = 1; i < 15; i++) {
		this->loadSTB<EquipmentSTB>(this->equipmentFile[i], equipFileNames[i]);
	}
	//this->loadSTB<STB>(craftingFile;
	//this->loadSTB<STB>(sellFile;
	this->loadSTB<ZoneSTB>(zoneFile, "3DDATA\\STB\\LIST_ZONE.STB");
	this->loadSTB<STB>(motionFile, "3DDATA\\STB\\TYPE_MOTION.STB", false);
	this->loadSTB<STB>(dropFile, "3DDATA\\STB\\ITEM_DROP.STB");
	this->loadSTB<STB>(warpFile, "3DDATA\\STB\\WARP.STB");

	this->loadAI();
	this->loadQuestData();
	this->loadZoneData();
	this->loadAnimationData();
}

WorldServer::~WorldServer() {
	std::for_each(this->maps.begin(), this->maps.end(), [](Map* map) {
		delete map;
		map = nullptr;
	});
	std::for_each(this->aiData.begin(), this->aiData.end(), [](std::pair<const word_t, AIP*>& pair) {
		delete pair.second;
		pair.second = nullptr;
	});
	std::for_each(this->playerAnimations.begin(), this->playerAnimations.end(), [](ZMO* zmo) {
		delete zmo;
		zmo = nullptr;
	});
	this->playerAnimations.clear();
	this->maps.clear();
	this->aiData.clear();
}

void WorldServer::loadAI() {
	this->logger.info("Loading AI...");
	for (unsigned int i = 0; i < aiFile->getEntryAmount(); i++) {
		String file = aiFile->getAIPath(i);
		auto vfsEntry = vfs->getEntry(file);
		if (vfsEntry.getContent().getSize()>0) {
			this->aiData[i] = new AIP(vfsEntry.getPathInVFS(), vfsEntry);
		}
	}
}

void WorldServer::loadAnimationData() {
	this->logger.info("Loading NPC animations...");
	//NPCs
	VFS::Entry& entry = vfs->getEntry("3DDATA\\NPC\\LIST_NPC.CHR");
	this->chrFile = std::shared_ptr<CHR>(new CHR(entry.getPathInVFS(), entry));

	this->logger.info("Loading player animations...");
	//Player
	auto& fileMotionEntry = vfs->getEntry("3DDATA\\STB\\FILE_MOTION.STB");
	STB stbMotionFile(fileMotionEntry.getPathInVFS(), fileMotionEntry.getContent());
	for (dword_t i = 0; i < stbMotionFile.getEntryAmount(); i++) {
		String motionPath = stbMotionFile.getEntry(i)->get(0);
		this->playerAnimations.push_back(new ZMO(motionPath, vfs->getEntry(motionPath)));
	}
}

void WorldServer::onStartup() {
	if (this->multiThreadedStart) {
		for (unsigned int i = 0; i < this->maps.size(); i++) {
			Map* map = this->maps[i];
			if (map->getZoneData() != nullptr) {
				this->runningThreads.push_back(std::thread(&WorldServer::doMapActions, this, map));
			}
		}
	}
}

void WorldServer::loadZoneData() {
	this->logger.info("Loading zone data...");
	std::map<word_t, std::vector<VFS::Entry>> zoneFoldersWithIFOFiles;
	for (unsigned int i = 0; i < zoneFile->getEntryAmount(); i++) {
		String zoneFileName = zoneFile->getZoneFile(i);
		auto entry = VFS::get()->getEntry(zoneFileName);
		ZON* zoneData = nullptr;
		if (entry.getContent().getSize() > 0) {
			zoneData = new ZON(zoneFileName, entry.getContent());
			zoneData->setSectorSize(this->zoneFile->getZoneSize(i));
		}
		String zoneFolder = zoneFileName.substring(0, zoneFileName.findLastOf("\\") + 1);
		std::vector<VFS::Entry> ifoFiles = this->vfs->getEntriesFromPath(zoneFolder, String(".ifo"));
		zoneFoldersWithIFOFiles[i] = ifoFiles;
		this->maps.push_back(new Map(i, zoneData, ifoFiles));
	}
	for (unsigned int i = 0; i<zoneFile->getEntryAmount(); i++) {
		this->loadIFOs(this->maps[i], zoneFoldersWithIFOFiles[i]);
	}
}

void WorldServer::loadIFOs(Map *currentMap, std::vector<VFS::Entry>& ifoFiles) {
	std::for_each(ifoFiles.begin(), ifoFiles.end(), [&](VFS::Entry& entry) {
		IFO ifo(entry.getPathInVFS(), entry.getContent());
		std::for_each(ifo.getSpawns().begin(), ifo.getSpawns().end(), [&](IFO::Spawn spawn) {
			currentMap->addSpawn(spawn);
		});
		std::for_each(ifo.getNPCs().begin(), ifo.getNPCs().end(), [&](IFO::NPC npc) {
			currentMap->addNPC(npc);
		});

		std::for_each(ifo.getTelegateSources().begin(), ifo.getTelegateSources().end(), [&](IFO::TelegateSource& tele) {
			STB::Entry* row = this->warpFile->getEntry(tele.getUnknownValue());
			byte_t destMapId = row->get(0x01).toByte();
			String gateName = row->get(0x02);
			Map* destMap = this->getMap(destMapId);
			const ZON::EventInformation* event = nullptr;
			if (destMap && destMap->isValid()) {
				SingleTelegate src(tele.getPosition(), currentMap->getId());
				event = destMap->getZoneData()->getEvent(gateName);
				if (event == nullptr) {
					event = destMap->getZoneData()->getEvent("start");
				}
				SingleTelegate dest(event->getPosition(), destMapId);
				currentMap->addTelegate(tele.getUnknownValue(), src, dest);
			}
		});
	});
}

void WorldServer::loadQuestData() {
	this->questFiles.reserve(questFile->getEntryAmount());
	this->logger.info("Loading quest data...");
	for (dword_t i = 0; i < questFile->getEntryAmount(); i++) {
		String questFilePath = questFile->getEntry(i)->get(0);
		if (questFilePath.isEmpty()) {
			continue;
		}
		auto entry = vfs->getEntry(questFilePath);
		QSD* questData = new QSD(static_cast<word_t>(i), questFilePath, entry);
		this->questFiles.push_back(questData);

		auto questEntries = questData->getQuestEntries();
		for (dword_t j = 0; j < questData->getQuestEntryAmount(); j++, questEntries++) {
			auto entry = (*questEntries);
			auto records = entry->getRecords();
			for (dword_t k = 0; k < entry->getRecordAmount(); k++, records++) {
				auto currentRecord = (*records);
				this->questData.insert(std::pair<dword_t, QSD::Record*>(currentRecord->getQuestHash(), currentRecord));
			}
		}
	}
}

NetworkClient* WorldServer::onClientConnected(NetworkInterface *iFace) {
	Player* player = new Player(iFace, this->getCryptInfo());
	this->interfaceMapping[player->getNetworkInterface()->getSocket()] = player;
	return player->getNetworkInterface();
}

void WorldServer::onRequestsFinished() {
	if (!this->multiThreadedStart) {
		Map** mapData = this->maps.data();
		for (unsigned int i = 0; i < this->maps.size(); i++, mapData++) {
			this->doMapActions(*mapData);			
		}
	}
}

void WorldServer::doMapActions(Map* map) {
	while (this->isActive()) {
		if (map->isActive()) {
			auto entitiesOnMap = map->getAllEntitiesOnMap();
			for (auto it = entitiesOnMap.begin(); it != entitiesOnMap.end();) {
				auto currentEntity = it->second;
				if (!currentEntity->isActive()) {

					it = entitiesOnMap.erase(it);
					map->removeEntity(currentEntity);

					delete currentEntity;
					currentEntity = nullptr;

					continue;
				}
				if (currentEntity->isPlayer()) {
					currentEntity = currentEntity;
				}

				currentEntity->movementProc();
				map->updateEntity(currentEntity);
				currentEntity->doAction();
				it++;
			}
			map->clearInvalidEntities();
		}
		Sleep(20);
	}
}


void WorldServer::onClientDisconnect(NetworkInterface* iFace) {
	auto p = this->interfaceMapping[iFace->getSocket()];
	if (p) {
		p->invalidateNetworkInterface();
		this->interfaceMapping.erase(iFace->getSocket());
	}
}