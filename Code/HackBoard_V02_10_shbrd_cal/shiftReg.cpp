#include "shiftReg.h"

volatile uint8_t SRPins[3] = {0,0,0};
volatile uint8_t SRDigIn[2] = {0,0};
volatile bool doWriteSRPins = true;
uint32_t SRPinsLastUpdate = 0;
switches_t switches;

void digitalWriteSR(uint8_t pin,bool st){
	uint8_t ind = pin >> 3;
	uint8_t bit = pin & 7;
	if(ind > 2){
		return;
	}
	if(st){
		SRPins[ind] = SRPins[ind] | (1<<bit);
	}else{
		SRPins[ind] = SRPins[ind] & (~(1<<bit));
	}
	doWriteSRPins = true;
}


void writeSRPins(){
	uint32_t dif = millis()-SRPinsLastUpdate;
	if(dif > 50){
		doWriteSRPins = true;
	}
	if(doWriteSRPins){
		doWriteSRPins = false;
		digitalWrite(SR_RCK,0);
		SPI1.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
		delayMicroseconds(1);
		SPI1.transfer(SRPins[2]);
		delayMicroseconds(1);
		SPI1.transfer(SRPins[1]);
		delayMicroseconds(1);
		SPI1.transfer(SRPins[0]);
		delayMicroseconds(1);
		digitalWrite(SR_RCK,1);
		SRPinsLastUpdate = millis();
		delayMicroseconds(2);
		SRDigIn[0] = SPI1.transfer(0);
		SRDigIn[1] = SPI1.transfer(0);
		digitalWrite(SR_RCK,0);
		SPI.endTransaction();
	}
}

void zeroSwitches(){
	switches.volts = 0;
	switches.amps = 0;
	switches.mA = 0;
	switches.ohm = 0;
	switches.autorange = 0;
	switches.power = 0;
	switches.altUp = 0;
	switches.altDn = 0;
	switches.rng1Up = 0;
	switches.rng1Dn = 0;
	switches.rng0Up = 0;
	switches.rng0Dn = 0;
	switches.momUp = 0;
	switches.momDn = 0;
	switches.explic = 0;
}

void readSwitches(){
	uint32_t swt = 0;
	swt = (((uint32_t)SRDigIn[1])<<8) | ((uint32_t)SRDigIn[0]);

	switches.volts     = (swt & SWITCH_SRPIN_SW4_UP )==0;
	switches.amps      = (swt & SWITCH_SRPIN_SW4_DN )==0;
	switches.mA        = (swt & SWITCH_SRPIN_MA     )==0;
	switches.ohm       = (swt & SWITCH_SRPIN_OHMS   )==0;
	switches.autorange = (swt & SWITCH_SRPIN_SW8    )==0;
	switches.power     = (swt & SWITCH_SRPIN_SW0    )==0;
	switches.altUp     = (swt & SWITCH_SRPIN_SW3_UP )==0;
	switches.altDn     = (swt & SWITCH_SRPIN_SW3_DN )==0;
	switches.rng1Up    = (swt & SWITCH_SRPIN_SW1_UP )==0;
	switches.rng1Dn    = (swt & SWITCH_SRPIN_SW1_DN )==0;
	switches.rng0Up    = (swt & SWITCH_SRPIN_SW2_UP )==0;
	switches.rng0Dn    = (swt & SWITCH_SRPIN_SW2_DN )==0;
	switches.momUp     = (swt & SWITCH_SRPIN_SW6_UP )==0;
	switches.momDn     = (swt & SWITCH_SRPIN_SW6_DN )==0;
	switches.explic    = (swt & SWITCH_SRPIN_SW7    )==0;

  /*switches.volts = 0;
  switches.amps = 0;
  switches.mA = 0;
  switches.ohm = 1;
  switches.autorange = 1;
  switches.power = 1;
  switches.altUp = 1;
  switches.altDn = 0;
  switches.rng1Up = 0;
  switches.rng1Dn = 0;
  switches.rng0Up = 0;
  switches.rng0Dn = 1;
  switches.explic = 1;
  switches.momUp = 0;
  switches.momDn = 0;*/
  /*static uint32_t tsl=0;
  if(millis()-tsl > 100){
    Serial.print(SRDigIn[1],BIN);
    Serial.print('\t');
    Serial.println(SRDigIn[0],BIN);
    tsl = millis();
  }*/


	/*if((~SRDigIn[1]) & 0xF0){
		Serial.print((~SRDigIn[1]),HEX);
	}else{
		Serial.print('0');
		Serial.print((~SRDigIn[1]),HEX);
	}
	Serial.print("\t");
	if((~SRDigIn[0]) & 0xF0){
		Serial.println((~SRDigIn[0]),HEX);
	}else{
		Serial.print('0');
		Serial.println((~SRDigIn[0]),HEX);
	}*/
}