// temp_sensors.h

#ifndef _TEMP_SENSORS_h
#define _TEMP_SENSORS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


void init_tempsensors();

double read_fwd_temp();

double read_aft_temp();

double Thermister(int RawADC);

#endif

