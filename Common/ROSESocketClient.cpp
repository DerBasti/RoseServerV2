#include "ROSESocketClient.h"

bool ROSESocketClient::sendPacket(const Packet& p) {
	SharedArrayPtr<char> buf = SharedArrayPtr<char>(new char[p.getLength()], p.getLength());
	memcpy(buf.get(), &p, p.getLength());
	EncryptBuffer(this->crypt.table, reinterpret_cast<unsigned char*>(buf.get()));

	return this->sendData(buf.get(), p.getLength());
}

void ROSESocketClient::onDataReceived(int len) {
	memcpy(&(&this->packet)[this->getForcedOffset()], &this->getData()[this->getForcedOffset()], len - this->getForcedOffset());
	if (this->getForcedOffset() != 0 && len < this->getPacket().getLength()) {
		this->setForcedOffset(len);
		return;
	}
	if (len == Packet::DEFAULT_HEADER_OFFSET) {
		word_t newLen = DecryptBufferHeader(&this->crypt.status, this->crypt.table, this->getPacket().toUnsignedChar());
		if (newLen > Packet::DEFAULT_HEADER_OFFSET) {
			this->setForcedOffset(len);
			return;
		}
	}
	DecryptBufferHeader(&this->crypt.status, this->crypt.table, this->getPacket().toUnsignedChar());
	
	if (!::DecryptBufferData(this->crypt.table, this->getPacket().toUnsignedChar()) || !this->handlePacket()) {
		this->setActiveFlag(false);
	}
	this->getPacket().newPacket(0x00);
}