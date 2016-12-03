#ifndef __ROSE_SOCKET_CLIENT__
#define __ROSE_SOCKET_CLIENT__

#include "BasicTypes\NetworkServer.h"
#include "rosecrypt.h"
#include "datatypes.h"

class ROSESocketClient : public NetworkClient {
private:
	CryptInfo crypt;
	Packet packet; 
	std::function<bool(const Packet&)> onHandlePacketTrigger;
protected:
	ROSESocketClient() {}

	void onDataReceived(int length);
	__inline CryptInfo getCryptInfo() {
		return crypt;
	}

	__inline Packet& getPacket() {
		return this->packet;
	}
public:
	ROSESocketClient(NetworkInterface* iFace, const CryptInfo& cryptInfo) : NetworkClient(iFace) {
		this->crypt.status.additionalValuePerStep = rand() | (rand() << 16);
		this->crypt = cryptInfo;
	}
	virtual ~ROSESocketClient() {
		this->packet = nullptr;
	}

	__inline void setOnHandlePacket(std::function<bool(const Packet&)> f) {
		this->onHandlePacketTrigger = (f == nullptr ? [](const Packet&) -> bool { return false; } : f);
	}

	bool sendPacket(const Packet& p);

	virtual bool handlePacket(const Packet& pak) {
		return this->onHandlePacketTrigger(pak);
	}

};

#endif //