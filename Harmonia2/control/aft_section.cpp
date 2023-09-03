// 
// 
// 

#include "aft_section.h"
#include <Wire.h>

void aft_section_init() {
	
	Wire1.begin();
}

void aft_setpoints(byte* dataSetpoints) {
	
	Wire1.beginTransmission(8);
	Wire1.write("+*");
	Wire1.write(dataSetpoints[0]);
	Wire1.write(dataSetpoints[1]);
	Wire1.write(dataSetpoints[2]);
	Wire1.write(dataSetpoints[3]);
	Wire1.write("*+");
	Wire1.endTransmission();
}