// 
// 
// 

#include "state_static_trim.h"
#include "..\control\syringe_ballast.h"
#include "..\sensors\IMU.h"
#include "..\sensors\depth_sensor.h"

#include <PID_v1.h>
//https://playground.arduino.cc/Code/PIDLibaryBasicExample/


double dveSetpoint, dveInput, dveOutput;
double trmSetpoint, trmInput, trmOutput;

double KpDve = 50, KiDve = 0, KdDve = 0;
double KpTrm = 2, KiTrm = 0, KdTrm = 0; //initially 2

PID dvePID(&dveInput, &dveOutput, &dveSetpoint, KpDve, KiDve, KdDve, DIRECT);
PID trmPID(&trmInput, &trmOutput, &trmSetpoint, KpTrm, KiTrm, KdTrm, DIRECT);

double dblPrevFwdSetpoint;
double dblPrevAftSetpoint;

void init_static_trim(double dblDepthSetpoint,double dblTrimSetpoint) {
	
	dveSetpoint = dblDepthSetpoint;
	trmSetpoint = dblTrimSetpoint;
	
	dvePID.SetOutputLimits(-500, 500);
	trmPID.SetOutputLimits(-200, 200);

	//turn the PID on
	dvePID.SetMode(AUTOMATIC);
	trmPID.SetMode(AUTOMATIC);

	dblPrevFwdSetpoint = 700;
	dblPrevAftSetpoint = 700;

}

void adjust_trim() {
	
	//compute PIDs with latest inputs
	dveInput = get_depth();
	dvePID.Compute();
	
	trmInput = -read_imu_pitch();
	trmPID.Compute();

	//calc new ballast setpoints

	Serial.println(String(trmInput) + "," + String(trmOutput) + "," + String(dveOutput));

	double fwdSetpoint = max(min(dblPrevFwdSetpoint + (dveOutput + trmOutput) / 2, BALLAST_MAX), BALLAST_MIN);
	double aftSetpoint = max(min(dblPrevAftSetpoint + (dveOutput - trmOutput) / 2, BALLAST_MAX), BALLAST_MIN);

	/*double fwdSetpoint = dblPrevFwdSetpoint + ((dveOutput + trmOutput)/2);
	double aftSetpoint = dblPrevAftSetpoint + ((dveOutput - trmOutput)/2);*/

	ballast_setpoints(fwdSetpoint, aftSetpoint);
	
	dblPrevFwdSetpoint = fwdSetpoint;
	dblPrevAftSetpoint = aftSetpoint;

}