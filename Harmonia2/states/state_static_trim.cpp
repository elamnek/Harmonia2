// 
// 
// 

#include "state_static_trim.h"
#include "..\control\syringe_ballast.h"
#include "..\sensors\IMU.h"

#include <PID_v1.h>
//https://playground.arduino.cc/Code/PIDLibaryBasicExample/


//pitch PID
double m_dbPitchSetpoint, m_dblPitchInput, m_dblPitchOutput;
PID m_PIDpitch(&m_dblPitchInput, &m_dblPitchOutput, &m_dbPitchSetpoint, 2, 5, 1, DIRECT);



void init_static_trim(float flt) {
	
	m_dbPitchSetpoint = flt; //user defined on remote

	m_PIDpitch.SetOutputLimits(-200, 200); 

	//turn the PID on
	m_PIDpitch.SetMode(AUTOMATIC);

}

void adjust_pitch() {
	
	
	m_dblPitchInput = get_imuorientation_y();
	m_PIDpitch.Compute();

	if (m_dblPitchOutput < 0) {
		double dblActual = m_dblPitchOutput - 600;
		command_ballast("AFT_BALLAST", dblActual);
		command_ballast("FWD_BALLAST", -dblActual);
	}
	if (m_dblPitchOutput > 0) {
		double dblActual = m_dblPitchOutput + 600;
		command_ballast("AFT_BALLAST", dblActual);
		command_ballast("FWD_BALLAST", -dblActual);
	}
	else {
		command_ballast("AFT_BALLAST", 0);
		command_ballast("FWD_BALLAST", 0);
	}
	

}