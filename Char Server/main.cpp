#include "CharServer.h"
#include "D:\Programmieren\BasicObject\File.h"
#include "D:\Programmieren\BasicObject\Config.h"

int main() {
	MYSQL mysql;
	String workPath = File::GetWorkingDirectory();
	Config cfg(workPath + String("config.conf"));
	CharServer server(cfg.get("ChannelIp", "127.0.0.1"), cfg.get("ChannelPort", "29100").toInt(), &mysql);
	server.loadEncryption();
	server.start();
}