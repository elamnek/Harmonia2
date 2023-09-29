// 
// 
// 

#include "power_sensor.h"

#include <Wire.h>
#include "power_sensor.h"
#include <DFRobot_INA219.h>

DFRobot_INA219_IIC ina219(&Wire, INA219_I2C_ADDRESS4);
boolean m_blnIna219_OK = false;

// Revise the following two paramters according to actual reading of the INA219 and the multimeter
// for linearly calibration
float ina219Reading_mA = 1000;
float extMeterReading_mA = 1000;

String init_powersensor() {

    delay(1000);

    if (!ina219.begin()) {
        return "ERROR: ina219 power sensor failed to initialise";
    }
    m_blnIna219_OK = true;
    ina219.linearCalibrate(ina219Reading_mA, extMeterReading_mA);

    return "";

}

float get_bus_voltage() {

    if (m_blnIna219_OK) {return ina219.getBusVoltage_V(); }
    return -1;
}
float get_shunt_voltage() {
    if (m_blnIna219_OK) {return ina219.getShuntVoltage_mV();}
    return -1;
}
float get_current_mA() {
    if (m_blnIna219_OK) {return ina219.getCurrent_mA();}
    return -1;
}
float get_power_mW() {
    if (m_blnIna219_OK) {return ina219.getPower_mW();}
    return -1;
}

