// controls dual syringe ballast system
// uses PID controller for each syringe
// 2 leonardo uCs are accessed via i2C - these supply the position of the plungers unsing the encoders 

#include "syringe_ballast.h"
#include <Motoron.h>
#include <motoron_protocol.h>
#include <PID_v1.h>
//https://playground.arduino.cc/Code/PIDLibaryBasicExample/
#include "..\comms\rf_comms.h"
#include "..\sensors\temp_sensors.h"


//motor controller channels
#define fwdBalMC 1
#define aftBalMC 2

//i2C addresses of each leanardo
#define fwdBalAddr 10
#define aftBalAddr 9

//create the motor controller object (controls both motors)
MotoronI2C m_ballastMC;

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

typedef enum {
	INIT,
	BACKOFF,
	OPERATE
} balState_t;

balState_t fwdBalState = INIT;
balState_t aftBalState = INIT;

void ballast_init() {

	//Wire2.begin();

	m_ballastMC.setAddress(12);
	m_ballastMC.reinitialize();
	m_ballastMC.clearResetFlag();
	m_ballastMC.disableCrc();
	// Configure the Motoron to coast the motors while obeying
	// decleration limits when there is an error (the default).
	// m_ballastMC.setErrorResponse(MOTORON_ERROR_RESPONSE_COAST);
	// m_ballastMC.setErrorMask(errorMask);

	// Use a short command timeout of 100 ms: the Motoron will
	// stop the motors if it does not get a command for 100 ms.
	m_ballastMC.setCommandTimeoutMilliseconds(2000);

	// Configure motor - Forward
	m_ballastMC.setMaxAcceleration(fwdBalMC, 140);
	m_ballastMC.setMaxDeceleration(fwdBalMC, 300);

	// Configure motor - Aft
	m_ballastMC.setMaxAcceleration(aftBalMC, 140);
	m_ballastMC.setMaxDeceleration(aftBalMC, 300);
	m_ballastMC.clearMotorFault();
	resetBallast();

	fwdBalInput = -10;
	aftBalInput = -10;

	fwdBalPID.SetOutputLimits(-1, 1);
	aftBalPID.SetOutputLimits(-1, 1);

	//turn the PIDs on
	fwdBalPID.SetMode(AUTOMATIC);
	aftBalPID.SetMode(AUTOMATIC);
	
}

void ballast_adjust() {

	//check temps
	double dblFwdTemp = read_fwd_temp();
	double dblAftTemp = read_aft_temp();

	int32_t fwdBallastPos = readBallastPos(fwdBalAddr);
	int32_t aftBallastPos = readBallastPos(aftBalAddr);
	fwdBalInput = fwdBallastPos;
	aftBalInput = aftBallastPos;

	m_intFwdMotorSpeed = 0;
	m_intAftMotorSpeed = 0;

	//check for fault state
	//needs to stay in fault state if any of the conditions are true
	if (dblFwdTemp > 75 || dblAftTemp > 75 || fwdBallastPos == -10000 || aftBallastPos == -10000) {
		m_intFwdMotorSpeed = 0;
		m_intAftMotorSpeed = 0;
	}
	else {

		switch (fwdBalState) {
		case INIT:
			if (fwdBallastPos < 0) {
				m_intFwdMotorSpeed = -800;
			}
			else {
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
			if (abs(fwdBalSetpoint - fwdBalInput) > 3) {
				m_intFwdMotorSpeed = fwdBalOutput * 800;
			}
			break;
		}

		switch (aftBalState) {
		case INIT:
			if (aftBallastPos < 0) {
				m_intAftMotorSpeed = -800;
			}
			else {
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
			if (abs(aftBalSetpoint - aftBalInput) > 3) {
				m_intAftMotorSpeed = aftBalOutput * 800;
			}
			break;
		}
	}
	
	m_ballastMC.setSpeed(fwdBalMC, m_intFwdMotorSpeed);
	m_ballastMC.setSpeed(aftBalMC, m_intAftMotorSpeed);
	
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
		m_ballastMC.setSpeed(fwdBalMC, intValue);
	}
	else if (strCommand == "AFT_BALLAST") {
		m_intAftMotorSpeed = intValue;
		m_ballastMC.setSpeed(aftBalMC, intValue);
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
//reads position of plunger from encoder (via leonardo)
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
