// RPM.h

#ifndef _RPM_h
#define _RPM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void rpm_init();

float read_rpm();

int32_t readRPM();

#endif

