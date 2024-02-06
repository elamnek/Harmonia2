// 
// 
// 

#include "state_static_trim.h"
#include "..\control\syringe_ballast.h"
#include "..\sensors\IMU.h"
#include "..\sensors\depth_sensor.h"
#include "..\helpers.h"

#include <PID_v1.h>
//https://playground.arduino.cc/Code/PIDLibaryBasicExample/


double dveSetpoint, dveInput, dveOutput;
double trmSetpoint, trmInput, trmOutput;

double KpDve = 50, KiDve = 1, KdDve = 1;
double KpTrm = 2, KiTrm = 1, KdTrm = 1; //initially 2

PID dvePID(&dveInput, &dveOutput, &dveSetpoint, KpDve, KiDve, KdDve, DIRECT);
PID trmPID(&trmInput, &trmOutput, &trmSetpoint, KpTrm, KiTrm, KdTrm, DIRECT);

double dblPrevFwdSetpoint;
double dblPrevAftSetpoint;

void init_static_trim(String strRemoteParam) {

	dveSetpoint = get_sep_part_dbl(strRemoteParam, '|', 0);
	trmSetpoint = get_sep_part_dbl(strRemoteParam, '|', 1);
	KpDve = get_sep_part_dbl(strRemoteParam, '|', 2);
	KiDve = get_sep_part_dbl(strRemoteParam, '|', 3);
	KdDve = get_sep_part_dbl(strRemoteParam, '|', 4);
	KpTrm = get_sep_part_dbl(strRemoteParam, '|', 5);
	KiTrm = get_sep_part_dbl(strRemoteParam, '|', 6);
	KdTrm = get_sep_part_dbl(strRemoteParam, '|', 7);
	double limitmin = get_sep_part_dbl(strRemoteParam, '|', 8);
	double limitmax = get_sep_part_dbl(strRemoteParam, '|', 9);

	
	/*dveSetpoint = dblDepthSetpoint;
	trmSetpoint = dblTrimSetpoint;*/
	
	dvePID.SetOutputLimits(limitmin, limitmax);
	trmPID.SetOutputLimits(limitmin, limitmax);

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
	
	trmInput = 0; //-read_imu_pitch();
	trmPID.Compute();

	//use output to control ballast motors directly
	set_ballast_speeds(dveOutput * 800, -dveOutput * 800);


	/*Serial.println(String(trmSetpoint) + "," + String(dveSetpoint));
	Serial.println(String(trmInput) + "," + String(trmOutput) + "," + String(dveOutput));
	Serial.println(String(KpDve) + "," + String(KpTrm));*/

	//calc new ballast setpoints
	/*double fwdSetpoint = max(min(dblPrevFwdSetpoint + (dveOutput + trmOutput) / 2, BALLAST_MAX), BALLAST_MIN);
	double aftSetpoint = max(min(dblPrevAftSetpoint + (dveOutput - trmOutput) / 2, BALLAST_MAX), BALLAST_MIN);
	ballast_setpoints(fwdSetpoint, aftSetpoint);
	dblPrevFwdSetpoint = fwdSetpoint;
	dblPrevAftSetpoint = aftSetpoint;
	*/

	

}