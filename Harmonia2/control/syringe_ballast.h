// syringe_ballast.h

#ifndef _SYRINGE_BALLAST_h
#define _SYRINGE_BALLAST_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void ballast_init();

int32_t read_fwd_ballast_pos();
int32_t read_aft_ballast_pos();

void resetBallast(void);

int32_t readBallastPos(int BalAddr);

#endif

