// 
// 
// 

#include "syringe_ballast.h"
#include <Motoron.h>
#include <motoron_protocol.h>
#include <PID_v1.h>
#include "..\comms\rf_comms.h"
//https://playground.arduino.cc/Code/PIDLibaryBasicExample/


//motor controller bays
#define fwdBalMC 1
#define aftBalMC 2

//i2C address
#define fwdBalAddr 10
#define aftBalAddr 9



MotoronI2C BallastMC;

int m_intFwdMotorSpeed;
int m_intAftMotorSpeed;


//define the PIDs to drecctly control the position of the plungers
//setpoints will be supplied to this module as an input
//PID inputs will be plunger position (from leonardo encoder reading)
//PID outputs will be a speed factor
double fwdBalSetpoint, fwdBalInput, fwdBalOutput;
double aftBalSetpoint, aftBalInput, aftBalOutput;
double KpBal = 8, KiBal = 0, KdBal = 0.1;
PID fwdBalPID(&fwdBalInput, &fwdBalOutput, &fwdBalSetpoint, KpBal, KiBal, KdBal, DIRECT);
PID aftBalPID(&aftBalInput, &aftBalOutput, &aftBalSetpoint, KpBal, KiBal, KdBal, DIRECT);

boolean m_blnFwdInitialising;
boolean m_blnAftInitialising;

//enum fwdBalState { INIT, BACKOFF, OPERATE} fwd_bal_state;
//enum aftBalState { INIT, BACKOFF, OPERATE } aft_bal_state;

typedef enum {
	INIT,
	BACKOFF,
	OPERATE
} balState_t;

balState_t fwdBalState = INIT;
balState_t aftBalState = INIT;

void ballast_init() {

	Wire2.begin();

	BallastMC.setAddress(12);
	BallastMC.reinitialize();
	BallastMC.clearResetFlag();
	BallastMC.disableCrc();
	// Configure the Motoron to coast the motors while obeying
	// decleration limits when there is an error (the default).
	// BallastMC.setErrorResponse(MOTORON_ERROR_RESPONSE_COAST);
	// BallastMC.setErrorMask(errorMask);

	// Use a short command timeout of 100 ms: the Motoron will
	// stop the motors if it does not get a command for 100 ms.
	BallastMC.setCommandTimeoutMilliseconds(2000);

	// Configure motor - Forward
	BallastMC.setMaxAcceleration(fwdBalMC, 140);
	BallastMC.setMaxDeceleration(fwdBalMC, 300);

	// Configure motor - Aft
	BallastMC.setMaxAcceleration(aftBalMC, 140);
	BallastMC.setMaxDeceleration(aftBalMC, 300);
	BallastMC.clearMotorFault();
	resetBallast();

	fwdBalInput = -10;
	aftBalInput = -10;

	fwdBalPID.SetOutputLimits(-800, 800);
	aftBalPID.SetOutputLimits(-800, 800);

	//turn the PIDs on
	fwdBalPID.SetMode(AUTOMATIC);
	aftBalPID.SetMode(AUTOMATIC);
	
}



void ballast_adjust() {

	int32_t fwdBallastPos = readBallastPos(fwdBalAddr);
	int32_t aftBallastPos = readBallastPos(aftBalAddr);
	fwdBalInput = fwdBallastPos;
	aftBalInput = aftBallastPos;

	m_intFwdMotorSpeed = 0;
	m_intAftMotorSpeed = 0;

	switch (fwdBalState) {
	case INIT:
		if (fwdBallastPos < 0) {
			//BallastMC.setSpeed(fwdBalMC, -800);
			m_intFwdMotorSpeed = -800;
		}
		else {
			//BallastMC.setSpeed(fwdBalMC, 0);
			fwdBalState = BACKOFF;
		}
		break;
	case BACKOFF:
		fwdBalSetpoint = 10;
		//if (dveInput > 0.15) {
		fwdBalState = OPERATE;
		//}
		break;
	case OPERATE:
		fwdBalPID.Compute();
		if (abs(fwdBalSetpoint - fwdBalInput) < 3) {
			//BallastMC.setSpeed(fwdBalMC, 0);
		}
		else {
			//BallastMC.setSpeed(fwdBalMC, fwdBalOutput * 800 / 255);
			m_intFwdMotorSpeed = fwdBalOutput * 800 / 255;
		}
		break;
	}
	BallastMC.setSpeed(fwdBalMC, m_intFwdMotorSpeed);


	
	switch (aftBalState) {
	case INIT:
		if (aftBallastPos < 0) {
			//BallastMC.setSpeed(aftBalMC, -800);
			m_intAftMotorSpeed = -800;
		}
		else {
			//BallastMC.setSpeed(aftBalMC, 0);
			aftBalState = BACKOFF;
		}
		break;
	case BACKOFF:
		aftBalSetpoint = 10;
		//if (dveInput > 0.15) {
		aftBalState = OPERATE;
		//}
		break;
	case OPERATE:
		aftBalPID.Compute();
		if (abs(aftBalSetpoint - aftBalInput) < 3) {
			//BallastMC.setSpeed(aftBalMC, 0);
		}
		else {
			//BallastMC.setSpeed(aftBalMC, aftBalOutput * 800 / 255);
			m_intAftMotorSpeed = aftBalOutput * 800 / 255;
		}
		break;
	}
	BallastMC.setSpeed(aftBalMC, m_intAftMotorSpeed);
	
}

boolean ballast_setpoints(double fwdSetpoint, double aftSetpoint) {
	
	//make sure setpoints are within allowable limits
	if (fwdSetpoint > BALLAST_MAX) { fwdSetpoint = BALLAST_MAX; }
	if (aftSetpoint > BALLAST_MAX) { aftSetpoint = BALLAST_MAX; }
	if (fwdSetpoint < BALLAST_MIN) { fwdSetpoint = BALLAST_MIN; }
	if (aftSetpoint < BALLAST_MIN) { aftSetpoint = BALLAST_MIN; }
	
	fwdBalSetpoint = fwdSetpoint;
	aftBalSetpoint = aftSetpoint;

	//send_rf_comm("fwd setpoint: " + String(fwdBalSetpoint));
	//send_rf_comm("aft setpoint: " + String(aftBalSetpoint));

	return true;
}

void command_ballast(String strCommand, int intValue) {

	if (strCommand == "FWD_BALLAST") {
		m_intFwdMotorSpeed = intValue;
		BallastMC.setSpeed(fwdBalMC, intValue);
	}
	else if (strCommand == "AFT_BALLAST") {
		m_intAftMotorSpeed = intValue;
		BallastMC.setSpeed(aftBalMC, intValue);
	}	
}

int get_fwd_ballast_motor_speed() {
	return m_intFwdMotorSpeed;
}
int get_aft_ballast_motor_speed() {
	return m_intAftMotorSpeed;
}


int32_t read_fwd_ballast_pos() {
	return readBallastPos(fwdBalAddr);
}
int32_t read_aft_ballast_pos() {
	return readBallastPos(aftBalAddr);	
}

//sends a reset command (*R) to both leonardos to tell them to set positions to -100
void resetBallast(void) {
	Wire2.beginTransmission(fwdBalAddr);
	Wire2.write("*R");
	Wire2.endTransmission();
	delay(200);
	Wire2.beginTransmission(aftBalAddr);
	Wire2.write("*R");
	Wire2.endTransmission();
	delay(200);
}
int32_t readBallastPos(int BalAddr) {
	int i = 0;
	int32_t val = 0;
	bool blnSendStop = true;
	Wire2.requestFrom(BalAddr, 4, blnSendStop);
	if (Wire2.available() > 0) {
		while (Wire2.available() > 0) {  // slave may send less than requested
			val += Wire2.read() << (8 * i);
			i++;
		}
		return val;
	}
	else {
		return -10000;
	}
}
