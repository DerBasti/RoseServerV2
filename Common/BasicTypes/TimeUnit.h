#ifndef __TIME_UNIT__
#define __TIME_UNIT__

#ifdef _MSC_VER
#pragma once
#endif

#include "BasicObject.h"
#include "StoppableClock.h"
#include <time.h>

class TimeUnits {
private:
	static void GetDateAsArray(String* array) {
#ifdef _MSC_VER
		SYSTEMTIME st;
		GetLocalTime(&st);
		array[0] = String::fromInt(st.wDay);
		array[1] = String::fromInt(st.wMonth);
		array[2] = String::fromInt(st.wYear);

		if (st.wDay < 10) {
			array[0] = String("0") + array[0];
		}
		if (st.wMonth < 10) {
			array[1] = String("0") + array[1];
		}
#else

#endif
	}
public:
	static String GetDate() {
		String result = String();
		String date[3] = { String() };
		TimeUnits::GetDateAsArray(date);
		return date[2] + String(".") + date[1] + String(".") + date[2];
	}

	static String GetSortableDate() {
		String result = String();
		String date[3] = { String() };
		TimeUnits::GetDateAsArray(date);
		return date[2] + String(".") + date[1] + String(".") + date[0];
	}

	static String GetCurrentDayTime() {
		String result = String();
#ifdef _MSC_VER
		SYSTEMTIME st;
		GetLocalTime(&st);
		String hours = String::fromInt(st.wHour);
		String minutes = String::fromInt(st.wMinute);
		String seconds = String::fromInt(st.wSecond);
		if (st.wHour < 10) {
			hours = String("0") + hours;
		}
		if (st.wMinute < 10) {
			minutes = String("0") + minutes;
		}
		if (st.wSecond < 10) {
			seconds = String("0") + seconds;
		}
		result = hours + String(":") + minutes + String(":") + seconds;
#else

#endif
		return result;
	}
	static String GetCurrentDayTimeWithMillis() {
		String dayTime = TimeUnits::GetCurrentDayTime();
		dayTime += ".";
		String milliSeconds = String();
		int ms = StoppableClock::GetCurrentTimeStamp() % 1000;
		for (unsigned int i = 0;i<=2; i++) {
			milliSeconds = String::fromInt(ms % 10) + milliSeconds;
			ms /= 10;
		}

		return dayTime + milliSeconds;
	}
};

#endif