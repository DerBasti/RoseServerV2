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
	static void GetDateAsArray(String* array);
public:
	static String GetDate();
	static String GetSortableDate();
	static String GetCurrentDayTime();
	static String GetCurrentDayTimeWithMillis();
};

#endif