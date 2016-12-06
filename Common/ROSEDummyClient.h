#ifndef __ROSE_DUMMY_CLIENT__
#define __ROSE_DUMMY_CLIENT__

#ifdef _MSC_VER
#pragma once
#endif

#include "ROSESocketClient.h"

//This "client" implementation only exists for multi threading safety. 
//It is nothing but an ensurance that no nullpointer exceptions arise when packets are sent 
//in an untimely manner (e.g. whisper).
class ROSEDummyClient : public ROSESocketClient {
public:
	ROSEDummyClient() : ROSESocketClient() {}
	virtual ~ROSEDummyClient() {}

	virtual bool handlePacket(const Packet& pak) {
		return true;
	}
	virtual bool sendPacket(const Packet& p) {
		return true; 
	}
};

#endif