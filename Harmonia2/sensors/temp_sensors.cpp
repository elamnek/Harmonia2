// 
// 
// 

#include "temp_sensors.h"

int m_intFwdTempSensorPin = A17;
int m_intAftTempSensorPin = A15;

void init_tempsensors() {
	pinMode(m_intFwdTempSensorPin, INPUT);
	pinMode(m_intAftTempSensorPin, INPUT);
}

double read_fwd_temp() {
	return Thermister(analogRead(m_intFwdTempSensorPin));
}
double read_aft_temp() {
	return Thermister(analogRead(m_intAftTempSensorPin));
}



//function sourced from: https://www.jaycar.com.au/medias/sys_master/images/images/9963122556958/XC4494-manualMain.pdf
double Thermister(int RawADC) {
	double Temp;
	Temp = log(((10240000 / RawADC) - 10000));
	Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp)) * Temp);
	Temp = Temp - 273.15; // Convert Kelvin to Celcius
	return Temp;
}