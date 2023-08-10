// 
// 
// 

#include "syringe_ballast.h"
#include <Motoron.h>
#include <motoron_protocol.h>
#include <PID_v1.h>
//https://playground.arduino.cc/Code/PIDLibaryBasicExample/


//motor controller bays
#define forBalMC 1
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
	BallastMC.setMaxAcceleration(forBalMC, 140);
	BallastMC.setMaxDeceleration(forBalMC, 300);

	// Configure motor - Aft
	BallastMC.setMaxAcceleration(aftBalMC, 140);
	BallastMC.setMaxDeceleration(aftBalMC, 300);
	BallastMC.clearMotorFault();
	resetBallast();

	m_blnFwdInitialising = true;
	m_blnAftInitialising = true;

}

void setpoint_follow(double fwdPosSetpoint,double aftPosSetpoint) {
	if (m_blnFwdInitialising) {
		//syringe is still initialising

	}
}
void command_ballast(String strCommand, int intValue) {

	if (strCommand == "FWD_BALLAST") {
		m_intFwdMotorSpeed = intValue;
		BallastMC.setSpeed(forBalMC, intValue);
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
