// 
// 
// 

#include "syringe_ballast.h"
#include <Motoron.h>
#include <motoron_protocol.h>

//motor controller bays
#define forBalMC 1
#define aftBalMC 2

//i2C address
#define forBalAddr 10
#define aftBalAddr 9

MotoronI2C BallastMC;

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
	// BallastMC.setCommandTimeoutMilliseconds(100);

	// Configure motor - Forward
	BallastMC.setMaxAcceleration(forBalMC, 140);
	BallastMC.setMaxDeceleration(forBalMC, 300);

	// Configure motor - Aft
	BallastMC.setMaxAcceleration(aftBalMC, 140);
	BallastMC.setMaxDeceleration(aftBalMC, 300);
	BallastMC.clearMotorFault();
	resetBallast();


}

int32_t read_fwd_ballast_pos() {
	int32_t balPos = readBallastPos(forBalAddr);
	if (balPos != -10000) {return balPos;}
}
int32_t read_aft_ballast_pos() {
	int32_t balPos = readBallastPos(aftBalAddr);
	if (balPos != -10000) {return balPos;}	
}


void resetBallast(void) {
	Wire2.beginTransmission(forBalAddr);
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
