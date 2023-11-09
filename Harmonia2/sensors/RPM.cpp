// 
// 
// 

#include "RPM.h"
#include <Wire.h>

//i2C address of RPM leanardo
#define rpmAddr 12



void rpm_init() {
	Wire1.begin();
}

float read_rpm() {
	return readRPM();
}


int32_t readRPM() {
	int i = 0;
	int32_t val = 0;
	bool blnSendStop = true;
	Wire1.requestFrom(rpmAddr, 4, blnSendStop);
	if (Wire1.available() > 0) {
		while (Wire1.available() > 0) {  // slave may send less than requested
			val += Wire1.read() << (8 * i);
			i++;
		}
		return val;
	}
	else {
		return -10000;
	}
}
