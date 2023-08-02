// RTC.h

#ifndef _RTC_h
#define _RTC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

String init_rtc();

void set_rtc_time(String strUNIXDateTimeFromPC);

String get_rtc_time();

#endif

