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

float get_imuorientation_x();
float get_imuorientation_y();
float get_imuorientation_z();

float get_imuacceleration_x();
float get_imuacceleration_y();
float get_imuacceleration_z();


#endif

