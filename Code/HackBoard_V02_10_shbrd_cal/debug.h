#ifndef DEBUG_H
#define DEBUG_H
#include "Arduino.h"

//#define DEBUG_1


#ifdef DEBUG_1
	#define d1print(A)		Serial.print(A)
	#define d1printHex(A)	Serial.print(A,HEX)
	#define d1print2(A,B)	Serial.print(A,B)
	#define d1println(A)	Serial.println(A)
	
	#define d1print2L(A,B)	Serial.print(A);Serial.println(B)
	#define d1printLHex(A)	Serial.println(A,HEX)
	#define d1printL2(A,B)	Serial.println(A,B)
#else
	#define d1print(A)	
	#define d1printHex(A)
	#define d1print2(A,B)
	#define d1println(A)
	
	#define d1print2L(A,B)
	#define d1printLHex(A)
	#define d1printL2(A,B)
#endif

#ifdef DEBUG_2
	#define d2print(A)		Serial.print(A)
	#define d2printHex(A)	Serial.print(A,HEX)
	#define d2print2(A,B)	Serial.print(A,B)
	#define d2println(A)	Serial.println(A)
	
	#define d2print2L(A,B)	Serial.print(A);Serial.println(B)
	#define d2printLHex(A)	Serial.println(A,HEX)
	#define d2printL2(A,B)	Serial.println(A,B)
#else
	#define d2print(A)	
	#define d2printHex(A)
	#define d2print2(A,B)
	#define d2println(A)
	
	#define d2print2L(A,B)
	#define d2printLHex(A)
	#define d2printL2(A,B)
#endif


#endif