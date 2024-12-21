#include "globals.h"

char mainStateStrLc[MAIN_STATE_STR_LEN];//TODO, move to global::
mutex_t globalMutex;//MUST be held to alter OR read any global::

namespace global{
	
	ampRange_t currentAmpRange = ampRange_disabled;
	ampRange_t desiredAmpRange;
	ampRange_t nextAutoAmpRange;
	
	ohmRange_t currentOhmRange;
	ohmRange_t desiredOhmRange;
	ohmRange_t nextAutoOhmRange;
	
	double ohmRangeSmoothingDiv = 0.1;
	double rMeter = 10e6;
	voltRange_t vRange = voltRange_0;
	
	voltRange_t currentVoltRange;
	voltRange_t desiredVoltRange;
	voltRange_t nextAutoVoltRange;
	
	pgaGain_t currentOhmPGA;
	pgaGain_t desiredOhmPGA;
	pgaGain_t nextAutoOhmPGA;
	
	pgaGain_t currentVoltPGA;
	pgaGain_t desiredVoltPGA;
	pgaGain_t nextAutoVoltPGA;
	
	pgaGain_t currentAmpPGA;
	pgaGain_t desiredAmpPGA;
	pgaGain_t nextAutoAmpPGA;

	boolean isAutoRangingR;
	boolean isAutoRangingV;
	boolean isAutoRangingI;
	
	uint64_t timeVRangeSet = 0;
	uint64_t timeIRangeSet = 0;
	uint64_t timeRRangeSet = 0;
	
	int32_t lastADCReading = 0;

	double currentVoltage=0;
	double currentACVoltage=0;
	double currentCurrent=0;
	double currentOhmCur=0;
	double currentResistance=0;
	double currentM0 = 0;
	double currentM1 = 0;

	int32_t lastVAdcRdg=0;
	int32_t lastIAdcRdg=0;
	int32_t lastRAdcRdg=0;	
	
	boolean ohmsIsActive = false;
	boolean diodeIsActive = false;
	boolean voltsIsActive = false;
	boolean powerIsActive = false;
	boolean ampsIsActive = false;
	boolean contIsActive = false;
	boolean errorState = false;
	boolean mod0IsActive = false;
	boolean mod1IsActive = false;

	boolean enableRPath = false;
	boolean disableRPath = true;
	boolean enableRSample = false;
	boolean enableRNoSample = false; //function for R not sampling
	boolean enableVSample = false;
	boolean enableVNoSample = false;
	boolean enableILowSample = false; //low-I ranges (uA/mA)
	boolean enableILowNoSample = false;
	boolean enableIHighSample = false; //high-I range (amps)
	boolean enableIHighNoSample = false;
	
	boolean doSendDisplayVoltage = false;
	boolean doSendDisplayCurrent = false;
	boolean doSendDisplayResistance = false;
	boolean doSendDisplayTestCurrent = false;
	
	boolean doSendVoltageRange;
	boolean doSendCurrentRange;
	boolean doSendOhmRange;
	
	uint32_t core0LoopTime = 0;
	//TODO, actually do a string copy for these, DONT allow pointers to the same address
	//within both cores
	//char* mainStateStr = &mainStateStrLc[0];
	//char* voltRangeStr = &voltRangeNames[3][0];
	//char* ampRangeStr = &ampRangeNames[3][0];
	//char* ohmRangeStr = &ohmRangeNames[9][0];

};

namespace c0g{
	ampRange_t currentAmpRange = ampRange_disabled;
	//ampRange_t desiredAmpRange;
	//ampRange_t nextAutoAmpRange;
	
	ohmRange_t currentOhmRange;
	//ohmRange_t desiredOhmRange;
	ohmRange_t nextAutoOhmRange;
	
	double ohmRangeSmoothingDiv = 0.1;
	double rMeter = 10e6;
	voltRange_t vRange = voltRange_0;
	
	voltRange_t currentVoltRange;
	//voltRange_t desiredVoltRange;
	voltRange_t nextAutoVoltRange;
	
	pgaGain_t currentOhmPGA;
	//pgaGain_t desiredOhmPGA;
	pgaGain_t nextAutoOhmPGA;
	
	pgaGain_t currentVoltPGA;
	//pgaGain_t desiredVoltPGA;
	pgaGain_t nextAutoVoltPGA;
	
	pgaGain_t currentAmpPGA;
	//pgaGain_t desiredAmpPGA;
	pgaGain_t nextAutoAmpPGA;

	boolean isAutoRangingR;
	boolean isAutoRangingV;
	boolean isAutoRangingI;
	
	//uint64_t timeVRangeSet = 0;
	//uint64_t timeIRangeSet = 0;
	//uint64_t timeRRangeSet = 0;
	
	int32_t lastADCReading = 0;

	double currentVoltage=0;
	double currentACVoltage=0;
	double currentCurrent=0;
	double currentOhmCur=0;
	double currentResistance=0;
	double currentM0 = 0;
	double currentM1 = 0;

	int32_t lastVAdcRdg=0;
	int32_t lastIAdcRdg=0;
	int32_t lastRAdcRdg=0;	
	
	boolean ohmsIsActive = false;
	boolean diodeIsActive = false;
	boolean voltsIsActive = false;
	boolean powerIsActive = false;
	boolean ampsIsActive = false;
	boolean contIsActive = false;
	boolean errorState = false;
	boolean mod0IsActive = false;
	boolean mod1IsActive = false;
	
	//boolean enableRPath = false;
	//boolean disableRPath = true;
	boolean enableRSample = false;
	boolean enableRNoSample = false; //function for R not sampling
	boolean enableVSample = false;
	boolean enableVNoSample = false;
	boolean enableILowSample = false; //low-I ranges (uA/mA)
	boolean enableILowNoSample = false;
	boolean enableIHighSample = false; //high-I range (amps)
	boolean enableIHighNoSample = false;
	
	//volatile boolean doSendDisplayVoltage = false;
	//volatile boolean doSendDisplayCurrent = false;
	//volatile boolean doSendDisplayResistance = false;
	//volatile boolean doSendDisplayTestCurrent = false;
	
	//volatile boolean doSendVoltageRange;
	//volatile boolean doSendCurrentRange;
	//volatile boolean doSendOhmRange;
	
	uint32_t core0LoopTime = 0;
	
	//char* mainStateStr = &mainStateStrLc[0];
	//char* voltRangeStr = &voltRangeNames[3][0];
	//char* ampRangeStr = &ampRangeNames[3][0];
	//char* ohmRangeStr = &ohmRangeNames[9][0];
};

namespace c1g{
	ampRange_t currentAmpRange = ampRange_disabled;
	ampRange_t desiredAmpRange;
	ampRange_t nextAutoAmpRange;
	
	ohmRange_t currentOhmRange;
	ohmRange_t desiredOhmRange;
	ohmRange_t nextAutoOhmRange;
	
	double ohmRangeSmoothingDiv = 0.1;
	double rMeter = 10e6;
	voltRange_t vRange = voltRange_0;
	
	voltRange_t currentVoltRange;
	voltRange_t desiredVoltRange;
	voltRange_t nextAutoVoltRange;
	
	pgaGain_t currentOhmPGA;
	pgaGain_t desiredOhmPGA;
	pgaGain_t nextAutoOhmPGA;
	
	pgaGain_t currentVoltPGA;
	pgaGain_t desiredVoltPGA;
	pgaGain_t nextAutoVoltPGA;
	
	pgaGain_t currentAmpPGA;
	pgaGain_t desiredAmpPGA;
	pgaGain_t nextAutoAmpPGA;

	boolean isAutoRangingR;
	boolean isAutoRangingV;
	boolean isAutoRangingI;
	
	uint64_t timeVRangeSet = 0;
	uint64_t timeIRangeSet = 0;
	uint64_t timeRRangeSet = 0;
	
	int32_t lastADCReading = 0;

	double currentVoltage=0;
	double currentACVoltage=0;
	double currentCurrent=0;
	double currentOhmCur=0;
	double currentResistance=0;
	double currentM0 = 0;
	double currentM1 = 0;

	int32_t lastVAdcRdg=0;
	int32_t lastIAdcRdg=0;
	int32_t lastRAdcRdg=0;	
	
	boolean ohmsIsActive = false;
	boolean diodeIsActive = false;
	boolean voltsIsActive = false;
	boolean voltsACIsActive = false;
	boolean powerIsActive = false;
	boolean ampsIsActive = false;
	boolean contIsActive = false;
	boolean errorState = false;
	boolean mod0IsActive = false;
	boolean mod1IsActive = false;

	boolean enableRPath = false;
	boolean disableRPath = true;
	boolean enableRSample = false;
	boolean enableRNoSample = false; //function for R not sampling
	boolean enableVSample = false;
	boolean enableVNoSample = false;
	boolean enableILowSample = false; //low-I ranges (uA/mA)
	boolean enableILowNoSample = false;
	boolean enableIHighSample = false; //high-I range (amps)
	boolean enableIHighNoSample = false;
	
	boolean doSendDisplayVoltage = false;
	boolean doSendDisplayVoltageAC = false;
	boolean doSendDisplayCurrent = false;
	boolean doSendDisplayResistance = false;
	boolean doSendDisplayTestCurrent = false;
	boolean doSendDisplayMod = false;
	boolean doRadioTx = false; //sends wireless in addition to serial
	
	boolean doSendDisplayModVoltage = false;
	boolean doSendDisplayModCurrent = false;
	boolean doSendDisplayModPower = false;
	boolean doSendDisplayModResistance = false;
	
	boolean doSendVoltageRange;
	boolean doSendCurrentRange;
	boolean doSendOhmRange;
	
	uint32_t core0LoopTime = 0;
	
	char* mainStateStr = &mainStateStrLc[0];
	char* voltRangeStr = &voltRangeNames[3][0];
	char* ampRangeStr = &ampRangeNames[3][0];
	char* ohmRangeStr = &ohmRangeNames[9][0];
};

//evil preprocessor hackey stuff.  Thanks to Weijun Zhou on stackoverflow for figuring this out
//basically to keep this mess of a global state in check, each var can only be set by one core, based on its local copy.
//to make sure that's enforced, this will cause a compiler error if any variable is set in both functions
//it does this by making an additional "extern global" variable with a name derived from the variables name, with a 
//different datatype for each function.  If multiple are declared, the types conflict and its an error
//since they're extern and don't exist elsewhere, none of these variables are actually created in memory

//#define SET_GLOBAL_HEADER_0 using _globalOnceCheck = decltype([](){}) //place this ONCE at the start of any function that sets globals
//#define CORE0_GLOBAL_HEADER typedef struct c0gh{}c0gh; using _globalOnceCheck = c0gh; //TODO make this not hacky
//#define CORE1_GLOBAL_HEADER typedef struct c1gh{}c1gh; using _globalOnceCheck = c1gh;
#define SET_GLOBAL_HEADER using _globalOnceCheck = struct{};
//use SET_GLOBAL to set any global.  do not use global:: for "var", so to set global::myVar = newVal; do SET_GLOBAL(myVar,newVal);
#define SET_GLOBAL(var, val) do{extern _globalOnceCheck checkSetOnce##var; global::var = (val);}while(0)

void copyC0Global(){
	SET_GLOBAL_HEADER;
	bool didGet = false;
	didGet = mutex_try_enter(&globalMutex,NULL);
	if(didGet){
		//extern bool hello;
		c0g::currentAmpRange = global::currentAmpRange;
		//c0g::desiredAmpRange = global::desiredAmpRange;
		//global::nextAutoAmpRange = c0g::nextAutoAmpRange;
		
		c0g::currentOhmRange = global::currentOhmRange;
		//ohmRange_t desiredOhmRange;
		SET_GLOBAL(nextAutoOhmRange , c0g::nextAutoOhmRange);
		
		c0g::ohmRangeSmoothingDiv = global::ohmRangeSmoothingDiv;
		c0g::rMeter = global::rMeter;
		c0g::vRange = global::vRange;
		
		c0g::currentVoltRange = global::currentVoltRange;
		//voltRange_t desiredVoltRange;
		SET_GLOBAL(nextAutoVoltRange , c0g::nextAutoVoltRange);
		
		c0g::currentOhmPGA = global::currentOhmPGA;
		//pgaGain_t desiredOhmPGA;
		SET_GLOBAL(nextAutoOhmPGA , c0g::nextAutoOhmPGA);
		
		//currentVoltPGA
		//desiredVoltPGA
		//nextAutoVoltPGA
		
		//currentAmpPGA
		//desiredAmpPGA
		//nextAutoAmpPGA

		c0g::isAutoRangingR = global::isAutoRangingR;
		c0g::isAutoRangingV = global::isAutoRangingV;
		c0g::isAutoRangingI = global::isAutoRangingI;
		
		//uint64_t timeVRangeSet = 0;
		//uint64_t timeIRangeSet = 0;
		//uint64_t timeRRangeSet = 0;
		
		SET_GLOBAL(lastADCReading , c0g::lastADCReading);

		SET_GLOBAL(currentVoltage , c0g::currentVoltage);
		SET_GLOBAL(currentACVoltage , c0g::currentACVoltage);
		SET_GLOBAL(currentCurrent , c0g::currentCurrent);
		SET_GLOBAL(currentOhmCur , c0g::currentOhmCur);
		SET_GLOBAL(currentResistance , c0g::currentResistance);
		SET_GLOBAL(currentM0 , c0g::currentM0);
		SET_GLOBAL(currentM1 , c0g::currentM1);

		SET_GLOBAL(lastVAdcRdg , c0g::lastVAdcRdg);
		SET_GLOBAL(lastIAdcRdg , c0g::lastIAdcRdg);
		SET_GLOBAL(lastRAdcRdg , c0g::lastRAdcRdg);	
		
		c0g::ohmsIsActive = global::ohmsIsActive;
		c0g::diodeIsActive = global::diodeIsActive;
		c0g::voltsIsActive = global::voltsIsActive;
		c0g::powerIsActive = global::powerIsActive;
		c0g::ampsIsActive = global::ampsIsActive;
		c0g::contIsActive = global::contIsActive;
		c0g::errorState = global::errorState;
		c0g::mod0IsActive = global::mod0IsActive;
		c0g::mod1IsActive = global::mod1IsActive;

		//c0g::enableRPath     = global::enableRPath;
		//c0g::disableRPath    = global::disableRPath;
		c0g::enableRSample   = global::enableRSample;
		c0g::enableRNoSample = global::enableRNoSample;//function for R not sampling
		c0g::enableVSample   = global::enableVSample;
		c0g::enableVNoSample = global::enableVNoSample;
		c0g::enableILowSample    = global::enableILowSample; //low-I ranges (uA/mA)
		c0g::enableILowNoSample  = global::enableILowNoSample;
		c0g::enableIHighSample   = global::enableIHighSample; //high-I range (amps)
		c0g::enableIHighNoSample = global::enableIHighNoSample;
		
		SET_GLOBAL(core0LoopTime , c0g::core0LoopTime);	
		
		//char* mainStateStr = &mainStateStrLc[0];
		//char* voltRangeStr = &voltRangeNames[3][0];
		//char* ampRangeStr = &ampRangeNames[3][0];
		//char* ohmRangeStr = &ohmRangeNames[9][0];
		mutex_exit(&globalMutex);
	}
}

void copyC1Global(){
	SET_GLOBAL_HEADER;
	bool didGet = false;
	didGet = mutex_try_enter(&globalMutex,NULL);
	if(didGet){
		//extern int hello;
		SET_GLOBAL(currentAmpRange , c1g::currentAmpRange);
		//global::desiredAmpRange , c1g::desiredAmpRange;
		//c1g::nextAutoAmpRange = global::nextAutoAmpRange;
		
		SET_GLOBAL(currentOhmRange , c1g::currentOhmRange);
		//ohmRange_t desiredOhmRange;
		c1g::nextAutoOhmRange = global::nextAutoOhmRange;
		
		SET_GLOBAL(ohmRangeSmoothingDiv , c1g::ohmRangeSmoothingDiv);
		SET_GLOBAL(rMeter , c1g::rMeter);
		SET_GLOBAL(vRange , c1g::vRange);
		
		SET_GLOBAL(currentVoltRange , c1g::currentVoltRange);
		//voltRange_t desiredVoltRange;
		c1g::nextAutoVoltRange = global::nextAutoVoltRange;
		
		SET_GLOBAL(currentOhmPGA , c1g::currentOhmPGA);
		//pgaGain_t desiredOhmPGA;
		c1g::nextAutoOhmPGA = global::nextAutoOhmPGA;
		
		//currentVoltPGA
		//desiredVoltPGA
		//nextAutoVoltPGA
		
		//currentAmpPGA
		//desiredAmpPGA
		//nextAutoAmpPGA

		SET_GLOBAL(isAutoRangingR , c1g::isAutoRangingR);
		SET_GLOBAL(isAutoRangingV , c1g::isAutoRangingV);
		SET_GLOBAL(isAutoRangingI , c1g::isAutoRangingI);
		
		//uint64_t timeVRangeSet = 0;
		//uint64_t timeIRangeSet = 0;
		//uint64_t timeRRangeSet = 0;
		
		c1g::lastADCReading = global::lastADCReading;

		c1g::currentVoltage = global::currentVoltage;
		c1g::currentACVoltage = global::currentACVoltage;
		c1g::currentCurrent = global::currentCurrent;
		c1g::currentOhmCur = global::currentOhmCur;
		c1g::currentResistance = global::currentResistance;
		c1g::currentM0 = global::currentM0;
		c1g::currentM1 = global::currentM1;
		
		c1g::lastVAdcRdg = global::lastVAdcRdg;
		c1g::lastIAdcRdg = global::lastIAdcRdg;
		c1g::lastRAdcRdg = global::lastRAdcRdg;	
		
		SET_GLOBAL(ohmsIsActive , c1g::ohmsIsActive);
		SET_GLOBAL(diodeIsActive , c1g::diodeIsActive);
		SET_GLOBAL(voltsIsActive , c1g::voltsIsActive);
		SET_GLOBAL(powerIsActive , c1g::powerIsActive);
		SET_GLOBAL(ampsIsActive , c1g::ampsIsActive);
		SET_GLOBAL(contIsActive , c1g::contIsActive);
		SET_GLOBAL(errorState , c1g::errorState);
		SET_GLOBAL(mod0IsActive , c1g::mod0IsActive);
		SET_GLOBAL(mod1IsActive , c1g::mod1IsActive);

		SET_GLOBAL(enableRPath     , c1g::enableRPath);
		SET_GLOBAL(disableRPath    , c1g::disableRPath);
		SET_GLOBAL(enableRSample   , c1g::enableRSample);
		SET_GLOBAL(enableRNoSample , c1g::enableRNoSample);//function for R not sampling
		SET_GLOBAL(enableVSample   , c1g::enableVSample);
		SET_GLOBAL(enableVNoSample , c1g::enableVNoSample);
		SET_GLOBAL(enableILowSample    , c1g::enableILowSample); //low-I ranges (uA/mA)
		SET_GLOBAL(enableILowNoSample  , c1g::enableILowNoSample);
		SET_GLOBAL(enableIHighSample   , c1g::enableIHighSample); //high-I range (amps)
		SET_GLOBAL(enableIHighNoSample , c1g::enableIHighNoSample);
		
		c1g::core0LoopTime = global::core0LoopTime;
		
		mutex_exit(&globalMutex);
	}
}
