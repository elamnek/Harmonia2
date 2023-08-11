// IMU.h

#ifndef _IMU_h
#define _IMU_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

String init_imu();

void read_imu();
int8_t read_imu_temp();

float read_imu_pitch();

float get_cached_heading();
float get_cached_roll();
float get_cached_pitch();

float get_imuacceleration_x();
float get_imuacceleration_y();
float get_imuacceleration_z();


#endif

