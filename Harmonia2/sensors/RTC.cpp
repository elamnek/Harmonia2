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

String init_rtc() {

	setSyncProvider(RTC.get);   // the function to get the time from the RTC
	if (timeStatus() != timeSet)
		return "ERROR: RTC failed to initialise";
	else
		return "";

}

String get_rtc_time() {
	return String(hour()) + ":" + String(minute()) + ":" + String(second()) + " " + String(day()) + "/" + String(month()) + "/" + String(year());
}

/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
	unsigned long pctime = 0L;
	const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 

	if (Serial.find(TIME_HEADER)) {
		pctime = Serial.parseInt();
		return pctime;
		if (pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
			pctime = 0L; // return 0 to indicate that the time is not valid
		}
	}
	return pctime;
}
