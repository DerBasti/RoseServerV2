#pragma once

#ifndef __ROSE_LOGIN_SERVER__
#define __ROSE_LOGIN_SERVER__

#include "..\Common\ROSESocketServer.h"

class LoginServer : public ROSEServer {
	private:
		bool checkForServer(NetworkInterface* clients, std::string& ip);
	public:
		LoginServer(const char* IP, unsigned short port, MYSQL* mysql) : LoginServer(String(IP), port, mysql) {

		}
		LoginServer(const String& IP, unsigned short port, MYSQL* mysql) : ROSEServer(IP, port, mysql) {

		}
		~LoginServer();
		const static BYTE DEFAULT_ACCESSLEVEL = 0x00;
		NetworkClient* onClientConnected(NetworkInterface* ni);

		void loadEncryption() { ::GenerateLoginServerCryptTables(this->getCryptInfo().table); }

};

#endif //__ROSE_LOGIN_SERVER__