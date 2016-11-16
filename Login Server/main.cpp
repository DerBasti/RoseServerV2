#include "LoginClient.h"
#include "LoginServer.h"
#include "..\Common\BasicTypes\Config.h"

int main() {
	MYSQL mysql;
	String path = File::GetWorkingDirectory();
	Config config(path + String("config.conf"));
	LoginServer server(config.get("LoginIp", "127.0.0.1"), config.get("LoginPort", "29000").toInt(), &mysql);
 	server.loadEncryption();
	server.start();
}