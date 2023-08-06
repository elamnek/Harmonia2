// 
// 
// 

#include "water_sensors.h"

int m_intFwdWaterSensorPin = 16;
int m_intAftWaterSensorPin = 14;

void init_watersensors() {
	pinMode(m_intFwdWaterSensorPin, INPUT);
	pinMode(m_intAftWaterSensorPin, INPUT);
}

int fwd_leak_check() {
	//normal state of pins = 1 changes to 0 if water touches sensor track
	if (digitalRead(m_intFwdWaterSensorPin) == 1) {
		return 1;
	}
	return 0;
}
int aft_leak_check() {
	//normal state of pins = 1 changes to 0 if water touches sensor track
	if (digitalRead(m_intAftWaterSensorPin) == 1) {
		return 1;
	}
	return 0;
}


//analog reads for testing only - not used currently
boolean leak_detected2() {

	if (analogRead(m_intFwdWaterSensorPin) < 80) {
		//|| digitalRead(m_intAftWaterSensorPin) == 0

		return true;
	}
	return false;
}
uint8_t leak_read() {
	return analogRead(m_intFwdWaterSensorPin);
}


