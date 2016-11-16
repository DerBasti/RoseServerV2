#include "WorldServer.h"
#include "Entities\Player.h"

WorldServer::WorldServer(const String& IP, unsigned short port, MYSQL* mysql) : ROSEServer(IP, port, mysql) {
	srand(time(nullptr));
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

	for (unsigned int i = 0; i < zoneFile->getEntryAmount(); i++) {
		String zoneFileName = zoneFile->getZoneFile(i);
		auto entry = VFS::get()->getEntry(zoneFileName);
		ZON* zoneData = nullptr;
		if (entry.getContent().getSize() > 0) {
			zoneData = new ZON(zoneFileName, entry.getContent());
		}
		//String zoneFolder = zoneFileName.substring(0, zoneFileName.lastPositionOf("\\") + 1);
		//std::vector<VFS::Entry> ifoFiles; = this->vfs->getEntriesFromPath(zoneFolder, String(".ifo"));
		std::vector<IFO> ifos;
		this->maps.push_back(new Map(i, zoneData, ifos));
	}
}

WorldServer::~WorldServer() {

}

NetworkClient* WorldServer::onClientConnected(NetworkInterface *iFace) {
	return new Player(iFace, this->getCryptInfo());
}

void WorldServer::onRequestsFinished() {
	std::for_each(this->maps.begin(), this->maps.end(), [&](Map* map) {
		if (map->)
	});
}