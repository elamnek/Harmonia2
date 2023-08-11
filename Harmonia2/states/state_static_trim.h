// state_static_trim.h

#ifndef _STATE_STATIC_TRIM_h
#define _STATE_STATIC_TRIM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void init_static_trim(double dblDepthSetpoint, double dblTrimSetpoint);

void adjust_trim();

#endif

