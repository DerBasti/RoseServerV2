#ifndef __ROSE_SOCKET_SERVER__
#define __ROSE_SOCKET_SERVER__

#ifdef _MSC_VER
#pragma once
#endif

#include "..\Common\PacketIDs.h"
#include "..\Common\BasicTypes\NetworkServer.h"
#include "..\Common\BasicTypes\StringWrapper.h"
#include "..\Common\BasicTypes\Config.h"
#include "..\Common\BasicTypes\File.h"
#include "..\Common\rosecrypt.h"

#include <mysql.h>
#include <type_traits>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "libmysql")

class ROSEServer : public NetworkServer {
private:
	Config config;
	DataBase *database;
	CryptInfo crypt;
protected:
	static ROSEServer *server;
	__inline CryptInfo& getCryptInfo() {
		return crypt;
	}
	__inline Config& getConfig() {
		return this->config;
	}
public:
	static const unsigned int DEFAULT_ENCRYPTION_KEY = 0x87654321;
	ROSEServer(const char* IP, unsigned short port, MYSQL* mysql) : ROSEServer(String(IP), port, mysql) {}
	ROSEServer(const String& IP, unsigned short port, MYSQL* mysql) : NetworkServer(IP, port) {
		this->logger = Logger(this);
		String currentPath = File::GetWorkingDirectory();
		config = Config(currentPath + String("config.conf"));
		this->database = new DataBase(__DATABASE_HOST__, __DATABASE_USERNAME__, __DATABASE_PASSWORD__, __DATABASE_DBNAME__, __DATABASE_PORT__, mysql);
		this->getDB()->setAutoCommit(false);
		server = this;
	}
	virtual ~ROSEServer() {
		delete this->database;
		this->database = nullptr;
		server = nullptr;
	}
	virtual NetworkClient* onClientConnected(NetworkInterface *iFace) {
		return nullptr;
	}

	__inline static DataBase* getDatabase() {
		return server != nullptr ? server->getDB() : nullptr;
	}
	__inline DataBase* getDB() {
		return this->database;
	}

	template<class _T = ROSEServer, class = typename std::enable_if<std::is_base_of<ROSEServer, _T>::value>::type> __inline static _T* getServer() {
		return static_cast<_T*>(server);
	}
};

#endif //__ROSE_SOCKET_SERVER__