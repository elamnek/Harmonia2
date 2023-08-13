// 
// 
// 

#include "dive_plane.h"
#include <Servo.h>
#include <PID_v1.h>
//https://playground.arduino.cc/Code/PIDLibaryBasicExample/

int m_intDivePlaneInputPin = A9;
int m_intDivePlaneOutputPin = 22;

Servo m_dpServo;

double KpDP = 25, KiDP = 6, KdDP = 0.1;
double dpSetpoint, dpInput, dpOutput;
int dpcommand = 1500;
PID dpPID(&dpInput, &dpOutput, &dpSetpoint, KpDP, KiDP, KdDP, DIRECT);

void dive_plane_init() {

	dpInput = analogRead(m_intDivePlaneInputPin) * 270.0 / 1023.0;

	dpSetpoint = 135;
	// setpointDiff = 1;
	
	dpPID.SetOutputLimits(-1000, 1000);
	
	//turn the PID on
	dpPID.SetMode(AUTOMATIC);

	m_dpServo.attach(m_intDivePlaneOutputPin);

}


