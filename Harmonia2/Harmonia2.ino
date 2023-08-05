/*
 Name:		Harmonia2.ino
 Created:	7/1/2023 5:03:31 PM
 Author:	eugene lamnek
*/

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
#include "sensors\depth_sensor.h"
#include "sensors\water_sensors.h"
#include "sensors\temp_sensors.h"
#include "sensors\IMU.h"
#include "sensors\RTC.h"
#include "comms\rf_comms.h"
#include "data\sdcard.h"
#include "control\syringe_ballast.h"

//FSM states
enum { IDLE, MANUAL, STATIC_TRIM, RUN, ALARM } fsm_state;
//function used to return text description of current state
String  get_system_state() {
	switch (fsm_state) {
	case IDLE: return "IDLE";
	case MANUAL: return "MANUAL";
	case STATIC_TRIM: return "STATIC_TRIM";	
	case RUN: return "RUN";
	case ALARM: return "ALARM";}
	return "";
}

int intStateTimerStart;

void setup() {
	Serial.begin(9600);
	
	init_rf_comms();
	init_rtc();

	String strMsg = init_sdcard();
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
		send_rf_comm("depth sensor OK!!");
	}

	send_rf_comm("Harmonia II is awake - stored time is: " + get_rtc_time());

	intStateTimerStart = millis();


	fsm_state = IDLE;
}

void loop() {
  
	
	//do state data write/send here...
	int intStateTimeElapsed = millis() - intStateTimerStart;
	if (intStateTimeElapsed > 1000) {

		//refresh IMU data
		read_imu();

		//create the state object
		subSystemState_t systemState;
		systemState.logTime = get_rtc_time();
		systemState.FSMState = get_system_state();
		systemState.balPos[0] = 50;
		systemState.balPos[1] = 50;
		systemState.balMotorTemp[0] = read_fwd_temp();
		systemState.balMotorTemp[1] = read_aft_temp();
		systemState.att[0] = get_imuorientation_x();
		systemState.att[1] = get_imuorientation_y();
		systemState.att[2] = get_imuorientation_z();
		systemState.acc[0] = get_imuacceleration_x();
		systemState.acc[1] = get_imuacceleration_y();
		systemState.acc[2] = get_imuacceleration_z();
		systemState.internalTemp = read_imu_temp();
		
		//log to sdcard and retrieve the logged string
		String strLogLine = sdcard_logState(&systemState, LOG_ATT + LOG_ACC + LOG_BALLAST);
		send_rf_comm(strLogLine);

		//reset timer
		intStateTimerStart = millis();
	}

	
	//check leak sensors and override any state that has been set
	/*if (fwd_leak_detected() == 1 || aft_leak_detected() == 1) {
		fsm_state = ALARM;
	}*/
	//Serial.println("tester");
	//send_rf_comm("hello from teensy!!");
	//delay(2000);
	//send_rf_comm(get_rtc_time() + " temperatures fwd: " + String(read_fwd_temp()) + " aft: " + String(read_aft_temp()) + " IMU: " + String(read_imu_temp())); //

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
		if (strRemoteCommand == "MANUAL") { fsm_state = MANUAL; }
		if (strRemoteCommand == "STATIC_TRIM") {
			fsm_state = STATIC_TRIM;
			//init_static_trim_2(get_remote_param().toFloat(), 0);
			clear_rf_command();
		}
		if (strRemoteCommand == "RUN") {
			fsm_state = RUN;
			//blnReadyToRun = false;
			//init_run_2(get_remote_param());
			clear_rf_command();

		}	
		if (strRemoteCommand == "ALARM") { fsm_state = ALARM; }

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
	case MANUAL:

		//this checks for a manual command from RF remote and applies it
		//apply_manual_command();
		//check_pushrod(); //adjusts position of pushrod based on latest setpoint command
		clear_rf_command();

		break;
	case STATIC_TRIM:

		//adjust_depth_2();
		//adjust_pitch_2(get_imuorientation_y());

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
	}


}
