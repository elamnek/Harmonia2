/*
 Name:		Harmonia2.ino
 Created:	7/1/2023 5:03:31 PM
 Author:	eugene lamnek
*/



#include <Servo.h>
#include <PID_v1.h>
#include <Motoron.h>
#include <motoron_protocol.h>
#include <MS5837.h>
#include <SD.h>
#include <_Teensy.h>
#include <TimeLib.h>
#include <WireKinetis.h>
#include <WireIMXRT.h>
#include <Wire.h>
#include <DS1307RTC.h>

//harmonia libraries
#include "sensors\RPM.h"
#include "sensors\depth_sensor.h"
#include "sensors\water_sensors.h"
#include "sensors\temp_sensors.h"
#include "sensors\IMU.h"
#include "sensors\RTC.h"
#include "sensors\power_sensor.h"
#include "comms\rf_comms.h"
#include "data\sdcard.h"
#include "control\syringe_ballast.h"
#include "control\dive_plane.h"
#include "control\aft_section.h"
#include "control\main_motor.h"
#include "states\state_manual.h"
#include "states\state_static_trim.h"
#include "helpers.h"

//FSM states
enum FSMState { IDLE, REMOTE, STATIC_TRIM, RUN, ALARM, TEST } fsm_state;

//function used to return text description of current state
String  get_system_state() {
	switch (fsm_state) {
	case IDLE: return "IDLE";
	case REMOTE: return "REMOTE";
	case STATIC_TRIM: return "STATIC_TRIM";
	case RUN: return "RUN";
	case ALARM: return "ALARM";
	case TEST: return "TEST";}
	return "";
}

//polling timers
int intStateTimerStart;
int intBallastMotorTestTimerStart;

int intBallastTestInc = 150;

void setup() {
	Serial.begin(9600);
	
	//all three i2C buses used
	Wire.begin();  //ballast motor control
	Wire1.begin(); //aft section
	Wire2.begin(); //ballast encoders

	init_rf_comms();
	init_watersensors();
	init_rtc();
	ballast_init();
	aft_section_init();
	dive_plane_init();
	init_main_motor();

	String strMsg = init_powersensor();
	if (strMsg.length() > 0) {
		send_rf_comm(strMsg);
	}
	else {
		send_rf_comm("Power sensor OK!!");
	}
	
	strMsg = init_sdcard();
	if (strMsg.length() > 0) {
		send_rf_comm(strMsg);
	}
	else {
		send_rf_comm("SDCard OK!!");
	}
	
	strMsg = init_imu();
	if (strMsg.length() > 0) {
		send_rf_comm(strMsg);
	}
	else {
		send_rf_comm("IMU sensor OK!!");
	}

	strMsg = init_depth_sensor(997);
	if (strMsg.length() > 0) {
		send_rf_comm(strMsg);
	}
	else {
		send_rf_comm("Depth sensor OK!!");
	}

	send_rf_comm("Harmonia II is awake - stored time is: " + get_rtc_time());

	scan_i2c_wire(0);
	scan_i2c_wire(1);
	scan_i2c_wire(2);

	intStateTimerStart = millis();
	
	fsm_state = IDLE;

}
	
void loop() {

	ballast_adjust();
	dive_plane_adjust(); 
	int intStateTimeElapsed = millis() - intStateTimerStart;
	if (intStateTimeElapsed > 1000) {

		//refresh IMU data
		read_imu();

		//create the state object
		subSystemState_t systemState;
		systemState.logTime = get_rtc_time();
		systemState.FSMState = get_system_state();
		systemState.leak[0] = fwd_leak_check();
		systemState.leak[1] = aft_leak_check();
		systemState.depth = get_depth();
		systemState.balPos[0] = read_fwd_ballast_pos();
		systemState.balPos[1] = read_aft_ballast_pos();
		systemState.balMotorTemp[0] = read_fwd_temp();
		systemState.balMotorTemp[1] = read_aft_temp();
		systemState.balMotorSpeed[0] = get_fwd_ballast_motor_speed();
		systemState.balMotorSpeed[1] = get_aft_ballast_motor_speed();
		systemState.attitude[0] = get_cached_heading();
		systemState.attitude[1] = get_cached_pitch();
		systemState.attitude[2] = get_cached_roll();
		systemState.acceleration[0] = get_imuacceleration_x();
		systemState.acceleration[1] = get_imuacceleration_y();
		systemState.acceleration[2] = get_imuacceleration_z();
		systemState.internalTemp = read_imu_temp();
		systemState.dpPos = get_diveplane_pot();
		systemState.motorTh = get_main_motor_throttle();
		systemState.mototRPM = read_rpm();
		systemState.motorVolt = get_bus_voltage();
		systemState.motorCurr = get_current_mA();

		//log to sdcard and retrieve the logged string
		String strLogLine = sdcard_logState(&systemState, LOG_ATTITUDE + LOG_ACCELERATION + LOG_BALLAST + LOG_DEPTH + LOG_SURFACES + LOG_MOTOR);

		send_rf_comm(strLogLine);

		//reset timer
		intStateTimerStart = millis();
	}

	
	//check leak sensors and override any state that has been set
	if (fwd_leak_check() == 1 || aft_leak_check() == 1) {
		fsm_state = ALARM;
	}
	
	//check for new commands coming from desktop remote
	check_rf_comms();

	//set state using commands from remote
	String strRemoteCommand = get_remote_command();
	//send_rf_comm("remote command received: " + strRemoteCommand);

	if (fsm_state == ALARM) {
		//system in alarm state - can only be changed by command to go to idle state
		if (strRemoteCommand == "IDLE") { fsm_state = IDLE; }
	}
	else {
		//system NOT in alarm state - normal state changes allowed
		if (strRemoteCommand == "IDLE") { fsm_state = IDLE; }
		if (strRemoteCommand == "REMOTE") { fsm_state = REMOTE; }
		if (strRemoteCommand == "STATIC_TRIM") {
			fsm_state = STATIC_TRIM;
			init_static_trim(get_remote_param().toFloat(), 0);
			clear_rf_command();
		}
		if (strRemoteCommand == "RUN") {
			fsm_state = RUN;
			//blnReadyToRun = false;
			//init_run_2(get_remote_param());
			clear_rf_command();

		}	
		if (strRemoteCommand == "ALARM") { fsm_state = ALARM; }
		if (strRemoteCommand == "TEST") {
			fsm_state = TEST;
			intBallastMotorTestTimerStart = millis();
			clear_rf_command();
		}

	}

	//check for command to set time
	if (strRemoteCommand == "TIMESET") {
		set_rtc_time(get_remote_param());
		clear_rf_command();
	}

	//state control	
	switch (fsm_state) {
	case IDLE:

		//in idle state need to stop any active operation
		//command_pump("INFLATE", 0);
		//commmand_main_motor(0);

		break;
	case REMOTE:

		//this checks for a manual command from RF remote and applies it
		apply_manual_command();
		
		clear_rf_command();

		break;
	case STATIC_TRIM:

		adjust_trim();

		break;
	case RUN:

		//allow for manual adjustments while running
		//apply_manual_command();

		//if (!blnReadyToRun) {
		//	//adjust until trim achieved 
		//	boolean blnDepthTrim = adjust_depth_2();
		//	//boolean blnPitchTrim = adjust_pitch_2(get_imuorientation_y()); 
		//	// && blnPitchTrim
		//	if (blnDepthTrim) {
		//		blnReadyToRun = true;
		//		command_pushrod("REVERSE", 0);
		//		delay(200);
		//		command_pump("DEFLATE", 0);

		//		run_start_2(get_imuorientation_x());
		//	}
		//}
		//else {
		//	//boolean blnRunDone = adjust_run_2(get_imuorientation_x(), get_imuorientation_y());
		//	//if (blnRunDone) {

		//		//just inflate and go into manual state
		//		//command_pump("INFLATE", 255);
		//		//state = IDLE;
		//		//blnReadyToRun = false;

		//		//when run is complete - maintain depth and pitch until state is changed
		//		//static_trim_reset();
		//		//adjust_depth_2();
		//		//adjust_pitch_2(get_imuorientation_y());

		//	//}
		//}

		//clear_rf_command();

		break;

	case ALARM:
		//once in alarm state can only exit by user changing to another state via remote software

		//inflating will help sub return to surface and also ensure it stops pumping in water if the air bag is ruptured
		//command_pump("INFLATE", 255);

		//could also initiate power to surface using motor at high thrust and rudders but this would be a problem in confined space such as tank
		//collision with walls could occur

		//also clear the comms buffer so that any user command to idle state can be read
		clear_rf_command();

		break;

	case TEST:

	
		int intTimeElapsed = millis() - intBallastMotorTestTimerStart;
		if (intTimeElapsed > 5000) {

			intBallastTestInc = -intBallastTestInc;
			
			ballast_setpoints(500 + intBallastTestInc, 500 + intBallastTestInc);


			//reset timer
			intBallastMotorTestTimerStart = millis();
		}
		break;
	}


}

void scan_i2c_wire(int intWireNum) {

	byte error, address; //variable for error and I2C address
	int nDevices;

	send_rf_comm("Scanning I2C on wire " + String(intWireNum));

	nDevices = 0;
	for (address = 1; address < 127; address++)
	{
		// The i2c_scanner uses the return value of
		// the Write.endTransmisstion to see if
		// a device did acknowledge to the address.

		if (intWireNum == 0) {
			Wire.beginTransmission(address);
			error = Wire.endTransmission();
		}
		else if (intWireNum == 1) {
			Wire1.beginTransmission(address);
			error = Wire1.endTransmission();
		}
		else if (intWireNum == 2) {
			Wire2.beginTransmission(address);
			error = Wire2.endTransmission();
		}

		if (error == 0)
		{
			nDevices++;			
			send_rf_comm(FormatMsg(address, "I2C device found"));
		}
		else if (error == 4)
		{		
			send_rf_comm(FormatMsg(address, "Unknown error"));
		}
	}
	if (nDevices == 0) {
		send_rf_comm("No I2C devices found");
	}
	else 
	{
		send_rf_comm("done");
	}

}
String FormatMsg(byte address,String strPrefix) {
	String strMsg = strPrefix + " at address 0x";
	if (address < 16) { strMsg = strMsg + "0"; }
	strMsg = strMsg + String(address, HEX);
	return strMsg;
}

