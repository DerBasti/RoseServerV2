#include "NetworkServer.h"
#ifdef _WIN32
	#include <WinSock2.h>
	#include <Windows.h>

	#ifdef _MSC_VER
		#pragma comment(lib, "Ws2_32.lib")
	#endif
#endif

NetworkStartup startup = NetworkStartup();

NetworkStartup::NetworkStartup() {
	WSADATA wsa;
	if (::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) == INVALID_SOCKET && WSAGetLastError() == WSANOTINITIALISED) {
		WSAStartup(MAKEWORD(2, 0), &wsa);
	}
}
NetworkStartup::~NetworkStartup() {
	WSACleanup();
}


NetworkInterface::NetworkInterface(__w64 unsigned int sock, sockaddr_in *addr) : NetworkInterface(String(inet_ntoa(addr->sin_addr)), addr->sin_port) {
	this->setSocket(sock);
}

__w64 unsigned int NetworkInterface::FAULTY_SOCKET() const {
	return static_cast<SOCKET>(SOCKET_ERROR);
}

bool NetworkInterface::establishSocket() {
	this->logger.debug("Establishing socket...");
	this->socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->isSocketValid()) {
		this->logger.debug("Status: Successfully established.");
	}
	else {
		this->logger.warn(String("Status: Failed to establish socket. Error-Code: ") + String::fromInt(GetLastError()));
	}
	return this->isSocketValid();
}

void NetworkInterface::moveSocketFrom(NetworkInterface* ni) {
	this->setSocket(ni->getSocket());
	ni->setSocket(FAULTY_SOCKET());
}

void NetworkInterface::closeSocket() {
	if (this->isSocketValid()) {
		this->logger.debug("Closing existing socket.");
		::closesocket(this->getSocket());
		this->socket = SOCKET_ERROR;
	}
}

sockaddr_in NetworkInterface::GetSockaddrFromIpAndPort(String ip, unsigned int port) {
	sockaddr_in addr;
	addr.sin_port = port;
	addr.sin_addr.S_un.S_addr = inet_addr(ip.toConstChar());
	addr.sin_family = IPPROTO_TCP;
	return addr;
}

NetworkClient::NetworkClient(SOCKET_TYPE sock, struct sockaddr_in addr) : NetworkClient(sock, &addr) {}
NetworkClient::NetworkClient(SOCKET_TYPE sock, struct sockaddr_in *addr) : NetworkClient(NetworkInterface(sock, addr)) {}

void NetworkClient::connect() {
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = ::inet_addr(this->getIp().toConstChar());
	addr.sin_port = htons(this->getPort());
	memset(&addr.sin_zero, '\0', 8);
	this->logger.info(String("Connecting towards: ") + this->getIp() + String(":") + String::fromInt(this->getPort()));
	this->setActiveFlag(::connect(this->getSocket(), reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in)) == 0);
	if (!this->isActive()) {
		this->logger.error(String("Couldn't connect to ") + this->getIp() + String(". Error-Code: ") + String::fromInt(GetLastError()));
	}
	else {
		this->logger.info(String("Successfully connected to: ") + IP_AND_PORT_TOSTRING);
	}
}

bool NetworkClient::receiveDataImpl(unsigned int bufferLength, unsigned int offset) {
	this->setForcedOffset(0);
	if (offset >= this->getMaxDataLength()) {
		this->logger.warn(String("The offset of ") + String::fromInt(offset) + String(" exceeds the maximum capacity of ") +
			String::fromLong(this->getMaxDataLength()) + String(" bytes by ") +
			String::fromInt(offset - this->getMaxDataLength()) + String(". Receiving data is impossible."));
		return false;
	}
	int remainingSize = this->getMaxDataLength() - offset;
	if (remainingSize <= 0) {
		return false;
	}
	if (static_cast<unsigned int>(remainingSize) < bufferLength) {
		bufferLength = remainingSize - 1;
	}
	this->lastPacketSize = ::recv(this->getSocket(), &this->packetBuffer.get()[offset], bufferLength, 0x00);
	if (this->lastPacketSize == static_cast<unsigned int>(-1)) {
		this->logger.info(String("Connection from '") + this->getIp() + String("' suddenly aborted."));
		return false;
	}
	this->packetBuffer.get()[offset + this->lastPacketSize] = 0;
	if (this->getLastPacketLength() > 0) {
		this->lastPacketSize += offset;
		this->onDataReceived(this->getLastPacketLength());
	}
	return this->getLastPacketLength() > 0;
}

bool NetworkClient::sendData(const char *data, unsigned long len) {
	bool success = false;
	if (this->isSocketValid()) {
		this->onDataSend(data, len);
		success = (static_cast<unsigned long>(::send(this->getSocket(), data, len, 0x00)) == len);
		this->onDataSendFinished(success, len);
	}
	if (!success) {
		this->logger.warn(String("Failed to transmit data of length ") + String::fromInt(len));
	}
	return success;
}


NetworkServer::NetworkServer(const String& serverIp, unsigned short port) : NetworkInterface(serverIp, port) {
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

NetworkServer::~NetworkServer() {
	std::for_each(clients.begin(), clients.end(), [](NetworkClient* ni) {
		::closesocket(ni->getSocket());
		delete ni;
		ni = nullptr;
	});
	this->clients.clear();
}


bool NetworkServer::setupSocket() {
	unsigned int optVal = 1;
	if (::setsockopt(this->getSocket(), SOL_SOCKET, SO_KEEPALIVE, (const char*)&optVal, sizeof(optVal)) == SOCKET_ERROR) {
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

void NetworkServer::fillFDS(fd_set *set) {
	for (auto it = this->clients.cbegin(); it != this->clients.cend();) {
		NetworkClient *ni = *it;
		if (ni->isActive()) {
			FD_SET(ni->getSocket(), set);
			if (ni->getSocket() > maxFileDescriptors) {
				maxFileDescriptors = ni->getSocket();
			}
			it++;
		}
		else {
			this->disconnectClient(ni, it);
		}
	}
}

void NetworkServer::addNewClient(SOCKET_TYPE sock, struct sockaddr_in* addr) {
	NetworkInterface ni(sock, addr);
	NetworkClient* client = this->onClientConnected(&ni);
	this->logger.info(String("Adding new client with IP: ") + ni.getIp());
	client->setActiveFlag(true);
	this->clients.push_back(client);
}

void NetworkServer::handleClients(FD_SET* fds) {
	std::for_each(clients.begin(), clients.end(), [=](NetworkClient* ni) {
		if (!ni->isActive()) {
			return;
		}
		if (FD_ISSET(ni->getSocket(), fds)) {
			this->logger.debug(String("Client from IP ") + ni->getIp() + String(" sent data to retrieve."));
			this->onIncomingData(ni);
			ni->setActiveFlag(ni->receiveData(ni->getForcedOffset()));
			this->onClientDataReceived(ni);
		}
	});
}

void NetworkServer::start() {
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
		activity = ::select(static_cast<int>(this->maxFileDescriptors + 1), &fds, nullptr, nullptr, &timeout);
		if (activity == 0 && clients.empty()) {
			this->onRequestsFinished();
			continue;
		}
		if (FD_ISSET(this->getSocket(), &fds)) {
			int clientinfolen = sizeof(sockaddr_in);
			newClientSocket = ::accept(this->getSocket(), (sockaddr*)&clientInfo, (int*)&clientinfolen);
			if (newClientSocket != FAULTY_SOCKET()) {
				this->addNewClient(newClientSocket, &clientInfo);
			}
		}
		this->handleClients(&fds);
		this->onRequestsFinished();
	} while (this->isActive());
}