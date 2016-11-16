#include "StoppableClock.h"
#include <Windows.h>

long long StoppableClock::GetCurrentTimeStamp() {
	long long result = 0x00;
#ifdef _MSC_VER
	SYSTEMTIME st;
	GetSystemTime(&st);

	std::tm tm;
	tm.tm_sec = st.wSecond;
	tm.tm_min = st.wMinute;
	tm.tm_hour = st.wHour;
	tm.tm_mday = st.wDay;
	tm.tm_mon = st.wMonth - 1;
	tm.tm_year = st.wYear - 1900;

	time_t timeStamp = std::mktime(&tm);
	result = (timeStamp * 1000) + st.wMilliseconds;
#else
	struct timeval tp;
	gettimeofday(&tp, NULL);
	unsigned long long current = (static_cast<unsigned long long>(tp.tv_sec) * 1000);
	result = (current + (tp.tv_usec / 1000));
#endif
	return result;
}