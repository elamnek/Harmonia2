// sdcard.h

#ifndef _SDCARD_h
#define _SDCARD_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define LOG_MOTOR 0x40 // (1<<6)
#define LOG_SURFACES 0x20 // (1<<5)
#define LOG_BALLAST 0x10 // (1<<4)
#define LOG_POSITION 0x8 // (1<<3)
#define LOG_DEPTH  0x04 // (1<<2)
#define LOG_ATTITUDE 0x02 // (1<<1)
#define LOG_ACCELERATION  0x01 // (1<<0)

typedef struct {
	String logTime;
	String FSMState;
	int leak[2];

	float acceleration[3];
	float attitude[3];
	
	float depth;
	float xPos;

	int balPos[2];
	float balMotorTemp[2];
	int balMotorSpeed[2];
	float internalTemp;

	float dpPos;
	float tailPos[4];

	float motorTh;
	float mototRPM;
	float motorVolt;
	float motorCurr;
} subSystemState_t;


String init_sdcard();

String sdcard_logState(subSystemState_t* state, int8_t logFlags);


#endif

