#include "LoginServer.h"
#include "LoginClient.h"


LoginServer::~LoginServer() {
}

NetworkClient* LoginServer::onClientConnected(NetworkInterface *iFace) {
	LoginClient *lc = new LoginClient(iFace, this->getCryptInfo());
	this->logger.info(String("Client connected from: ") + iFace->getIp());
	return lc;
}