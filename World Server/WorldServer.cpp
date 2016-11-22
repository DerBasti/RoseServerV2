#include "WorldServer.h"
#include "Entities\Player.h"

WorldServer::WorldServer(const String& IP, unsigned short port, MYSQL* mysql) : ROSEServer(IP, port, mysql) {
	vfs = std::shared_ptr<VFS>(new VFS(this->getConfig().get("GameFolder"), true));

	this->loadSTB<NPCSTB>(npcFile, "3DDATA\\STB\\LIST_NPC.STB");
	this->loadSTB<AISTB>(aiFile, "3DDATA\\STB\\FILE_AI.STB");
	this->loadSTB<SkillSTB>(skillFile, "3DDATA\\STB\\LIST_SKILL.STB");
	this->loadSTB<StatusSTB>(statusFile, "3DDATA\\STB\\LIST_STATUS.STB");
	this->loadSTB<STB>(questFile, "3DDATA\\STB\\LIST_QUEST.STB", false);

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
		this->loadSTB<STB>(this->equipmentFile[i], equipFileNames[i]);
	}
	//this->loadSTB<STB>(craftingFile;
	//this->loadSTB<STB>(sellFile;
	this->loadSTB<ZoneSTB>(zoneFile, "3DDATA\\STB\\LIST_ZONE.STB");
	this->loadSTB<STB>(motionFile, "3DDATA\\STB\\TYPE_MOTION.STB", false);
	this->loadSTB<STB>(dropFile, "3DDATA\\STB\\ITEM_DROP.STB");
	this->loadSTB<STB>(warpFile, "3DDATA\\STB\\WARP.STB");

	this->loadAI();
	this->loadZoneData();
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

NetworkClient* WorldServer::onClientConnected(NetworkInterface *iFace) {
	return new Player(iFace, this->getCryptInfo());
}

void WorldServer::onRequestsFinished() {
	std::for_each(this->maps.begin(), this->maps.end(), [this](Map* map) {
		if (map->isActive()) {
			std::for_each(map->beginEntities(), map->endEntities(), [&](std::pair<const word_t, Entity*>& p) {
				auto currentEntity = p.second;
				currentEntity->movementProc();
				map->updateEntity(currentEntity);
				currentEntity->doAction();
			});
		}
	});
}

void WorldServer::onClientDisconnect(NetworkInterface* iFace) {
	for (NetworkClient* client : clients) {
		Player* player = static_cast<Player*>(client);
		player->getVisuality()->forceClear();
		Map* map = player->getPositionInformation()->getMap();
		if (map != nullptr) {
			map->removeEntity(player);
		}
		player->getPositionInformation()->setMap(nullptr);
	}
}