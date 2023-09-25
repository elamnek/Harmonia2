// dive_plane.h

#ifndef _DIVE_PLANE_h
#define _DIVE_PLANE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void dive_plane_init();

float get_diveplane_pot();

void diveplane_setpoint(double fwdDPSetpoint);

void dive_plane_adjust();


#endif

