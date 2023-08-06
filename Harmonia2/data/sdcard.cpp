// 
// 
// 

#include "sdcard.h"
#include <SD.h>

String init_sdcard() {
    
    //SD Card Initialization
    if (!SD.begin(BUILTIN_SDCARD))
    {
        return "ERROR: SDCard failed to initialise";
    }

    return "";	
}

//returns adtf line
String sdcard_logState(subSystemState_t* state, int8_t logFlags) {
    String dataString = "{";

    //always include timestamp and state
    dataString += "13|" + state->logTime + ",";
    dataString += "4|" + state->FSMState + ",";

    if ((logFlags & LOG_ACCELERATION) == LOG_ACCELERATION) {
        dataString += "32|" + String(state->acceleration[0]) + ","; //x direction
        dataString += "33|" + String(state->acceleration[1]) + ","; //y direction
        dataString += "34|" + String(state->acceleration[2]) + ","; //z direction
    }

    if ((logFlags & LOG_ATTITUDE) == LOG_ATTITUDE) {
        dataString += "14|" + String(state->attitude[0]) + ","; //heading
        dataString += "15|" + String(state->attitude[1]) + ","; //pitch
        dataString += "16|" + String(state->attitude[2]) + ","; //roll
    }

    if ((logFlags & LOG_DEPTH) == LOG_DEPTH) {
        dataString += "1|" + String(state->depth) + ",";
    }

    if ((logFlags & LOG_POSITION) == LOG_POSITION) {
        dataString += "20|" + String(state->xPos) + ",";
    }

    if ((logFlags & LOG_BALLAST) == LOG_BALLAST) {
        dataString += "17|" + String(state->balPos[0]) + ","; //fwd ballast
        dataString += "18|" + String(state->balPos[1]) + ","; //aft ballast
        dataString += "36|" + String(state->balMotorTemp[0]) + ","; //fwd ballast motor temp
        dataString += "37|" + String(state->balMotorTemp[1]) + ","; //aft ballast motor temp
        dataString += "38|" + String(state->balMotorSpeed[0]) + ","; //fwd ballast motor controller speed setting
        dataString += "39|" + String(state->balMotorSpeed[1]) + ","; //aft ballast motor controller speed setting
        dataString += "12|" + String(state->internalTemp) + ","; //internal IMU recorded temp
    }

    if ((logFlags & LOG_SURFACES) == LOG_SURFACES) {
        dataString += "27|" + String(state->dpPos) + ",";

        dataString += "28|" + String(state->tailPos[0]) + ",";
        dataString += "29|" + String(state->tailPos[1]) + ",";
        dataString += "30|" + String(state->tailPos[2]) + ",";
        dataString += "31|" + String(state->tailPos[3]) + ",";
    }
    if ((logFlags & LOG_MOTOR) == LOG_MOTOR) {
        dataString += "22|" + String(state->motorTh) + ",";
        dataString += "7|" + String(state->mototRPM) + ",";
        dataString += "23|" + String(state->motorVolt) + ",";
        dataString += "25|" + String(state->motorCurr) + ",";
    }

    //always include uC time since start
    dataString += "35|" + String(millis()) + "}";

    //do the actual write
    File dataFile = SD.open("data.log", FILE_WRITE);
    dataFile.println(dataString);
    dataFile.close();

    return dataString;
}

