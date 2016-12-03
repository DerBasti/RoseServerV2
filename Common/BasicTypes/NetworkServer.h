#ifndef __NETWORK_SERVER__
#define __NETWORK_SERVER__

#include <thread>

#if defined(_GLIBCXX_HAS_GTHREADS) || defined(_MSC_VER)
	#ifndef __THREADS_DEFINED__
	#define __THREADS_DEFINED__
	#endif //__THREADS_DEFINED__
#endif

#include "BasicObject.h"
#include "SharedArrayPointer.h"

class NetworkStartup : public BasicObject {
	public:
		NetworkStartup();
		virtual ~NetworkStartup();
};

typedef __w64 unsigned int SOCKET_TYPE;

class NetworkInterface : public BasicObject {
	private:
		SOCKET_TYPE socket;
		String ip;
		unsigned short port;
		bool isSocketActive;
	protected:
		SOCKET_TYPE FAULTY_SOCKET() const;
		__inline void setIp(const char *newIp) {
			this->setIp(String(newIp));
		}
		__inline void setIp(const String& ip) {
			this->ip = ip;
		}
		__inline void setPort(const unsigned short port) {
			this->port = port;
		}
		__inline void setSocket(const __w64 unsigned int sock) {
			this->socket = sock;
		}

		void moveSocketFrom(NetworkInterface* ni);
		bool establishSocket();
		void closeSocket();
	public:
		static struct sockaddr_in GetSockaddrFromIpAndPort(String ip, unsigned int port);
		NetworkInterface() {
			this->socket = FAULTY_SOCKET();
			this->ip = String();
			this->port = 0;
			this->isSocketActive = false;
		}
		NetworkInterface(const char *_ip, unsigned short _port) : NetworkInterface() {
			this->ip = _ip;
			this->port = _port;
		}
		NetworkInterface(String ip, unsigned short port) : NetworkInterface(ip.toConstChar(), port) {}
		NetworkInterface(SOCKET_TYPE sock, sockaddr_in *addr);

		virtual ~NetworkInterface() {
			this->closeSocket();
		}
		__inline bool operator==(const NetworkInterface& ni) const {
			return this->getSocket() == ni.getSocket();
		}

		__inline String getIp() const {
			return this->ip;
		}

		__inline unsigned short getPort() const {
			return this->port;
		}

		__inline SOCKET_TYPE getSocket() const {
			return this->socket;
		}

		__inline bool isSocketValid() const {
			return (this->getSocket() != static_cast<SOCKET_TYPE>(FAULTY_SOCKET()));
		}

		__inline bool isActive() const {
			return this->isSocketActive;
		}

		__inline void setActiveFlag(const bool value) {
			this->isSocketActive = value;
		}
};

#ifndef IP_AND_PORT_TOSTRING
#define IP_AND_PORT_TOSTRING this->getIp() + String(":") + String::fromInt(this->getPort())
#endif

class NetworkClient : public NetworkInterface {
	private:
		const static unsigned int DEFAULT_PACKETBUFFER_SIZE = 0x400;

		void connect();
	protected:
		SharedArrayPtr<char> packetBuffer;
		unsigned int lastPacketSize;
		unsigned int forcedOffset;

		NetworkClient() {
			this->lastPacketSize = forcedOffset = 0;
			packetBuffer = SharedArrayPtr<char>(new char[DEFAULT_PACKETBUFFER_SIZE], DEFAULT_PACKETBUFFER_SIZE);
			memset(this->packetBuffer.get(), 0x00, DEFAULT_PACKETBUFFER_SIZE);
			this->logger = Logger(this);
			this->setActiveFlag(false);
		}

		bool receiveDataImpl(unsigned int bufferLength, unsigned int offset);

		//Set an offset which is used once.
		__inline void setForcedOffset(const unsigned int off) {
			this->forcedOffset = off;
		}
	public:
		explicit NetworkClient(SOCKET_TYPE sock, struct sockaddr_in addr);
		explicit NetworkClient(SOCKET_TYPE sock, struct sockaddr_in *addr);

		explicit NetworkClient(NetworkInterface& iFace) : NetworkClient(&iFace) {}
		explicit NetworkClient(NetworkInterface* iFace) : NetworkClient() {
			this->setIp(iFace->getIp());
			this->setPort(iFace->getPort());
			this->moveSocketFrom(iFace);
		}

		NetworkClient(const char *serverIp, const unsigned short serverPort) : NetworkClient(String(serverIp), serverPort) {}
		NetworkClient(const String& serverIp, const unsigned short serverPort) : NetworkClient() {

			this->setIp(String(serverIp));
			this->setPort(serverPort);

			if(this->establishSocket()) {
				this->connect();
			}
		}

		virtual ~NetworkClient() {
		}

		void disconnect() {
			this->logger.info(String("Disconnecting from Server: ") + this->getIp() + String(":") + String::fromInt(this->getPort()));
			this->closeSocket();
		}

		__inline bool receiveData() {
			return this->receiveData(this->getForcedOffset());
		}

		__inline bool receiveData(unsigned int offset) {
			return this->receiveData(this->getMaxDataLength(), offset);
		}

		__inline bool receiveData(unsigned int bufferLength, unsigned int offset) {
			return this->receiveDataImpl(bufferLength, offset);
		}

		bool sendData(const char *data, unsigned long len);

		__inline const char *getData() const {
			return this->packetBuffer.get();
		}

		__inline unsigned long getMaxDataLength() const {
			return this->packetBuffer.getSize();
		}

		__inline unsigned int getLastPacketLength() const {
			return this->lastPacketSize;
		}

		__inline unsigned int getForcedOffset() const {
			return this->forcedOffset;
		}

		virtual void onReceiveCheckOffset() {
			//Set an offset in case it's necessary
		}

		virtual void onDataReceived(int length) {
			this->logger.debug(String("Received a total of ") + String::fromInt(length) + String(" bytes."));
		}

		virtual void onDataSend(const char *data, unsigned int length) {
			this->logger.debug(String("Sending a total of ") + String::fromInt(length) + String(" bytes."));
		}

		virtual void onDataSendFinished(bool successState, unsigned int length) {
			this->logger.debug(String("Sent ") + String::fromInt(length) + String(" bytes: ") + String::fromBool(successState));
		}
};

class NetworkServer : public NetworkInterface {
	protected:
		std::vector<NetworkClient*> clients;
		NetworkServer() {}
	private:
#ifdef __THREADS_DEFINED__
		std::thread runnerThread;
#endif //__THREADS_DEFINED__
		SOCKET_TYPE maxFileDescriptors;

		bool setupSocket();
		void fillFDS(struct fd_set *set);
		void addNewClient(SOCKET_TYPE sock, struct sockaddr_in* addr);

		void disconnectClient(NetworkClient* ni, std::vector<NetworkClient*>::const_iterator& it) {

			this->logger.info(String("Disconnecting interface from IP: ") + ni->getIp());
			this->onClientDisconnect(ni);

			it = clients.erase(it);

			delete ni;
			ni = nullptr;
		}

		void handleClients(struct fd_set* fds);
public:
		NetworkServer(const char *serverIp, unsigned short port) : NetworkServer(String(serverIp), port) {
		}
		NetworkServer(const String& serverIp, unsigned short port);

		virtual ~NetworkServer();

		void startWithExtraThread() {
#ifdef __THREADS_DEFINED__
			this->runnerThread = std::thread(&NetworkServer::start, this);
#endif //__THREADS_DEFINED__
		}

		void start();

		virtual void onStartup() {

		}
		virtual NetworkClient* onClientConnected(NetworkInterface* iFace) {
			this->logger.debug(String("Client connected from: ") + iFace->getIp());
			auto client = new NetworkClient(iFace);
			return client;
		}
		virtual void onIncomingData(NetworkClient* client) {
			this->logger.debug(String("Incoming message from client: ") + client->getIp());
		}
		virtual void onClientDataReceived(NetworkClient* client) {
			//this->logger.debug(String("Message from client ") + client->getIp() + String(" says: ") + client->getData());
		}
		virtual void onClientDisconnect(NetworkInterface* iFace) {
			this->logger.debug(String("Client from IP ") + iFace->getIp() + String(" wants to disconnect."));
		}
		virtual void onRequestsFinished() {
		}
};

#endif //__NETWORK_SERVER__
