// 
// 
// 

#include "dive_plane.h"
#include <Servo.h>
#include <PID_v1.h>
//https://playground.arduino.cc/Code/PIDLibaryBasicExample/

int m_intDivePlaneInputPin = A9;
int m_intDivePlaneOutputPin = 5;

Servo m_dpServo;

#define CENTRE_RANGE 0

double KpDP = 25, KiDP = 6, KdDP = 0.1;
double dpSetpoint, dpInput, dpOutput;
int dpcommand = 1500;
PID dpPID(&dpInput, &dpOutput, &dpSetpoint, KpDP, KiDP, KdDP, DIRECT);

void dive_plane_init() {

	dpSetpoint = 90;	
	
	dpPID.SetOutputLimits(-1000, 1000);
	dpPID.SetControllerDirection(REVERSE);
	dpPID.SetMode(AUTOMATIC);

	m_dpServo.attach(m_intDivePlaneOutputPin);

}

void diveplane_setpoint(double fwdDPSetpoint) {
	dpSetpoint = fwdDPSetpoint;
}
float get_diveplane_pot() {
	return dpInput;
}

void dive_plane_adjust() {

	dpInput = analogRead(m_intDivePlaneInputPin) * 270.0 / 1023.0; //converts pot output to be 0-180

	dpPID.Compute();

	if (dpOutput < 0) {
		dpOutput = min(-CENTRE_RANGE, dpOutput);
	}
	else {
		dpOutput = max(dpOutput, CENTRE_RANGE);
	}

	if (abs(dpSetpoint - dpInput) < 5) {
		m_dpServo.writeMicroseconds(1500);
	}
	else {
		m_dpServo.writeMicroseconds(-dpOutput + 1500);
	}

}

