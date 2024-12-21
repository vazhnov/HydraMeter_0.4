#ifndef UTILITIES_H
#define UTILITIES_H
#include "Arduino.h"
#include "pico.h"

#ifndef PI
	#define PI (3.141592653589793238462643383279502)
#endif 

#define getTimeStamp() (time_us_64())
typedef union {
	uint8_t byteAr[4];
	float flt;
} floatToBytes_t;

void nrfPackRadioPacket(char* name,double val,uint8_t* buffer);

#endif