#include "Arduino.h"
#include "meterState.h"
#include "pga.h"
#include <pico/mutex.h>
#define MAIN_STATE_STR_LEN 32

extern mutex_t globalMutex;

namespace global{
	extern ampRange_t currentAmpRange;
	extern ampRange_t desiredAmpRange;
	extern ampRange_t nextAutoAmpRange;
	
	extern ohmRange_t currentOhmRange;
	extern ohmRange_t desiredOhmRange;
	extern ohmRange_t nextAutoOhmRange;
	
	extern double ohmRangeSmoothingDiv;
	extern double rMeter;
	extern voltRange_t vRange;
	
	extern voltRange_t currentVoltRange;
	extern voltRange_t desiredVoltRange;
	extern voltRange_t nextAutoVoltRange;

	extern pgaGain_t currentOhmPGA;
	extern pgaGain_t desiredOhmPGA;
	extern pgaGain_t nextAutoOhmPGA;
	
	extern pgaGain_t currentVoltPGA;
	extern pgaGain_t desiredVoltPGA;
	extern pgaGain_t nextAutoVoltPGA;
	
	extern pgaGain_t currentAmpPGA;
	extern pgaGain_t desiredAmpPGA;
	extern pgaGain_t nextAutoAmpPGA;

	extern boolean isAutoRangingR;
	extern boolean isAutoRangingV;
	extern boolean isAutoRangingI;

	extern uint64_t timeVRangeSet;
	extern uint64_t timeIRangeSet;
	extern uint64_t timeRRangeSet;

	extern int32_t lastADCReading;

	extern double currentVoltage;
	extern double currentACVoltage;
	extern double currentCurrent;
	extern double currentOhmCur;
	extern double currentResistance;
	extern double currentM0;
	extern double currentM1;

	extern int32_t lastVAdcRdg;
	extern int32_t lastIAdcRdg;
	extern int32_t lastRAdcRdg;	

	extern boolean ohmsIsActive;
	extern boolean diodeIsActive;
	extern boolean voltsIsActive;
	extern boolean powerIsActive;
	extern boolean ampsIsActive;
	extern boolean contIsActive;
	extern boolean errorState;
	extern boolean mod0IsActive;
	extern boolean mod1IsActive;

	extern boolean enableRPath;
	extern boolean disableRPath;
	extern boolean enableRSample;
	extern boolean enableRNoSample; //function for R not sampling
	extern boolean enableVSample;
	extern boolean enableVNoSample;
	extern boolean enableILowSample; //low-I ranges (uA/mA)
	extern boolean enableILowNoSample; 
	extern boolean enableIHighSample; //high-I range (amps)
	extern boolean enableIHighNoSample; 
	
	extern boolean doSendDisplayVoltage;
	extern boolean doSendDisplayCurrent;
	extern boolean doSendDisplayResistance;
	extern boolean doSendDisplayTestCurrent;
	
	extern boolean doSendVoltageRange;
	extern boolean doSendCurrentRange;
	extern boolean doSendOhmRange;
	
	extern uint32_t core0LoopTime;
	
	//extern char* mainStateStr;
	//extern char* voltRangeStr;
	//extern char* ampRangeStr;
	//extern char* ohmRangeStr;
};

//namespace c0{ 	void copyC0Global(); };
void copyC0Global();
namespace c0g{
	extern ampRange_t currentAmpRange;
	extern ampRange_t desiredAmpRange;
	extern ampRange_t nextAutoAmpRange;
	
	extern ohmRange_t currentOhmRange;
	extern ohmRange_t desiredOhmRange;
	extern ohmRange_t nextAutoOhmRange;
	
	extern double ohmRangeSmoothingDiv;
	extern double rMeter;
	extern voltRange_t vRange;
	
	extern voltRange_t currentVoltRange;
	extern voltRange_t desiredVoltRange;
	extern voltRange_t nextAutoVoltRange;

	extern pgaGain_t currentOhmPGA;
	extern pgaGain_t desiredOhmPGA;
	extern pgaGain_t nextAutoOhmPGA;
	
	extern pgaGain_t currentVoltPGA;
	extern pgaGain_t desiredVoltPGA;
	extern pgaGain_t nextAutoVoltPGA;
	
	extern pgaGain_t currentAmpPGA;
	extern pgaGain_t desiredAmpPGA;
	extern pgaGain_t nextAutoAmpPGA;

	extern boolean isAutoRangingR;
	extern boolean isAutoRangingV;
	extern boolean isAutoRangingI;

	extern uint64_t timeVRangeSet;
	extern uint64_t timeIRangeSet;
	extern uint64_t timeRRangeSet;

	extern int32_t lastADCReading;

	extern double currentVoltage;
	extern double currentACVoltage;
	extern double currentCurrent;
	extern double currentOhmCur;
	extern double currentResistance;
	extern double currentM0;
	extern double currentM1;

	extern int32_t lastVAdcRdg;
	extern int32_t lastIAdcRdg;
	extern int32_t lastRAdcRdg;	

	extern boolean ohmsIsActive;
	extern boolean diodeIsActive;
	extern boolean voltsIsActive;
	extern boolean powerIsActive;
	extern boolean ampsIsActive;
	extern boolean contIsActive;
	extern boolean errorState;
	extern boolean mod0IsActive;
	extern boolean mod1IsActive;

	extern boolean enableRPath;
	extern boolean disableRPath;
	extern boolean enableRSample;
	extern boolean enableRNoSample; //function for R not sampling
	extern boolean enableVSample;
	extern boolean enableVNoSample;
	extern boolean enableILowSample; //low-I ranges (uA/mA)
	extern boolean enableILowNoSample; 
	extern boolean enableIHighSample; //high-I range (amps)
	extern boolean enableIHighNoSample; 
	
	extern boolean doSendDisplayVoltage;
	extern boolean doSendDisplayCurrent;
	extern boolean doSendDisplayResistance;
	extern boolean doSendDisplayTestCurrent;
	
	extern boolean doSendVoltageRange;
	extern boolean doSendCurrentRange;
	extern boolean doSendOhmRange;
	
	extern uint32_t core0LoopTime;
};

//namespace c1{ void copyC1Global(); };
void copyC1Global();
namespace c1g{
	extern ampRange_t currentAmpRange;
	extern ampRange_t desiredAmpRange;
	extern ampRange_t nextAutoAmpRange;
	
	extern ohmRange_t currentOhmRange;
	extern ohmRange_t desiredOhmRange;
	extern ohmRange_t nextAutoOhmRange;
	
	extern double ohmRangeSmoothingDiv;
	extern double rMeter;
	extern voltRange_t vRange;
	
	extern voltRange_t currentVoltRange;
	extern voltRange_t desiredVoltRange;
	extern voltRange_t nextAutoVoltRange;

	extern pgaGain_t currentOhmPGA;
	extern pgaGain_t desiredOhmPGA;
	extern pgaGain_t nextAutoOhmPGA;
	
	extern pgaGain_t currentVoltPGA;
	extern pgaGain_t desiredVoltPGA;
	extern pgaGain_t nextAutoVoltPGA;
	
	extern pgaGain_t currentAmpPGA;
	extern pgaGain_t desiredAmpPGA;
	extern pgaGain_t nextAutoAmpPGA;

	extern boolean isAutoRangingR;
	extern boolean isAutoRangingV;
	extern boolean isAutoRangingI;

	extern uint64_t timeVRangeSet;
	extern uint64_t timeIRangeSet;
	extern uint64_t timeRRangeSet;

	extern int32_t lastADCReading;

	extern double currentVoltage;
	extern double currentACVoltage;
	extern double currentCurrent;
	extern double currentOhmCur;
	extern double currentResistance;
	extern double currentM0;
	extern double currentM1;

	extern int32_t lastVAdcRdg;
	extern int32_t lastIAdcRdg;
	extern int32_t lastRAdcRdg;	

	extern boolean ohmsIsActive;
	extern boolean diodeIsActive;
	extern boolean voltsIsActive;
	extern boolean voltsACIsActive;
	extern boolean powerIsActive;
	extern boolean ampsIsActive;
	extern boolean contIsActive;
	extern boolean errorState;
	extern boolean mod0IsActive;
	extern boolean mod1IsActive;

	extern boolean enableRPath;
	extern boolean disableRPath;
	extern boolean enableRSample;
	extern boolean enableRNoSample; //function for R not sampling
	extern boolean enableVSample;
	extern boolean enableVNoSample;
	extern boolean enableILowSample; //low-I ranges (uA/mA)
	extern boolean enableILowNoSample; 
	extern boolean enableIHighSample; //high-I range (amps)
	extern boolean enableIHighNoSample; 
	
	extern boolean doSendDisplayVoltage;
	extern boolean doSendDisplayVoltageAC;
	extern boolean doSendDisplayCurrent;
	extern boolean doSendDisplayResistance;
	extern boolean doSendDisplayTestCurrent;
	extern boolean doSendDisplayMod;
	extern boolean doRadioTx;
	
	extern boolean doSendDisplayModVoltage;
	extern boolean doSendDisplayModCurrent;
	extern boolean doSendDisplayModPower;
	extern boolean doSendDisplayModResistance;
	
	extern boolean doSendVoltageRange;
	extern boolean doSendCurrentRange;
	extern boolean doSendOhmRange;
	
	extern uint32_t core0LoopTime;
	
	extern char* mainStateStr;
	extern char* voltRangeStr;
	extern char* ampRangeStr;
	extern char* ohmRangeStr;
};