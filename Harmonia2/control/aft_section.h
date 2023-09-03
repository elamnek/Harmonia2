// aft_section.h

#ifndef _AFT_SECTION_h
#define _AFT_SECTION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void aft_section_init();

void aft_setpoints(byte* dataSetpoints);

#endif

