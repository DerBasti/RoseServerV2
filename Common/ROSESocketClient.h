#ifndef __ROSE_SOCKET_CLIENT__
#define __ROSE_SOCKET_CLIENT__

#include "D:\Programmieren\BasicObject\NetworkServer.h"
#include "rosecrypt.h"
#include "datatypes.h"

class ROSESocketClient : public NetworkClient {
private:
	CryptInfo crypt;
	Packet packet;
protected:
	ROSESocketClient() {}

	void onDataReceived(int length);
	__inline Packet& getPacket() {
		return this->packet;
	}
	__inline CryptInfo getCryptInfo() {
		return crypt;
	}
public:
	ROSESocketClient(NetworkInterface* iFace, const CryptInfo& cryptInfo) : NetworkClient(iFace) {
		this->crypt.status.additionalValuePerStep = rand() | (rand() << 16);
		this->crypt = cryptInfo;
	}
	virtual ~ROSESocketClient() {
		this->packet = nullptr;
	}
	bool sendPacket(const Packet& p);
	virtual bool handlePacket() {
		return true;
	}

};

#endif //