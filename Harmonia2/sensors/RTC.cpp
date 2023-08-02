// 
// 
// 


//NOTE THE ORDER OF INCLUSION OF THE FOLLOWING LIBRARIES IS IMPORTANT
//CHANGING THE ORDER MAY RESULT IN THE BOARD NOT RESPONDING AND
//HOURS OF TEETH GNASHING

#include <_Teensy.h>
#include <TimeLib.h>
#include <WireKinetis.h>
#include <WireIMXRT.h>
#include <Wire.h>
#include <DS1307RTC.h>

const long DEFAULT_TIME = 1357041600; // Jan 1 2013

void init_rtc() {

	setSyncProvider(RTC.get);   // the function to get the time from the RTC
	
}

void set_rtc_time(String strUNIXDateTimeFromPC) {

	long pctime = strUNIXDateTimeFromPC.toInt();
	if (pctime > DEFAULT_TIME) {
		RTC.set(pctime); 
		setTime(pctime);
	}
		
}

String get_rtc_time() {
	return String(hour()) + ":" + String(minute()) + ":" + String(second()) + " " + String(day()) + "/" + String(month()) + "/" + String(year());
}


