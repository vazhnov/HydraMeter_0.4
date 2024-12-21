#include "MCP436X.h"
#include <SPI.h>

//crap it's actually the MCP346X.  I don't feel like rewriting it

const int MCP436X::ADC_NUM_OSRS;// = 15;
const int32_t MCP436X::OSRvsDiv[MCP436X::ADC_NUM_OSRS] = {96, 192, 384,768,1536,2048,3072,5120,9216,17408,21504,25600,41984,50176,82944,99328};
const  MCP436X::MCP436X_OSR_t MCP436X::allOSRs[MCP436X::ADC_NUM_OSRS] = {OSR32,OSR64,OSR128,OSR256,OSR512,OSR1024,OSR2048,OSR4096,OSR8192,OSR16384,OSR20480,OSR24576,OSR40960,OSR49152,OSR81920,OSR98304};
const int MCP436X::allOSRVals[MCP436X::ADC_NUM_OSRS] = {32,64,128,256,512,1024,2048,4096,                      8192,16384,20480,24576,40960,49152,81920,98304};

uint8_t MCP436X::adcTxfer(uint8_t in){
	digitalWrite(CS_Pin,1);
	SPI.beginTransaction(SPISettings(MCP_SPI_RATE, MSBFIRST, SPI_MODE3));
	delayMicroseconds(1);
	digitalWrite(CS_Pin,0);
	delayMicroseconds(1);
	uint8_t res = SPI.transfer(in);
	delayMicroseconds(1);
	digitalWrite(CS_Pin,1);
	SPI.endTransaction();
	return res;
}





uint8_t MCP436X::adcFastCmd(uint8_t cmd){
	digitalWrite(CS_Pin,1);
	SPI.beginTransaction(SPISettings(MCP_SPI_RATE, MSBFIRST, SPI_MODE3));
	delayMicroseconds(1);
	digitalWrite(CS_Pin,0);
	delayMicroseconds(1);
	uint8_t res = SPI.transfer((ADC_ADDR<<6) | ((cmd & 0xF)<<2) | ADC_CMD_FAST);
	delayMicroseconds(1);
	digitalWrite(CS_Pin,1);
	SPI.endTransaction();

	return res;
}
uint8_t MCP436X::adcRead8(uint8_t reg){
	digitalWrite(CS_Pin,1);
	SPI.beginTransaction(SPISettings(MCP_SPI_RATE, MSBFIRST, SPI_MODE3));
	delayMicroseconds(1);
	digitalWrite(CS_Pin,0);
	delayMicroseconds(1);

	SPI.transfer((ADC_ADDR<<6) | ((reg & 0xF)<<2) | ADC_CMD_STATIC_READ);
	uint8_t res = SPI.transfer(0);
	
	delayMicroseconds(1);
	digitalWrite(CS_Pin,1);
	SPI.endTransaction();
	return res;
}
uint16_t MCP436X::adcRead16(uint8_t reg){
	digitalWrite(CS_Pin,1);
	SPI.beginTransaction(SPISettings(MCP_SPI_RATE, MSBFIRST, SPI_MODE3));
	delayMicroseconds(1);
	digitalWrite(CS_Pin,0);
	delayMicroseconds(1);

	SPI.transfer((ADC_ADDR<<6) | ((reg&0xF)<<2) | ADC_CMD_STATIC_READ);
	uint16_t res = 0;
	res = SPI.transfer(0);
	res = (res<<8) | SPI.transfer(0);

	delayMicroseconds(1);
	digitalWrite(CS_Pin,1);
	SPI.endTransaction();

	return res;
}
uint32_t MCP436X::adcRead24(uint8_t reg){
	digitalWrite(CS_Pin,1);
	SPI.beginTransaction(SPISettings(MCP_SPI_RATE, MSBFIRST, SPI_MODE3));
	delayMicroseconds(1);
	digitalWrite(CS_Pin,0);
	delayMicroseconds(1);
	SPI.transfer((ADC_ADDR<<6) | ((reg&0xF)<<2) | ADC_CMD_INCR_READ);
	uint32_t res = 0;
	res = SPI.transfer(0);
	res = (res<<8) | SPI.transfer(0);
	res = (res<<8) | SPI.transfer(0);
	delayMicroseconds(1);
	digitalWrite(CS_Pin,1);
	SPI.endTransaction();
	return res;
}
uint32_t MCP436X::adcRead32Stat(uint8_t reg, uint8_t* stat){
	digitalWrite(CS_Pin,1);
	SPI.beginTransaction(SPISettings(MCP_SPI_RATE, MSBFIRST, SPI_MODE3));
	delayMicroseconds(1);
	digitalWrite(CS_Pin,0);
	delayMicroseconds(1);
	if(stat != NULL){
		*stat = SPI.transfer((ADC_ADDR<<6) | ((reg&0xF)<<2) | ADC_CMD_INCR_READ);
	}else{
		SPI.transfer((ADC_ADDR<<6) | ((reg&0xF)<<2) | ADC_CMD_INCR_READ);
	}
	uint32_t res = 0;
	res = SPI.transfer(0);
	res = (res<<8) | (uint32_t)SPI.transfer(0);
	res = (res<<8) | (uint32_t)SPI.transfer(0);
	res = (res<<8) | (uint32_t)SPI.transfer(0);
	delayMicroseconds(1);
	digitalWrite(CS_Pin,1);
	SPI.endTransaction();
	return res;
}
uint8_t MCP436X::adcWrite8(uint8_t reg, uint8_t val){
	digitalWrite(CS_Pin,1);
	SPI.beginTransaction(SPISettings(MCP_SPI_RATE, MSBFIRST, SPI_MODE3));
	delayMicroseconds(1);
	digitalWrite(CS_Pin,0);
	delayMicroseconds(1);
	uint8_t stat = SPI.transfer((ADC_ADDR<<6) | ((reg&0xF)<<2) | ADC_CMD_INCR_WRITE);
	SPI.transfer(val);
	delayMicroseconds(1);
	digitalWrite(CS_Pin,1);
	SPI.endTransaction();
	return stat;
}
uint8_t MCP436X::adcWrite16(uint8_t reg, uint16_t val){
	digitalWrite(CS_Pin,1);
	SPI.beginTransaction(SPISettings(MCP_SPI_RATE, MSBFIRST, SPI_MODE3));
	delayMicroseconds(1);
	digitalWrite(CS_Pin,0);
	delayMicroseconds(1);
	uint8_t stat = SPI.transfer((ADC_ADDR<<6) | ((reg&0xF)<<2) | ADC_CMD_INCR_WRITE);
	SPI.transfer((uint8_t)((val>>8) & 0xFF));
	SPI.transfer((uint8_t)((val) & 0xFF));
	delayMicroseconds(1);
	digitalWrite(CS_Pin,1);
	SPI.endTransaction();
	return stat;
}
uint8_t MCP436X::adcWrite24(uint8_t reg, uint32_t val){
	digitalWrite(CS_Pin,1);
	SPI.beginTransaction(SPISettings(MCP_SPI_RATE, MSBFIRST, SPI_MODE3));
	delayMicroseconds(1);
	digitalWrite(CS_Pin,0);
	delayMicroseconds(1);
	uint8_t stat = SPI.transfer((ADC_ADDR<<6) | ((reg&0xF)<<2) | ADC_CMD_INCR_WRITE);
	SPI.transfer((uint8_t)((val>>16) & 0xFF));
	SPI.transfer((uint8_t)((val>>8) & 0xFF));
	SPI.transfer((uint8_t)((val) & 0xFF));
	delayMicroseconds(1);
	digitalWrite(CS_Pin,1);
	SPI.endTransaction();
	return stat;
}
void MCP436X::channelSelect(MCP436X_Mux_Type_t ch){
	uint8_t reg = 0;
	switch(ch){
	case Ch0V:
		reg = ADC_MUX_CHANNELS(ADC_MUX_CHANNEL_0,ADC_MUX_CHANNEL_1);
	break;
	case Ch0VGndRef:
		reg = ADC_MUX_CHANNELS(ADC_MUX_CHANNEL_0,ADC_MUX_CHANNEL_AGND);
	break;
	case SelfVoltage:
		reg = ADC_MUX_CHANNELS(ADC_MUX_CHANNEL_AVDD,ADC_MUX_CHANNEL_AGND);
	break;
	case Temperature:
		reg = ADC_MUX_CHANNELS(ADC_MUX_CHANNEL_TEMPP,ADC_MUX_CHANNEL_TEMPM);
	break;
	}
	adcWrite8(ADC_REG_MUX, reg);
}
MCP436X::MCP436X(int CSpin){
	CS_Pin = CSpin;
}
void MCP436X::begin(){
	pinMode(CS_Pin,OUTPUT);
	digitalWrite(CS_Pin,HIGH);
	SPI.begin();
}
//returns clock cycles per sample
uint32_t MCP436X::confiure(){
	adcFastCmd(ADC_FAST_RESET);
	adcRead8(0xD);
	//delay(10);
	adcWrite8(ADC_REG_IRQ,0xF2);
	//delay(1);
	adcWrite8(ADC_REG_CONFIG0,0x82);
	//delay(1);
	adcWrite8(ADC_REG_CONFIG1,ADC_CONFIG1_PRESCALE_1 | ADC_CONFIG1_OSR256);
	uint32_t result = 4*256;
	//delay(1);
	adcWrite8(ADC_REG_CONFIG2,0x8B);
	//delay(1);
	adcWrite8(ADC_REG_CONFIG3, ADC_CONFIG3_CONVMODE_CONT);
	//delay(1);
	adcWrite8(ADC_REG_MUX,0x01);
	//delay(1);
	adcWrite24(ADC_REG_SCAN,0x00);
	//delay(1);
	return result;
}

//same as configure, but takes OSR as an arg
void MCP436X::reConfig(uint8_t OSRreg){
	adcFastCmd(ADC_FAST_RESET);
	adcRead8(0xD);
	adcWrite8(ADC_REG_IRQ,0xF2);
	adcWrite8(ADC_REG_CONFIG0,0x82);
	adcWrite8(ADC_REG_CONFIG1,ADC_CONFIG1_PRESCALE_1 | OSRreg);
	adcWrite8(ADC_REG_CONFIG2,0x8B);
	adcWrite8(ADC_REG_CONFIG3, ADC_CONFIG3_CONVMODE_CONT);
	adcWrite8(ADC_REG_MUX,0x01);
	adcWrite24(ADC_REG_SCAN,0x00);
	delay(1);
}


void MCP436X::setResnRate(double sampleRate, int numEndpoints){
	
	stopReadings();
	
	double maxFreq = 5000000; //max master clock that we want to run the ADC at
	double maxDMCLK = maxFreq / 4;
	uint8_t OSRreg = ADC_CONFIG1_OSR256;
	int pickedI = 0;
	double desMCLK = maxFreq;
	
	if(numEndpoints > 1){
		for(int i=ADC_NUM_OSRS-1;i>=0;i--){
			if(4*allOSRVals[i]*sampleRate*numEndpoints <= maxDMCLK){ //OSRvsDiv[i]
				OSRreg = ADC_CONFIG1_OSR_I_TO_REG(i);
				desMCLK = 16*allOSRVals[i]*sampleRate*numEndpoints;
				changeFreqOnPin(desMCLK);
				//createFreqOnPin(desMCLK,3);
				pickedI = i;
				break;//from OSR indexing
			}
		}
	}else{
		for(int i=ADC_NUM_OSRS-1;i>=0;i--){
			if( allOSRVals[i]*sampleRate <= maxDMCLK ){ //OSRvsDiv[i]
				OSRreg = ADC_CONFIG1_OSR_I_TO_REG(i);
				desMCLK = 4*allOSRVals[i]*sampleRate;
				changeFreqOnPin(desMCLK);
				//createFreqOnPin(desMCLK,3);
				pickedI = i;
				break;//from OSR indexing
			}
		}
		
	}
	Serial.printf("Changing to %d and CLK %0.1fkhz [%d]\n",allOSRVals[pickedI],desMCLK/1000,pickedI);
	//adcWrite8(ADC_REG_CONFIG1,ADC_CONFIG1_PRESCALE_1 | OSRreg);
	reConfig(OSRreg);
	startReadings();
	
}

void MCP436X::startReadings(){
	adcWrite8(ADC_REG_CONFIG0 , 0x83);
	adcWrite8(ADC_REG_CONFIG3 , ADC_CONFIG3_CONVMODE_CONT);
	delayMicroseconds(1);
	adcFastCmd(ADC_FAST_CONV_START);
}
void MCP436X::stopReadings(){
	adcFastCmd(ADC_FAST_STANDBY_MODE);
}
uint16_t MCP436X::adcReadLatest(){
	uint16_t adcVal;
	adcVal = adcRead16(ADC_REG_ADCDATA);
	return adcVal;
}
int16_t MCP436X::adcReadSingle(){
	//delayMicroseconds(150);
	//adcRead8(ADC_REG_IRQ);
	//adcWrite8(ADC_REG_CONFIG0,0x83);
	//delayMicroseconds(2);
	//adcFastCmd(ADC_FAST_CONV_START);
	//uint8_t cfg0 = adcRead8(ADC_REG_CONFIG0);
	uint32_t start = micros();
	uint8_t lastIRQ = 0x40;
	uint32_t adcVal = 0;
	while(lastIRQ & 0x40){
		lastIRQ = adcRead8(ADC_REG_IRQ);
		if(micros()-start > 2000){
			//Serial.println("ADC reading timed out!");
			break;
		}
	}
	lastConvTime = micros()-start;
	adcVal = adcRead16(ADC_REG_ADCDATA);
	//delayMicroseconds(1);
	return (int16_t)adcVal;
}

