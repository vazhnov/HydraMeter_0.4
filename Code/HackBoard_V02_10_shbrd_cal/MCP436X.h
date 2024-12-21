#ifndef MCP436X_H
#define MCP436X_H
#include "Arduino.h"
#include "squarewave.h"
//crap it's actually the MCP*34*6X.  I don't feel like rewriting it
#define MCP_SPI_RATE  8000000
#define MCP_MCLK_RATE 5000000

#define ADC_CMD_STATIC_READ 1
#define ADC_CMD_INCR_WRITE 2
#define ADC_CMD_INCR_READ 3
#define ADC_CMD_FAST 0

#define ADC_ADDR 1

#define ADC_FAST_CONV_START      0xA
#define ADC_FAST_STANDBY_MODE    0xB
#define ADC_FAST_SHUTDOWN        0xC
#define ADC_FAST_FULL_SHUTDOWN   0xD
#define ADC_FAST_RESET           0xE

#define ADC_REG_ADCDATA   0x00  //4/16/32b
#define ADC_REG_CONFIG0   0x01  //8
#define ADC_REG_CONFIG1   0x02  //8
#define ADC_REG_CONFIG2   0x03  //8
#define ADC_REG_CONFIG3   0x04  //8
#define ADC_REG_IRQ       0x05  //8
#define ADC_REG_MUX       0x06  //8
#define ADC_REG_SCAN      0x07  //24
#define ADC_REG_TIMER     0x08  //24
#define ADC_REG_OFFSETCAL 0x09  //24
#define ADC_REG_GAINCAL   0x0A  //24
#define ADC_REG_LOCK      0x0D  //8
#define ADC_REG_CRCCFG    0x0F  //16

#define ADC_MUX_CHANNEL_0 0
#define ADC_MUX_CHANNEL_1 1
#define ADC_MUX_CHANNEL_2 2
#define ADC_MUX_CHANNEL_3 3
#define ADC_MUX_CHANNEL_4 4
#define ADC_MUX_CHANNEL_5 5
#define ADC_MUX_CHANNEL_6 6
#define ADC_MUX_CHANNEL_7 7
#define ADC_MUX_CHANNEL_AGND 8
#define ADC_MUX_CHANNEL_AVDD 9
#define ADC_MUX_CHANNEL_REFP 0xB
#define ADC_MUX_CHANNEL_REFM 0xC
#define ADC_MUX_CHANNEL_TEMPP 0XD
#define ADC_MUX_CHANNEL_TEMPM 0XE
#define ADC_MUX_CHANNEL_VCM 0XF

#define ADC_CONFIG1_PRESCALE_8 0xC0
#define ADC_CONFIG1_PRESCALE_4 0x80
#define ADC_CONFIG1_PRESCALE_2 0x40
#define ADC_CONFIG1_PRESCALE_1 0x00

#define ADC_CONFIG1_OSR32      0x00
#define ADC_CONFIG1_OSR64      0x04
#define ADC_CONFIG1_OSR128     0x08
#define ADC_CONFIG1_OSR256     0x0C
#define ADC_CONFIG1_OSR512     0x10
#define ADC_CONFIG1_OSR1024    0x14

#define ADC_CONFIG1_OSR_I_TO_REG(r)  ((r<<2) & 0x3C) //converts an OSR index within allOSRs[] to the register value for config1

#define ADC_CONFIG3_CONVMODE_CONT 0xC0
#define ADC_CONFIG3_CONVMODE_ONESHOT 0x80
#define ADC_CONFIG3_CONVMODE_OSHTSDN 0x40 //One-shot-shut-down

#define ADC_MUX_CHANNELS(a,b) (((a)<<4) | (b))

class MCP436X {
	public:

	typedef enum MCP436X_Type{
		MCP4361,
		MCP4362,
		MCP4364,
		MCP3465
	}MCP436X_Type_t;

	typedef enum MCP436X_Mux_Type{
		Ch0V,
		Ch0VGndRef,
		SelfVoltage,
		Temperature
	}MCP436X_Mux_Type_t;
	
	typedef enum MCP436X_OSR_t{
		OSR32,
		OSR64,
		OSR128,
		OSR256,
		OSR512,
		OSR1024,
		OSR2048,
		OSR4096,
		OSR8192,
		OSR16384,
		OSR20480,
		OSR24576,
		OSR40960,
		OSR49152,
		OSR81920,
		OSR98304
	}MCP436X_OSR_t;

	static const int ADC_NUM_OSRS = 16;//# of OSR options
	static const int32_t OSRvsDiv[ADC_NUM_OSRS];// = {96, 192, 384,768,1536,2048,3072,5120,9216,17408,21504,25600,41984,50176,82944,99328};//[ADC_NUM_OSRS];
	static const MCP436X_OSR_t allOSRs[ADC_NUM_OSRS];// = {OSR32,OSR64,OSR128,OSR256,OSR512,OSR1024,OSR2048,OSR4096,OSR8192,OSR16384,OSR20480,OSR24576,OSR40960,OSR49152,OSR81920,OSR98304};//[ADC_NUM_OSRS];
	static const int allOSRVals[ADC_NUM_OSRS];// = {32,64,128,256,512,1024,2048,4096,                      8192,16384,20480,24576,40960,49152,81920,98304};//[ADC_NUM_OSRS];
	
	uint8_t CS_Pin;
	int activeChannel;
	int16_t lastReading;
	double lastAdcValue;
	int32_t lastConvTime;
	
	MCP436X(int CS);
	MCP436X(int CS,MCP436X_Type_t type);

	void begin();
	void setRes();
	void setRate();
	uint32_t confiure();
	void reConfig(uint8_t OSRreg);
	void startReadings();
	void stopReadings();
	uint8_t adcTxfer(uint8_t in);
	uint8_t adcFastCmd(uint8_t cmd);
	uint8_t adcRead8(uint8_t reg);
	uint16_t adcRead16(uint8_t reg);
	uint32_t adcRead24(uint8_t reg);
	uint32_t adcRead32Stat(uint8_t reg, uint8_t* stat);
	uint8_t adcWrite8(uint8_t reg, uint8_t val);
	uint8_t adcWrite16(uint8_t reg, uint16_t val);
	uint8_t adcWrite24(uint8_t reg, uint32_t val);
	void setResnRate(double sampleRate, int numEndpoints);
	void channelSelect(MCP436X_Mux_Type_t ch);
	int16_t adcReadSingle();
	uint16_t adcReadLatest();
};
#endif