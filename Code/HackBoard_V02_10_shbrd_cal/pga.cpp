#include "pga.h"
double currentPGAGain = 1;
int activePGAChannel = 0;

void pgaSetGain(uint8_t in){
	uint8_t gainReg = ((uint8_t)in) & 7;
	static uint8_t lastSetGainReg = 255;
	if(gainReg == lastSetGainReg){
		return;
	}
	switch(in){
	case PGA_GAIN_1:
		currentPGAGain = 1;
	break;
	case PGA_GAIN_2:
		currentPGAGain = 2;
	break;
	case PGA_GAIN_4:
		currentPGAGain = 4;
	break;
	case PGA_GAIN_5:
		currentPGAGain = 5;
	break;
	case PGA_GAIN_8:
		currentPGAGain = 8;
	break;
	case PGA_GAIN_10:
		currentPGAGain = 10;
	break;
	case PGA_GAIN_16:
		currentPGAGain = 16;
	break;
	case PGA_GAIN_32:
		currentPGAGain = 32;
	break;
	}
	uint8_t cmd = PGA_CMD_WRITE_GAIN;
	digitalWrite(PGA_CS,1);//TODO remove delays and up SPI speed if possible
	SPI.beginTransaction(SPISettings(PGA_SPI_RATE, MSBFIRST, SPI_MODE3));
	delayMicroseconds(1);
	digitalWrite(PGA_CS,0);
	delayMicroseconds(1);

	SPI.transfer(cmd);
	SPI.transfer(gainReg);
	
	delayMicroseconds(1);
	digitalWrite(PGA_CS,1);
	SPI.endTransaction();
	lastSetGainReg = gainReg;
}

void pgaSetChannel(uint8_t newCh){
	static int lastChannel = -1;
	int in = newCh;
	if(in != lastChannel && in <= 7){
		uint8_t chReg = ((uint8_t)in) & 7;
		activePGAChannel = chReg;
		uint8_t cmd = PGA_CMD_WRITE_CH;
		digitalWrite(PGA_CS,1);
		SPI.beginTransaction(SPISettings(PGA_SPI_RATE, MSBFIRST, SPI_MODE3));
		delayMicroseconds(1);
		digitalWrite(PGA_CS,0);
		delayMicroseconds(1);

		SPI.transfer(cmd);
		SPI.transfer(chReg);
		
		delayMicroseconds(1);
		digitalWrite(PGA_CS,1);
		SPI.endTransaction();
		lastChannel = newCh;
		activePGAChannel = newCh;
		delayMicroseconds(PGA_CHCHANGE_DELAY);
	}else{
		//activePGAChannel = newCh;
	}
}