#ifndef SHIFTREF_H
#define SHIFTREF_H
#include "pins.h"
#include <Arduino.h>
#include <SPI.h>

extern volatile uint8_t SRPins[3];
extern volatile uint8_t SRDigIn[2];
extern volatile bool doWriteSRPins;

void digitalWriteSR(uint8_t pin,bool st);
void writeSRPins();
typedef struct switches_t{
bool volts:1; //4up
bool amps:1;  //4dn
bool mA:1;    //5
bool ohm:1;   //9
bool autorange:1; //8
bool power:1; //0
bool altUp:1; //3
bool altDn:1; //3
bool rng1Up:1;//1
bool rng1Dn:1;//1
bool rng0Up:1;//2
bool rng0Dn:1;//2
bool momUp:1; //6
bool momDn:1; //6
bool explic:1;//7
}switches_t;

extern switches_t switches;

void zeroSwitches();
void readSwitches();

#endif