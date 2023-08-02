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

//return adtf line
String sdcard_logState(subSystemState_t* state, int8_t logFlags) {
    String dataString = "{";

    //always include timestamp
    dataString += "13|" + state->logTime + ",";

    if ((logFlags & LOG_ACC) == LOG_ACC) {
        dataString += "32|" + String(state->acc[0]) + ","; //x direction
        dataString += "33|" + String(state->acc[1]) + ","; //y direction
        dataString += "34|" + String(state->acc[2]) + ","; //z direction
    }

    if ((logFlags & LOG_ATT) == LOG_ATT) {
        dataString += "14|" + String(state->att[0]) + ","; //heading
        dataString += "15|" + String(state->att[1]) + ","; //pitch
        dataString += "16|" + String(state->att[2]) + ","; //roll
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

