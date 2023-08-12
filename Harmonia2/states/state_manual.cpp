// 
// 
// 

#include "state_manual.h"
#include "..\comms\rf_comms.h"
#include "..\control\syringe_ballast.h"
#include "..\helpers.h"

void apply_manual_command() {

	String strRemoteCommand = get_remote_command();
	String strRemoteParam = get_remote_param();

	if (strRemoteCommand.length() > 0) {
		if (strRemoteCommand == "FWD_BALLAST" || strRemoteCommand == "AFT_BALLAST") {
			//send_rf_comm("{inflating: now}");
			command_ballast(strRemoteCommand, strRemoteParam.toInt());

		}
		else if (strRemoteCommand == "BALLAST_SETPOINTS") {

			//send_rf_comm("ballast setpoints: " + strRemoteParam);

			double dblFwdPosSetpoint = get_sep_part_dbl(strRemoteParam, '|', 0);
			double dblAftPosSetpoint = get_sep_part_dbl(strRemoteParam, '|', 1);
			ballast_setpoints(dblFwdPosSetpoint, dblAftPosSetpoint);

		}
		
	}

}

