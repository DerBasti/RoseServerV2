#ifndef __NETWORK_SERVER__
#define __NETWORK_SERVER__

#include <thread>

#if defined(_GLIBCXX_HAS_GTHREADS) || defined(_MSC_VER)
	#ifndef __THREADS_DEFINED__
	#define __THREADS_DEFINED__
	#endif //__THREADS_DEFINED__
#endif

#ifdef _WIN32
	#include <winsock2.h>
	#include <windows.h>

	#ifdef _MSC_VER
		#pragma comment(lib, "Ws2_32.lib")
	#endif
#else

#endif

#include "BasicObject.h"
#include "SharedArrayPointer.h"

class NetworkStartup : public BasicObject {
	public:
		NetworkStartup() {
			WSADATA wsa;
			if(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) == INVALID_SOCKET && WSAGetLastError() == WSANOTINITIALISED) {
				WSAStartup(MAKEWORD(2, 0), &wsa);
			}
		}
		~NetworkStartup() {
			WSACleanup();
		}
};

class NetworkInterface : public BasicObject {
	private:
		SOCKET socket;
		String ip;
		unsigned short port;
		bool isSocketActive;
	protected:
		SOCKET FAULTY_SOCKET() const {
			return static_cast<SOCKET>(SOCKET_ERROR);
		}
		__inline void setIp(const char *newIp) {
			this->setIp(String(newIp));
		}
		__inline void setIp(const String& ip) {
			this->ip = ip;
		}
		__inline void setPort(const unsigned short port) {
			this->port = port;
		}
		__inline void setSocket(const SOCKET sock) {
			this->socket = sock;
		}

		void moveSocketFrom(NetworkInterface* ni) {
			this->setSocket(ni->getSocket());
			ni->setSocket(FAULTY_SOCKET());
		}

		bool establishSocket() {
			this->logger.debug("Establishing socket...");
			this->socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(this->isSocketValid()) {
				this->logger.debug("Status: Successfully established.");
			} else {
				this->logger.warn(String("Status: Failed to establish socket. Error-Code: ") + String::fromInt(GetLastError()));
			}
			return this->isSocketValid();
		}

		__inline void closeSocket() {
			if(this->isSocketValid()) {
				this->logger.debug("Closing existing socket.");
				::closesocket(this->getSocket());
				this->socket = SOCKET_ERROR;
			}
		}
	public:
		static sockaddr_in GetSockaddrFromIpAndPort(String ip, unsigned int port) {
			sockaddr_in addr;
			addr.sin_port = port;
			addr.sin_addr.S_un.S_addr = inet_addr(ip.toConstChar());
			addr.sin_family = IPPROTO_TCP;
			return addr;
		}
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
		NetworkInterface(SOCKET sock, sockaddr_in *addr) : NetworkInterface(String(inet_ntoa(addr->sin_addr)), addr->sin_port) {
			this->setSocket(sock);
		}

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

		__inline SOCKET getSocket() const {
			return this->socket;
		}

		__inline bool isSocketValid() const {
			return (this->getSocket() != static_cast<UINT_PTR>(FAULTY_SOCKET()));
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

		void connect() {
			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.S_un.S_addr = ::inet_addr(this->getIp().toConstChar());
			addr.sin_port = htons(this->getPort());
			memset(&addr.sin_zero, '\0', 8);
			this->logger.info(String("Connecting towards: ") + this->getIp() + String(":") + String::fromInt(this->getPort()));
			this->setActiveFlag(::connect(this->getSocket(), reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in)) == 0);
			if(!this->isActive()) {
				this->logger.error(String("Couldn't connect to ") + this->getIp() + String(". Error-Code: ") + String::fromInt(GetLastError()));
			} else {
				this->logger.info(String("Successfully connected to: ") + IP_AND_PORT_TOSTRING);
			}
		}
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

		bool receiveDataImpl(unsigned int bufferLength, unsigned int offset) {
			this->setForcedOffset(0);
			if(offset >= this->getMaxDataLength()) {
				this->logger.warn(String("The offset of ") + String::fromInt(offset) + String(" exceeds the maximum capacity of ") +
						String::fromLong(this->getMaxDataLength()) + String(" bytes by ") +
						String::fromInt(offset - this->getMaxDataLength()) + String(". Receiving data is impossible."));
				return false;
			}
			int remainingSize = this->getMaxDataLength() - offset;
			if (remainingSize <= 0) {
				return false;
			}
			if(static_cast<unsigned int>(remainingSize) < bufferLength) {
				bufferLength = remainingSize - 1;
			}
			this->lastPacketSize = ::recv(this->getSocket(), &this->packetBuffer.get()[offset], bufferLength, 0x00);
			if (this->lastPacketSize == static_cast<unsigned int>(-1)) {
				this->logger.info(String("Connection from '") + this->getIp() + String("' suddenly aborted."));
				return false;
			}
			this->packetBuffer.get()[offset + this->lastPacketSize] = 0;
			if(this->getLastPacketLength() > 0) {
				this->lastPacketSize += offset;
				this->onDataReceived(this->getLastPacketLength());
			}
			return this->getLastPacketLength() > 0;
		}

		//Set an offset which is used once.
		__inline void setForcedOffset(const unsigned int off) {
			this->forcedOffset = off;
		}
	public:
		explicit NetworkClient(SOCKET sock, sockaddr_in addr) : NetworkClient(sock, &addr) {}
		explicit NetworkClient(SOCKET sock, sockaddr_in *addr) : NetworkClient(NetworkInterface(sock, addr)) {}

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

		bool sendData(const char *data, unsigned long len) {
			bool success = false;
			if(this->isSocketValid()) {
				this->onDataSend(data, len);
				success = (static_cast<unsigned long>(::send(this->getSocket(), data, len, 0x00)) == len);
				this->onDataSendFinished(success, len);
			}
			if(!success) {
				this->logger.warn(String("Failed to transmit data of length ") + String::fromInt(len));
			}
			return success;
		}

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

#ifndef SOMAXCONN
#define SOMAXCONN 128
#endif

class NetworkServer : public NetworkInterface {
	protected:
		std::vector<NetworkClient*> clients;
		NetworkServer() {}
	private:
#ifdef __THREADS_DEFINED__
		std::thread runnerThread;
#endif //__THREADS_DEFINED__
		SOCKET maxFileDescriptors;

		bool setupSocket() {
			unsigned int optVal = 1;
			if(::setsockopt(this->getSocket(), SOL_SOCKET, SO_KEEPALIVE, (const char*)&optVal, sizeof(optVal)) == SOCKET_ERROR) {
				this->logger.error(String("Setting socket options failed. Error-Code: ") + String::fromInt(GetLastError()));
				return false;
			}
			::setsockopt(this->getSocket(), IPPROTO_TCP, TCP_NODELAY, (const char*)&optVal, sizeof(optVal));
			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.S_un.S_addr = ::inet_addr(this->getIp().toConstChar());
			addr.sin_port = ::htons(this->getPort());
			memset(&addr.sin_zero, '\0', 8);
			return ::bind(this->getSocket(), (const sockaddr*)&addr, sizeof(struct sockaddr)) == 0;
		}

		void fillFDS(fd_set *set) {
			std::for_each(clients.begin(), clients.end(), [&, this](NetworkClient* ni) {
				if(ni->isActive()) {
					FD_SET(ni->getSocket(), set);
					if(ni->getSocket() > maxFileDescriptors) {
						maxFileDescriptors = ni->getSocket();
					}
				} else {
					this->disconnectClient(ni);
				}
			});
		}

		void addNewClient(SOCKET sock, sockaddr_in* addr) {
			NetworkInterface ni(sock, addr);
			NetworkClient* client = this->onClientConnected(&ni);
			this->logger.info(String("Adding new client with IP: ") + ni.getIp());
			client->setActiveFlag(true);
			this->clients.push_back(client);
		}

		void disconnectClient(NetworkClient* ni) {
			for(unsigned int i=0;i<clients.size();i++) {
				if(clients[i] == ni) {
					this->logger.debug("Found to-disconnect interface in client list.");
					clients.erase(clients.begin()+i);
					break;
				}
			}
			this->logger.info(String("Disconnecting interface from IP: ") + ni->getIp());
			this->onClientDisconnect(ni);

			delete ni;
			ni = nullptr;
		}

		void handleClients(FD_SET* fds) {
			std::for_each(clients.begin(), clients.end(), [=](NetworkClient* ni) {
				if(!ni->isActive()) {
					return;
				}
				if(FD_ISSET(ni->getSocket(), fds)) {
					this->logger.debug(String("Client from IP ") + ni->getIp() + String(" sent data to retrieve."));
					this->onIncomingData(ni);
					ni->setActiveFlag(ni->receiveData(ni->getForcedOffset()));
					this->onClientDataReceived(ni);
				}
			});
		}
public:
		NetworkServer(const char *serverIp, unsigned short port) : NetworkServer(String(serverIp), port) {
		}
		NetworkServer(const String& serverIp, unsigned short port) : NetworkInterface(serverIp, port) {
			this->logger = Logger(this);
			if (this->establishSocket() && this->setupSocket()) {
				if (::listen(this->getSocket(), SOMAXCONN) == SOCKET_ERROR) {
					this->logger.error(String("Listening on Socket failed. Reason: ") + String::fromInt(GetLastError()));
				}
				else {
					this->logger.info(String("Listening on: ") + IP_AND_PORT_TOSTRING);
				}
			}
			else {
				this->logger.error(String("Socket couldn't be setup. Error-Code: ") + String::fromInt(GetLastError()));
			}
			this->maxFileDescriptors = FAULTY_SOCKET();
		}

		virtual ~NetworkServer() {
			std::for_each(clients.begin(), clients.end(), [](NetworkClient* ni) {
				::closesocket(ni->getSocket());
				delete ni;
				ni = nullptr;
			});
			this->clients.clear();
		}

		void startWithExtraThread() {
#ifdef __THREADS_DEFINED__
			this->runnerThread = std::thread(&NetworkServer::start, this);
#endif //__THREADS_DEFINED__
		}

		void start() {
			fd_set fds;
			int activity;
			sockaddr_in clientInfo;
			SOCKET newClientSocket;

			timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 1000;
			this->maxFileDescriptors = this->getSocket();
			this->setActiveFlag(true);
			this->logger.info("Starting server...");
			this->onStartup();
			do {
				newClientSocket = FAULTY_SOCKET();
				FD_ZERO(&fds);
				this->fillFDS(&fds);
				FD_SET(this->getSocket(), &fds);
				activity = ::select(static_cast<int>(this->maxFileDescriptors+1), &fds, nullptr, nullptr, &timeout);
				if(activity == 0 && clients.empty()) {
					continue;
				}
				if(FD_ISSET(this->getSocket(), &fds)) {
					int clientinfolen = sizeof(sockaddr_in);
					newClientSocket = ::accept(this->getSocket(), (sockaddr*)&clientInfo, (int*)&clientinfolen);
					if(newClientSocket != FAULTY_SOCKET()) {
						this->addNewClient(newClientSocket, &clientInfo);
					}
				}
				this->handleClients(&fds);
				this->onRequestsFinished();
			} while(this->isActive());
		}

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
