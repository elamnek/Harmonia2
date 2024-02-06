// syringe_ballast.h

#ifndef _SYRINGE_BALLAST_h
#define _SYRINGE_BALLAST_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define BALLAST_MAX 1600
#define BALLAST_MIN 0

#define BALLAST_MAX_SAFETY 1550
#define BALLAST_MIN_SAFETY 50

void ballast_init();

void ballast_adjust();

void set_ballast_speeds(int intFwdSpeed, int intAftSpeed);

boolean ballast_setpoints(double fwdSetpoint, double aftSetpoint);

void command_ballast(String strCommand, int intValue);

int get_fwd_ballast_motor_speed();
int get_aft_ballast_motor_speed();

int32_t read_fwd_ballast_pos();
int32_t read_aft_ballast_pos();

void resetBallast(void);

int32_t readBallastPos(int BalAddr);

#endif

