#pragma once

#ifndef __ROSE_LOGIN_CLIENT__
#define __ROSE_LOGIN_CLIENT__

#include "LoginServer.h"
#include "..\Common\ROSESocketClient.h"

class LoginClient : public ROSESocketClient {
	private:
		struct _accountInfo {
			DWORD userId;
			DWORD accessLevel;
			String userName;
			String password;
		} accountInfo;

		LoginClient() {
			this->accountInfo.userId = 0x00; this->accountInfo.accessLevel = 1;
			this->accountInfo.userName = this->accountInfo.password = "";
		}

		void setBaseInfo(const WORD userId, const DWORD accessLevel, String& userNam, String& pw);

		bool pakIdentifyCharServer();
		bool pakEncryptionRequest();
		bool pakRequestChannelList();
		bool pakUserLogin();
		bool pakRequestChannelIP();

	public:
		LoginClient(NetworkInterface* iFace, const CryptInfo& cryptInfo) : ROSESocketClient(iFace, cryptInfo) {
		}
		~LoginClient() {
			//...nothing to do
		}

		bool handlePacket();
};

#endif //__ROSE_LOGIN_CLIENT__