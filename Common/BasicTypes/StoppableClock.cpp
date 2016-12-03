#include "StoppableClock.h"
#include <Windows.h>
#include <iostream>
#include <chrono>

unsigned long long StoppableClock::GetCurrentTimeStamp() {
	unsigned long long result = 0x00;
#ifdef _MSC_VER
	SYSTEMTIME st;
	GetSystemTime(&st);

	result = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
#else
	struct timeval tp;
	gettimeofday(&tp, NULL);
	unsigned long long current = (static_cast<unsigned long long>(tp.tv_sec) * 1000);
	result = (current + (tp.tv_usec / 1000));
#endif
	return result;
}

unsigned long long StoppableClock::getDuration() const {
	if (this->startTimeStamp == 0 || this->pausedWhen != 0) {
		return 0;
	}
	unsigned long long currentTimestamp = StoppableClock::GetCurrentTimeStamp();
	unsigned long long diff = currentTimestamp - this->startTimeStamp;
	return diff;
}


unsigned long long StoppableClock::timeLap() {
	if (this->startTimeStamp == 0 || this->pausedWhen != 0) {
		return 0;
	}
	unsigned long long res = this->getDuration();
	this->startTimeStamp = StoppableClock::GetCurrentTimeStamp();

	//std::cout << "Result: " << res << ", Start: " << this->startTimeStamp << "\n";

	return res;
}

void StoppableClock::start() {
	this->pausedWhen = 0;
	this->startTimeStamp = StoppableClock::GetCurrentTimeStamp();
}