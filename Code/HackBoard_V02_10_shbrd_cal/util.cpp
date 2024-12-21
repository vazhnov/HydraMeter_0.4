#include "util.h"

void nrfPackRadioPacket(char* name,double val,uint8_t* buffer){
	buffer[0] = name[0];
	buffer[1] = name[1];
	float trncVal  = (float)val;
	floatToBytes_t radioFltToBytes;
	radioFltToBytes.flt = trncVal;
	buffer[2] = radioFltToBytes.byteAr[0];
	buffer[3] = radioFltToBytes.byteAr[1];
	buffer[4] = radioFltToBytes.byteAr[2];
	buffer[5] = radioFltToBytes.byteAr[3];
}