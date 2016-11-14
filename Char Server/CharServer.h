#pragma once

#ifndef __ROSE_CHAR_SERVER__
#define __ROSE_CHAR_SERVER__

#include "..\Common\ROSESocketServer.h"
#include "CharClient.h"

class CharServer : public ROSEServer {
	public:
		CharServer(String ip, WORD port, MYSQL* mysql);
		~CharServer();
		void loadEncryption(); 
		NetworkClient* onClientConnected(NetworkInterface *iFace);

		bool updateLastPlayedChar(const unsigned long accId, const unsigned long charId);

		bool addCharacter(const unsigned long accId, Character& newChar);
		bool addDefaultStats(const Character& newChar);
		bool addDefaultItems(const Character& newChar);
		bool addDefaultSkills(const Character& newChar);
		bool addDefaultQuestInfos(const Character& newChar);
};

#endif //__ROSE_CHAR_SERVER__