
#include "WorldServer.h"
#include "FileTypes\VFS.h"
#include "FileTypes\IFO.h"
#include "FileTypes\AIP.h"

int main() {
	/*VFS vfs("D:\\Games\\iRose Online\\", true);
	auto zoneSTBData = vfs.getEntry("3DDATA\\STB\\LIST_ZONE.STB");
	ZoneSTB zoneFileBasic(zoneSTBData.getPathInVFS(), zoneSTBData.getContent());
	ZoneSTB *zoneFile = &zoneFileBasic;
	for (unsigned int i = 0; i < zoneFile->getEntryAmount(); i++) {
		String zoneFileName = zoneFile->getZoneFile(i);
		auto entry = VFS::get()->getEntry(zoneFileName);
		ZON* zoneData = nullptr;
		if (entry.getContent().getSize() > 0) {
			zoneData = new ZON(zoneFileName, entry.getContent());
		}
		String zoneFolder = zoneFileName.substring(0, zoneFileName.lastPositionOf("\\") + 1);
		std::vector<VFS::Entry> ifoFiles = vfs.getEntriesFromPath(zoneFolder, String(".ifo"));
		std::for_each(ifoFiles.begin(), ifoFiles.end(), [](VFS::Entry& ifo) {
			IFO ifoData(ifo.getPathInVFS(), ifo.getContent());
		});
	}
	*/
	MYSQL mysql;
	String workPath = File::GetWorkingDirectory();
	Config cfg(workPath + String("config.conf"));
	WorldServer ws(cfg.get("WorldIp", "127.0.0.1"), cfg.get("WorldPort", "29200").toInt(), &mysql);
	ws.loadEncryption();
	ws.start();
	return 0;
}