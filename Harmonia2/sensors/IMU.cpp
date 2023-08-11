// links to libraries/examples:
// https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/arduino-code
// 

#include "IMU.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);
float m_fltOrientation_x;
float m_fltOrientation_y;
float m_fltOrientation_z;

float m_fltAcceleration_x;
float m_fltAcceleration_y;
float m_fltAcceleration_z;

String init_imu() {

    if (!bno.begin()) {
        return "ERROR: IMU sensor failed to initialise";
    }

    delay(1000);

    bno.setExtCrystalUse(true);
  
    m_fltOrientation_x = 0.0;
    m_fltOrientation_y = 0.0;
    m_fltOrientation_z = 0.0;

    m_fltAcceleration_x = 0.0;
    m_fltAcceleration_y = 0.0;
    m_fltAcceleration_z = 0.0;

    return "";

}

void read_imu() {

    //get orientation data from IMU
    sensors_event_t orientationData;
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);

    m_fltOrientation_x = orientationData.orientation.x;
    m_fltOrientation_y = orientationData.orientation.y;
    m_fltOrientation_z = orientationData.orientation.z;

    //get acceleration data from IMU
    sensors_event_t linearAccelData;
    bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);

    m_fltAcceleration_x = linearAccelData.acceleration.x;
    m_fltAcceleration_y = linearAccelData.acceleration.y;
    m_fltAcceleration_z = linearAccelData.acceleration.z;

}

int8_t read_imu_temp() {

    return bno.getTemp();
}

float get_imuorientation_x() {
    return m_fltOrientation_x;
}

float get_imupitch() {

    sensors_event_t orientationData;
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);

    return orientationData.orientation.z;
}
float get_imuorientation_y() {
    return m_fltOrientation_y;
}
float get_imuorientation_z() {
    return m_fltOrientation_z;
}
float get_imuacceleration_x() {
    return m_fltAcceleration_x;
}
float get_imuacceleration_y() {
    return m_fltAcceleration_y;
}
float get_imuacceleration_z() {
    return m_fltAcceleration_z;
}
