#include <SPI.h>
#include "pins.h"
#include "MCP436X.h"
#include "calibration.h"
#include "genVar.h"
#include "debug.h"
#include "meterState.h"
#include "fnList.h"
#include "shiftReg.h"
#include "pga.h"
#include "globals.h"
#include "squarewave.h"
#include "IIRFilter.h"
#include "movingAvg.h"
#include "backup1.h"
#include <nrf24.h>



MCP436X mainAdc(ADC_CS);

//BIG outstanding parts:
/*
3)switch matrix to output different genVars
4)Make genvars work good.  Main thing is cal fns.
*/

//operation = what the meter is doing (i.e. "measure voltage" is an operation
//function is used in the programming sense.  
//main loop executes a series of selection functions, which configure the meter and execute an or several operations

IIR iir180HzV(1200,180,10);
IIR iir120HzV(1200,120,10);
IIR iir60HzV(1200,60,10);
movingAvgf rmsAccum;
nrf24 nrfRadio(RADIO_DO0,RADIO_CS);//ce,cs
const uint64_t pipes[2] = { 0xE7E7E7E7E7LL, 0xC2C2C2C2C2LL };
//const uint64_t pipe = 0xC2C2C2C2C2LL;

//boolean alwaysFalse = false;
boolean alwaysFalse1 = false;
boolean alwaysFalse0 = false;
//boolean alwaysTrue = true;
boolean alwaysTrue1 = true;
boolean alwaysTrue0 = true;

//c0=core0  and (surprise!) c1=core1
//namespaces are used to separate core fns
//functions are hardcoded to one core or another

//core0 handles sampling and "real time" stuff (this doesn't run an
//actual RTOS, but we're shooting for decent sampling speed).  

//core1 handles "slow"/non-"real-time" operations, shift regs, serial, UI, etc.
namespace c1{
	void interpSwitches();
  void onBrdAnalogRead();
	void setVState();
	void setIState();
	void setRState();
	void disableR();
	void printState();
	void printV();
	void printVAC();
	void printI();
	void printR();
	void printRI();
	void printMod();
	void radioLoopFn(); 
	void serialRxHandler();
  boolean isOnBattery = false;
  uint32_t timeStOnBattery = 0;
};

namespace c0{
	void configADC();
	void readADCSingle();

	void prepForRSample();
	void setRMuxChannel();
	void setRPGAGain();
	void applyRICal();

	void prepForVSample();
	void setVMuxChannel();
	void setVPGAGain();
	void applyVCal();

	void prepForIHighSample();
	void prepForILowSample();
	void setIMuxChannel();
	void setIPGAGain();
	void applyICal();
	
	void prepForM0Sample();
	void prepForM1Sample();
	void setModPGAGain();
	void setMod0MuxChannel();
	void setMod1MuxChannel();
	void applyM0Cal();
	void applyM1Cal();
};


void configLoopFunctions(){
	//global state copy should always come first
	fnListAdd(   fnList_t{&copyC0Global,    &alwaysTrue0,          "c0tg"});
	secFnListAdd(fnList_t{&copyC1Global,    &alwaysTrue1,          "c1tg"});
	
	
	fnListAdd(fnList_t{&c0::configADC,      &alwaysTrue0,          "ADCc"});
	fnListAdd(fnList_t{&c0::setVMuxChannel, &c0g::voltsIsActive,   "vmux"});
	fnListAdd(fnList_t{&c0::setVPGAGain,    &c0g::voltsIsActive,   "vpga"});
	fnListAdd(fnList_t{&c0::prepForVSample, &c0g::enableVSample,   "vpre"});
	fnListAdd(fnList_t{&c0::readADCSingle,  &c0g::enableVSample,   "vadc"});
	fnListAdd(fnList_t{&c0::applyVCal,      &c0g::voltsIsActive,   "vcal"});

	fnListAdd(fnList_t{&c0::setRMuxChannel, &c0g::ohmsIsActive,    "rmux"});
	fnListAdd(fnList_t{&c0::setRPGAGain,    &c0g::ohmsIsActive,    "rpga"});
	fnListAdd(fnList_t{&c0::prepForRSample, &c0g::enableRSample,   "rpre"});
	fnListAdd(fnList_t{&c0::readADCSingle,  &c0g::enableRSample,   "vohm"});
	fnListAdd(fnList_t{&c0::applyRICal,     &c0g::ohmsIsActive,    "rcal"});
	
	fnListAdd(fnList_t{&c0::setIMuxChannel, &c0g::ampsIsActive,      "imux"});
	fnListAdd(fnList_t{&c0::setIPGAGain,    &c0g::ampsIsActive,      "ipga"});
	fnListAdd(fnList_t{&c0::prepForILowSample, &c0g::enableILowSample,  "ilpr"});
	fnListAdd(fnList_t{&c0::prepForIHighSample, &c0g::enableIHighSample, "ihpr"});
	fnListAdd(fnList_t{&c0::readADCSingle,  &c0g::enableILowSample,  "iadl"});
	fnListAdd(fnList_t{&c0::readADCSingle,  &c0g::enableIHighSample, "iadh"});
	fnListAdd(fnList_t{&c0::applyICal,      &c0g::ampsIsActive,      "ical"});

	fnListAdd(fnList_t{&c0::setMod0MuxChannel, &c0g::mod0IsActive,   "m0mx"});
	fnListAdd(fnList_t{&c0::setModPGAGain,     &c0g::mod0IsActive,   "m0pg"});
	fnListAdd(fnList_t{&c0::prepForM0Sample,   &c0g::mod0IsActive,   "m0pr"});
	fnListAdd(fnList_t{&c0::readADCSingle,     &c0g::mod0IsActive,   "m0ad"});
	fnListAdd(fnList_t{&c0::applyM0Cal,        &c0g::mod0IsActive,   "m0cl"});
	
	fnListAdd(fnList_t{&c0::setMod1MuxChannel, &c0g::mod1IsActive,   "m1mx"});
	fnListAdd(fnList_t{&c0::setModPGAGain,     &c0g::mod1IsActive,   "m1pg"});
	fnListAdd(fnList_t{&c0::prepForM1Sample,   &c0g::mod1IsActive,   "m1pr"});
	fnListAdd(fnList_t{&c0::readADCSingle,     &c0g::mod1IsActive,   "m1ad"});
	fnListAdd(fnList_t{&c0::applyM1Cal,        &c0g::mod1IsActive,   "mqcl"});

////////////////////////////////////////////////////////////////////////////////
	secFnListAdd(fnList_t{&readSwitches,    &alwaysTrue1,          "rdsw"});
	secFnListAdd(fnList_t{&c1::interpSwitches, &alwaysTrue1,       "itsw"});
  secFnListAdd(fnList_t{&c1::onBrdAnalogRead, &alwaysTrue1,      "anrd"});

	secFnListAdd(fnList_t{&c1::setVState,   &c1g::voltsIsActive,   "vsta"});
	secFnListAdd(fnList_t{&c1::setRState,   &c1g::ohmsIsActive,    "rst"});
	secFnListAdd(fnList_t{&c1::disableR,    &c1g::disableRPath,    "disR"});
	secFnListAdd(fnList_t{&c1::setIState,   &c1g::ampsIsActive,    "ista"});
	
	secFnListAdd(fnList_t{&c1::printV,      &c1g::doSendDisplayVoltage,    "prtV"});
	secFnListAdd(fnList_t{&c1::printVAC,    &c1g::doSendDisplayVoltageAC,  "prVA"});
	secFnListAdd(fnList_t{&c1::printI,      &c1g::doSendDisplayCurrent,    "prtI"});
	secFnListAdd(fnList_t{&c1::printR,      &c1g::doSendDisplayResistance, "prtR"});
	secFnListAdd(fnList_t{&c1::printRI,     &c1g::doSendDisplayTestCurrent,"prRI"});
	secFnListAdd(fnList_t{&c1::printMod,    &c1g::doSendDisplayMod,        "prtM"});
	secFnListAdd(fnList_t{&c1::printState,  &alwaysTrue1,                  "prtS"});

	secFnListAdd(fnList_t{&writeSRPins,        &alwaysTrue1,           "srpn"});
	secFnListAdd(fnList_t{&c1::radioLoopFn,    &alwaysTrue1,           "rdlp"});
	secFnListAdd(fnList_t{&c1::serialRxHandler,&alwaysTrue1,           "seRX"});
}

mutex_t Core1LockMutex;
void setup(){
	mutex_init(&globalMutex);
	mutex_init(&Core1LockMutex);
	if(!mutex_try_enter(&Core1LockMutex,NULL)){
		delayMicroseconds(1);
		if(!mutex_try_enter(&Core1LockMutex,NULL)){
			Serial.begin(115200);
			Serial1.begin(115200);
			while(1){
				Serial.println("PANIC!!! Core0 could not lock Core1 at boot! Cannot init!");
				Serial1.println("PANIC!!! Core0 could not lock Core1 at boot! Cannot init!");
				delay(1000);
			}
		}
	}
	
	pinMode(BATTERY_EN,OUTPUT);
	digitalWrite(BATTERY_EN,1);
	pinMode(D5V_ISO_EN,OUTPUT);
	digitalWrite(D5V_ISO_EN,0);
	Serial.begin(115200);
	Serial1.begin(115200);
	mainAdc.begin();
	SPI1.setSCK(SPI1_SCK);
	SPI1.setRX(SPI1_MISO);
	SPI1.setTX(SPI1_MOSI);
	SPI1.begin(false);

	pinMode(PGA_CS,OUTPUT);
	digitalWrite(PGA_CS,1);
	
	pinMode(SR_RCK,OUTPUT);
	pinMode(RADIO_CS,OUTPUT);
	pinMode(RADIO_DO0,OUTPUT);
	pinMode(INMOD_CS,OUTPUT);
	pinMode(ONEWIRE,OUTPUT);
	pinMode(ONEWIRE_BRD,OUTPUT);
	pinMode(A0,INPUT);
  pinMode(A2,INPUT);
  analogReadResolution(10);

	digitalWrite(SR_RCK,0);
	digitalWrite(RADIO_CS,1);
	digitalWrite(RADIO_DO0,0);
	digitalWrite(INMOD_CS,1);
	digitalWrite(ONEWIRE,1);
	digitalWrite(ONEWIRE_BRD,1);

	SRPins[0] = 0;
	SRPins[1] = 0;
	SRPins[2] = 0;
	uint32_t ccps = mainAdc.confiure();
	double desSampleRate = 1;
	double desMCLK = ccps*desSampleRate;
	createFreqOnPin(desMCLK,ADC_MCLK);
	iir60HzV.compute(desSampleRate,60,10);
	iir120HzV.compute(desSampleRate,120,10);
	iir180HzV.compute(desSampleRate,180,10);
	rmsAccum.clear();
	delay(500);
	nrfRadio.begin(RF24_PTX,6);
	delay(20);
	mainAdc.startReadings();
	configLoopFunctions();
	
	mutex_exit(&Core1LockMutex);
	delay(500);
}

void setup1(){
	//this MUST be first!  Core0 has authority to set up anything before core1 starts doing stuff
	delay(100);//give core0 time to get the lock mutex
	while(!mutex_try_enter(&Core1LockMutex,NULL)){
		delay(10);
	}
	//at this point core0 has released lock, we're free to start running
	mutex_exit(&Core1LockMutex);//re-release, so core0 can lock us later (if implemented)
}

void configureMeter();
pgaGain_t vPgaGain = PGA_GAIN_1;
pgaGain_t iPgaGain = PGA_GAIN_1;
pgaGain_t rPgaGain = PGA_GAIN_1;
uint32_t dtl = 0;

void loop(){
	static uint32_t lastTimeStamp=0;
	uint32_t loopStartUs = micros();
	//c0g::core0LoopTime = loopStartUs-lastTimeStamp; 
	//dtl = timeSinceLastLoop;
	//Serial.print("DT=");Serial.println(timeSinceLastLoop);
	for(int i=0;i<mainFnListLength;i++){
		if(*(mainFnList[i].run)){
			//Serial.println((mainFnList[i].name));
			(*mainFnList[i].fn)();
		}
	}
	lastTimeStamp = loopStartUs;
}

void loop1(){
	static uint32_t lastTimeStamp=0;
	uint32_t loopStartUs = micros();
	uint32_t timeSinceLastLoop = loopStartUs-lastTimeStamp; 
	//Serial.print("DT=");Serial.println(dtl);
	for(int i=0;i<secFnListLength;i++){
		if(*(secFnList[i].run)){
			//Serial.println((secFnList[i].name));
			(*secFnList[i].fn)();
		}
	}
	/*uint8_t temp[32];
	delayMicroseconds(10);
	nrfRadio.loopFn();
	static uint32_t tsl = 0;
	if(millis()-tsl > 100){
		uint8_t buf[8];
		uint32_t time = millis();
		char nm[2] = {'V','\0'};
		nrfPackRadioPacket(nm , c1g::currentVoltage , buf);
		nrfRadio.write(buf);
		tsl = millis();
	}
	*/
	lastTimeStamp = loopStartUs;	
}

void configureMeter(){
}

void c1::interpSwitches(){
  
	if(switches.volts){
		//voltage
		c1g::voltsIsActive = true;
		c1g::ohmsIsActive = false;
		c1g::errorState = false;
		c1g::ampsIsActive = false;
		c1g::contIsActive = false;
		c1g::diodeIsActive = false;
		c1g::powerIsActive = false;
		
		c1g::enableRPath = false;
		c1g::disableRPath = true;
		c1g::enableRSample = false;
		c1g::enableRNoSample = false;
		
		c1g::enableVSample = true;
		c1g::enableVNoSample = false;
		
		c1g::enableILowSample = false;
		c1g::enableILowNoSample = false;
		c1g::enableIHighSample = false;
		c1g::enableIHighNoSample = false;
		
		c1g::mod0IsActive = false;
		c1g::mod1IsActive = false;
		c1g::doSendDisplayMod = false;
		
		if(switches.autorange){
			c1g::isAutoRangingV = true;
		}else{
			c1g::isAutoRangingV = false;
			if(switches.rng0Up){
				c1g::desiredVoltRange = voltRange_2;
			}else if (switches.rng0Dn){
				c1g::desiredVoltRange = voltRange_0;
			}else{
				c1g::desiredVoltRange = voltRange_1;
			}
		}
		
		if(switches.altUp){//AC only
			c1g::voltsACIsActive = true;
			c1g::doSendDisplayVoltageAC = true;
			c1g::doSendDisplayVoltage = false;
			snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN, "%s", "AC Voltage");
		}else if(switches.altDn){//both
			c1g::voltsACIsActive = true;
			c1g::doSendDisplayVoltageAC = true;
			c1g::doSendDisplayVoltage = true;
			snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN, "%s", "AC+DC Voltage");
		}else{//default DC mode
			c1g::voltsACIsActive = false;
			c1g::doSendDisplayVoltageAC = false;
			c1g::doSendDisplayVoltage = true;
			snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN, "%s", "DC Voltage");
		}
		
		c1g::doSendDisplayCurrent = false;
		c1g::doSendDisplayResistance = false;
		c1g::doSendDisplayTestCurrent = false;
	}else if(switches.amps){
		//current
		c1g::voltsIsActive = false;
		c1g::ohmsIsActive = false;
		c1g::errorState = false;
		c1g::ampsIsActive = true;
		c1g::contIsActive = false;
		c1g::diodeIsActive = false;
		c1g::powerIsActive = false;

		c1g::enableRPath = false;
		c1g::disableRPath = true;
		c1g::enableRSample = false;
		c1g::enableRNoSample = false;

		c1g::enableVSample = true;
		c1g::enableVNoSample = false;
		c1g::voltsACIsActive = false;
		
		c1g::mod0IsActive = false;
		c1g::mod1IsActive = false;
		c1g::doSendDisplayMod = false;
		
		if(switches.rng0Up){
			c1g::enableILowSample = false;
			c1g::enableIHighSample = true;
			c1g::desiredAmpRange = ampRange_A;
			c1g::currentAmpRange = ampRange_A;
      c1g::ampRangeStr = &ampRangeNames[2][0];
			snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN, "%s", "DC Current Amps");
		}else{
			c1g::enableILowSample = true;
			c1g::enableIHighSample = false;
			if(switches.mA){
				c1g::desiredAmpRange = ampRange_mA;
				c1g::currentAmpRange = ampRange_mA;
				snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN, "%s", "DC Current mA");
        c1g::ampRangeStr = &ampRangeNames[1][0];
			}else{
				c1g::desiredAmpRange = ampRange_uA;
				c1g::currentAmpRange = ampRange_uA;
				snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN, "%s", "DC Current uA");
        c1g::ampRangeStr = &ampRangeNames[0][0];
			}
		}

    if(switches.altUp){
      c1g::voltsIsActive = true;
      c1g::isAutoRangingV = true;
      c1g::doSendDisplayVoltage = true;
    }else{
      c1g::voltsIsActive = false;
      c1g::doSendDisplayVoltage = false;
    }

		c1g::enableIHighNoSample = false;
		c1g::enableILowNoSample = false;
		
		c1g::doSendDisplayCurrent = true;
		c1g::doSendDisplayResistance = false;
		//c1g::doSendDisplayVoltage = false;
		c1g::doSendDisplayVoltageAC = false;
		c1g::doSendDisplayTestCurrent = false;
		//snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN, "%s", "DC Current");
	}else{
		//alt mode
		c1g::voltsIsActive = true;
		c1g::voltsACIsActive = false;
		c1g::ohmsIsActive = true;
		c1g::errorState = false;
		c1g::ampsIsActive = false;
		c1g::contIsActive = false;
		c1g::diodeIsActive = false;
		c1g::powerIsActive = false;
		
		c1g::enableRPath = true;
		c1g::disableRPath = false;
		c1g::enableRSample = true;
		c1g::enableRNoSample = false;
		
		c1g::enableVSample = true;
		c1g::enableVNoSample = false;
		
		c1g::enableILowSample = false;
		c1g::enableILowNoSample = false;
		c1g::enableIHighSample = false;
		c1g::enableIHighNoSample = false;
		
		c1g::doSendDisplayCurrent = false;
		c1g::doSendDisplayResistance = false;
		c1g::doSendDisplayVoltage = false;
		c1g::doSendDisplayVoltageAC = false;
		c1g::doSendDisplayTestCurrent = false;
		
		c1g::mod0IsActive = false;
		c1g::mod1IsActive = false;
		c1g::doSendDisplayMod = false;
		if(switches.altUp){
			//diode mode
			c1g::isAutoRangingV = false;
			c1g::desiredVoltRange = voltRange_1;
			c1g::desiredOhmRange = ohmRange_0_highV;
			c1g::isAutoRangingR = false;
			c1g::doSendDisplayVoltage = true;
			char modifier0[] = "+IT\0";
			char modifier1[] = "100KR\0";
			if(switches.explic){
				c1g::doSendDisplayTestCurrent = true;
			}else{
				strcpy(modifier0,"\0");
			}
			int resRange = -1;
			if(switches.rng0Up){
				c1g::desiredOhmRange = ohmRange_2_highV;
			}else{
				if(switches.rng0Dn){
					c1g::desiredOhmRange = ohmRange_0_highV;
					strcpy(modifier1,"510R\0");
				}else{
					c1g::desiredOhmRange = ohmRange_1_highV;
					strcpy(modifier1,"10KR\0");
				}
			}
			snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN, "Diode %s%s", modifier0,modifier1);
		}else if (switches.altDn){
			//snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN, "%s", "Module");
			c1g::voltsIsActive = false;
			c1g::ohmsIsActive = false;
			c1g::enableRPath = false;
			c1g::enableRSample = false;
			c1g::enableVSample = false;
			c1g::mod0IsActive = true;
			c1g::mod1IsActive = true;
			c1g::doSendDisplayMod = true;
			
			if(switches.rng0Dn){
				c1g::doSendDisplayModVoltage = true;
				c1g::doSendDisplayModCurrent = true;
				c1g::doSendDisplayModPower = false;
				c1g::doSendDisplayModResistance = false;
				snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN,"Module V & I");
			}else if(switches.rng0Up){
				c1g::doSendDisplayModVoltage = true;
				c1g::doSendDisplayModCurrent = switches.explic;
				c1g::doSendDisplayModPower = true;
				c1g::doSendDisplayModResistance = false;
				snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN,"Module V & Pwr");
			}else{
				c1g::doSendDisplayModVoltage = switches.explic;
				c1g::doSendDisplayModCurrent = true;
				c1g::doSendDisplayModPower = switches.explic;
				c1g::doSendDisplayModResistance = true;
				snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN,"Module I & R");
			}
			
		}else{
			snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN, "%s", "Resistance");
			//resistance mode
			c1g::desiredVoltRange = voltRange_0;
			c1g::isAutoRangingV = false;
			if(switches.autorange){
				c1g::desiredOhmRange = ohmRange_3;//overridden but used when resuming from another mode
				c1g::isAutoRangingR = true;
				snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN,"Res AutoR");
			}else{
				c1g::isAutoRangingR = false;
				if(switches.rng0Up){
					if(switches.rng1Up){
						c1g::desiredOhmRange = ohmRange_3;
						snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN,"Res 150K+");
					}else{
						c1g::desiredOhmRange = ohmRange_2;
						snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN,"Res 15-150K");
					}
				}else{
					if(switches.rng0Dn){
						c1g::desiredOhmRange = ohmRange_0;
						snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN,"Res 0-330R");
					}else{
						c1g::desiredOhmRange = ohmRange_1;
						snprintf(c1g::mainStateStr,MAIN_STATE_STR_LEN,"Res 330-15K");
					}
				}
			}
			c1g::doSendDisplayCurrent = false;
			c1g::doSendDisplayResistance = true;
			c1g::doSendDisplayVoltageAC = false;
			if(switches.explic){
				c1g::doSendDisplayVoltage = true;
				c1g::doSendDisplayTestCurrent = true;
			}else{
				c1g::doSendDisplayVoltage = false;
				c1g::doSendDisplayTestCurrent = false;
			}
		}
	}
}

void c1::onBrdAnalogRead(){
  static uint32_t lastRd = 0;
  uint32_t mls = millis();
  if(mls - lastRd > 50){
    lastRd = mls;
    static float vBatFlt = 3.7;
    float vBat = analogRead(A2) * ((float)3.3 / 1024) * 2.65;//714;//((330.0f+510.0f) / (float)330.0);
    float vSys = analogRead(A0) * ((float)3.3 / 1024) * 2.5;//714;//((330.0f+510.0f) / (float)330.0);
    Serial.printf("Vbat: %f \tSys: %f\t%d\r\n",vBatFlt,vSys,analogRead(A2));
    vBatFlt = (vBat * 0.1) + (vBatFlt * 0.9);
    if(vBatFlt < 3.2){
      Serial.println("Battery low, shutting down");
      digitalWrite(BATTERY_EN,LOW);
      delay(100);
    }else{
      if(c1::isOnBattery && (millis() - c1::timeStOnBattery > 30000)){
        Serial.printf("On battery for %dmS, shutting down\r\n",millis()-timeStOnBattery);
        digitalWrite(BATTERY_EN,LOW);
        delay(100);
      }else{
        digitalWrite(BATTERY_EN,switches.power?HIGH:LOW);
      }
    }
  }
}

void c0::configADC(){
	static int prevNAdc=0;
	int nAdcRdgs = 0;
	if(c0g::enableVSample){
		nAdcRdgs++;
	}
	if(c0g::enableILowSample){
		nAdcRdgs++;
	}
	if(c0g::enableIHighSample){
		nAdcRdgs++;
	}
	if(c0g::enableRSample){
		nAdcRdgs++;
	}
	if(c0g::mod0IsActive){
		nAdcRdgs++;
	}
	if(c0g::mod1IsActive){
		nAdcRdgs++;
	}
	if(nAdcRdgs != prevNAdc){
		mainAdc.setResnRate(1000, nAdcRdgs);
	}
	prevNAdc = nAdcRdgs;
}

void c0::readADCSingle(){
	//delayMicroseconds(20);
	c0g::lastADCReading = mainAdc.adcReadSingle();
	c0g::core0LoopTime = mainAdc.lastConvTime;
}

void c0::prepForRSample(){
	if(c0g::enableVSample || c0g::enableILowSample || c0g::enableIHighSample){
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
	}
}

void c0::prepForVSample(){
	if(c0g::enableRSample || c0g::enableILowSample || c0g::enableIHighSample){
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
	}
}

void c0::prepForILowSample(){
	if(c0g::enableRSample || c0g::enableVSample || c0g::enableIHighSample){
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
	}
}
void c0::prepForIHighSample(){
	if(c0g::enableRSample || c0g::enableVSample || c0g::enableILowSample){
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
	}
}

void c0::prepForM0Sample(){
	if(c0g::mod1IsActive){
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
	}
}
void c0::prepForM1Sample(){
	if(c0g::mod0IsActive){
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
		c0::readADCSingle();
		delayMicroseconds(1);
	}
}

void c1::setRState(){
	static boolean wasAutoRanging = false;
	if(c1g::ohmsIsActive){
		if(c1g::isAutoRangingR){
			ohmRange_t nextRng;
			if(wasAutoRanging){
				nextRng = c1g::nextAutoOhmRange;
			}else{
				nextRng = c1g::desiredOhmRange;
			}
			if(nextRng == ohmRange_disabled){
				nextRng = ohmRange_3;
			}
			setOhmsRange(nextRng);
			wasAutoRanging = true;
		}else{
			setOhmsRange(c1g::desiredOhmRange);
			c1g::currentOhmRange = c1g::desiredOhmRange;
			wasAutoRanging = false;
		}
	}else{
		setOhmsRange(ohmRange_disabled);
		c1g::currentOhmRange = ohmRange_disabled;
		wasAutoRanging = false;
	}
}
void c1::disableR(){
	setOhmsRange(ohmRange_disabled);
	c1g::currentOhmRange = ohmRange_disabled;
}
void c0::setRMuxChannel(){
	pgaSetChannel(PGA_CHANNEL_OHMS);
}
void c0::setRPGAGain(){
	pgaSetGain(PGA_GAIN_1);
}
ohmRange_t ohmRangeDown(ohmRange_t in){
	static uint32_t lastSwitch = 0;
	if(millis()-lastSwitch > 200){
		lastSwitch = millis();
		switch(in){
		case ohmRange_0:
			return ohmRange_0;
		case ohmRange_1:
			return ohmRange_0;
		case ohmRange_2:
			return ohmRange_1;
		case ohmRange_3:
			return ohmRange_2;
		case ohmRange_0_highV:
			return ohmRange_0_highV;
		case ohmRange_1_highV:
			return ohmRange_0_highV;
		case ohmRange_2_highV:
			return ohmRange_1_highV;
		case ohmRange_3_highV:
			return ohmRange_2_highV;
		default:
			return ohmRange_disabled;
		}
	}
	return in;
}
ohmRange_t ohmRangeUp(ohmRange_t in){
	static uint32_t lastSwitch = 0;
	if(millis()-lastSwitch > 200){
		lastSwitch = millis();
		switch(in){
		case ohmRange_0:
			return ohmRange_1;
		case ohmRange_1:
			return ohmRange_2;
		case ohmRange_2:
			return ohmRange_3;
		case ohmRange_3:
			return ohmRange_3;
		case ohmRange_0_highV:
			return ohmRange_1_highV;
		case ohmRange_1_highV:
			return ohmRange_2_highV;
		case ohmRange_2_highV:
			return ohmRange_3_highV;
		case ohmRange_3_highV:
			return ohmRange_3_highV;
		default:
			return ohmRange_disabled;
		}
	}
	return in;
}
int32_t ohmADC = 0;
void c0::applyRICal(){
	int16_t adcRdg = c0g::lastADCReading;
	c0g::lastRAdcRdg = c0g::lastADCReading;
	ohmADC = adcRdg;
	double lowerRVoltage = adcRdg * ((double)2.4d/(1<<15));
	double upperRVoltage = lowerRVoltage * 3.3d/1.0;
	static double currentOutOfFe = 0;
	double div = c0g::ohmRangeSmoothingDiv;
	currentOutOfFe = ((ohmsFbToAmps(adcRdg)/1000) * div) + (currentOutOfFe*(1-div));
	c0g::currentOhmCur = currentOutOfFe;//TODO: redo this! cal's off! - (c0g::currentVoltage / c0g::rMeter);
	double rTest = c0g::currentVoltage / c0g::currentOhmCur;//1.0d / ((c0g::currentOhmCur/c0g::currentVoltage) - (1.0d/rMeter));
	c0g::currentResistance = rTest;
	if(c0g::isAutoRangingR){
		if(adcRdg > 12500){
			c0g::nextAutoOhmRange = ohmRangeDown(c0g::currentOhmRange);//range down
		}
		if((adcRdg < 500) || (c0g::currentVoltage > 1.55)){
			c0g::nextAutoOhmRange = ohmRangeUp(c0g::currentOhmRange);
		}
	}
}

void c1::setVState(){
	static boolean wasAutoRanging = false;
	if(c1g::isAutoRangingV){
		if(wasAutoRanging){
			c1g::desiredVoltRange = c1g::nextAutoVoltRange;
			//c1g::desiredVoltRange = c1g::currentVoltRange;
		}else{
			c1g::desiredVoltRange = c1g::currentVoltRange;
		}
		wasAutoRanging = true;
	}else{
		wasAutoRanging = false;
	}

	switch(c1g::desiredVoltRange){
	case voltRange_0:
		setRange(voltRange_0);
		c1g::currentVoltRange = c1g::desiredVoltRange; 
	break;
	case voltRange_1:
		setRange(voltRange_1);
		c1g::currentVoltRange = c1g::desiredVoltRange;
	break;
	case voltRange_2:
		setRange(voltRange_2);
		c1g::currentVoltRange = c1g::desiredVoltRange;
	break;
	default:
		setRange(voltRange_2);
		c1g::currentVoltRange = voltRange_2;//c1g::desiredVoltRange;
	}
}
void c0::setVMuxChannel(){
	pgaSetChannel(PGA_CHANNEL_VOLTS);
}
void c0::setVPGAGain(){
	pgaSetGain(PGA_GAIN_1);
}

voltRange_t voltRangeUp(voltRange_t in){
	static uint32_t lastSwitch = 0;
	if(millis()-lastSwitch > 200){
		lastSwitch = millis();
		switch(in){
		case voltRange_0:
			return voltRange_1;
		case voltRange_1:
			return voltRange_2;
		case voltRange_2:
			return voltRange_2;
		default:
			return in;
		}
	}
	return in;
}
voltRange_t voltRangeDown(voltRange_t in){
	static uint32_t lastSwitch = 0;
	if(millis()-lastSwitch > 200){
		lastSwitch = millis();
		switch(in){
		case voltRange_0:
			return voltRange_0;
		case voltRange_1:
			return voltRange_0;
		case voltRange_2:
			return voltRange_1;
		default:
			return in;
		}
	}
	return in;
}



void c0::applyVCal(){
	static double filter = 0;
	static double rmsFilter = 0;
	double V = ADCToVoltage(c0g::lastADCReading);
	double filtrd = V;//iir180HzV.apply(iir120HzV.apply(iir60HzV.apply(V)));
	c0g::currentVoltage = filtrd*0.05 + c0g::currentVoltage*0.95;
	
	//to compute AC, we subtract heavily-filtered DC reading, square it, batch 16 samples, send
	//those to a moving-window average, then take sqrt()
	static double ACBias = 0;
	ACBias = (V*0.0007) + (ACBias*0.9993); //DC bias used for AC computation
	static float sqSum = 0;//batch-of-16 accumulator
	static int aci = 0;//# of elments in sum
	sqSum += (V-ACBias)*(V-ACBias);
	aci++;
	if(aci >= 16){
		rmsAccum.addElement( sqSum/16.0 ); //moving window
		c0g::currentACVoltage = sqrt(rmsAccum.getAvg())*2;
		aci = 0;
		sqSum = 0;
	}
	
	
	c0g::lastVAdcRdg = c0g::lastADCReading;
	if(c0g::isAutoRangingV){
		//double absv = abs(c0g::currentVoltage);
		//filter = filter*0.85 + V*0.15;//simple complement filter - TODO upgrade
		double adc = c0g::lastADCReading;
		rmsFilter = sqrt((rmsFilter*0.9*rmsFilter) + (0.1*adc*adc));
		if(rmsFilter < 800){
			c0g::nextAutoVoltRange = voltRangeDown(c0g::currentVoltRange);
		}else if(rmsFilter > 20000){
			c0g::nextAutoVoltRange = voltRangeUp(c0g::currentVoltRange);
		}
	}
}

void c1::setIState(){
	//nothing to do here, current can't be controlled (only the switch)
}
void c0::setIMuxChannel(){
	switch(c0g::currentAmpRange){
		case ampRange_uA:
			pgaSetChannel(PGA_CHANNEL_UA);
			//c0g::ampRangeStr = &ampRangeNames[0][0];
		break;
		case ampRange_mA:
			pgaSetChannel(PGA_CHANNEL_MA);
			//c0g::ampRangeStr = &ampRangeNames[1][0];
		break;
		case ampRange_A:
			pgaSetChannel(PGA_CHANNEL_AMPS);
			//c0g::ampRangeStr = &ampRangeNames[2][0];
		break;
		default:
			pgaSetChannel(PGA_CHANNEL_UA);
			//c0g::ampRangeStr = &ampRangeNames[0][0];
		break;
	}
	//delayMicroseconds(20);
}

void c0::setIPGAGain(){
	switch(c0g::currentAmpRange){
	case ampRange_uA:
		pgaSetGain(PGA_GAIN_8);
	break;
	case ampRange_mA:
		pgaSetGain(PGA_GAIN_5);
	break;
	case ampRange_A:
		pgaSetGain(PGA_GAIN_8);
	break;
	default:
		pgaSetGain(PGA_GAIN_1);
	break;
	}
}
int32_t iadc  = 0;
void c0::applyICal(){
	int32_t adc = c0g::lastADCReading;
	c0g::lastIAdcRdg = c0g::lastADCReading;
	c0g::currentCurrent = ADCToCurrent(adc);
	iadc = adc;
}

void c0::setModPGAGain(){
	pgaSetGain(PGA_GAIN_1);
}
void c0::setMod0MuxChannel(){
	pgaSetChannel(PGA_CHANNEL_MOD0);
}
void c0::setMod1MuxChannel(){
	pgaSetChannel(PGA_CHANNEL_MOD1);
}
void c0::applyM0Cal(){
	int adc = c0g::lastADCReading;
	//c0g::lastM0AdcRdg = c0g::lastADCReading;
	c0g::currentM0 = getM0VoltageFromADC(adc);//((double)adc*0.01) + 0.99*c0g::currentM0;//(double)adc*69*2.4 / 32000;
}
void c0::applyM1Cal(){
	int adc = c0g::lastADCReading;
	//c0g::lastM1AdcRdg = c0g::lastADCReading;
	c0g::currentM1 = getM1CurrentFromADC(adc);//((double)adc*0.01) + 0.99*c0g::currentM1;//(double)adc*31*2.4 / 32000;
}

void c1::radioLoopFn(){
	nrfRadio.loopFn();
}

void c1::serialRxHandler(){
	static uint32_t lastRx = 0;
	if(Serial1.available()){
		if(Serial1.read() == 'H'){
			c1g::doRadioTx = false;
			lastRx = millis();
      if(isOnBattery == true){
        isOnBattery = false;
      }
      timeStOnBattery = lastRx;
		}
	}
	if(millis()-lastRx > 500){
		c1g::doRadioTx = true;
    if(isOnBattery == false){
      isOnBattery = true;
      timeStOnBattery = millis();
    }
	}
}

#define SERIAL_PRINT_PERIOD 100
#define RADIO_PRINT_PERIOD 150
#define STATE_PRINT_PERIOD 100
#define SLOW_PRINT_PERIOD 250

void c1::printState(){
	static uint32_t lastPrint = 0;
	static uint32_t lastRadio = 0;
	uint32_t mls = millis();
	if(mls-lastPrint > STATE_PRINT_PERIOD){
		lastPrint = mls;
		Serial.print("ST=");
		Serial.println(c1g::mainStateStr);
		Serial1.print("ST=");
		Serial1.println(c1g::mainStateStr);
	}
	static boolean radioIsMidPrint = false;
	if(mls-lastRadio > RADIO_PRINT_PERIOD && c1g::doRadioTx && nrfRadio.availableForWrite()){
		static char stateStrHolder[32];//copy here, if stateStr changes mid-print, finish printing anyway, before moving to the new one
		static int radioN = 0;
		static int nPacketsNeeded = 0;
		char buf[7];
		buf[0] = 'S';
		buf[1] = 'T';
		buf[6] = '\0';
		if(radioIsMidPrint){
			//if(nPacketsNeeded >= radioN){
				for(int i=0;i<3;i++){
					if(stateStrHolder[i] != '\0'){
						buf[i+3] = stateStrHolder[i+(3*radioN)];
					}else{
						buf[i+3] = '\0';
						break;
					}
				}
				buf[2] = '0'+radioN;
				Serial.printf("ST%d=<%s>of %d\r\n",radioN,buf,nPacketsNeeded);
				nrfRadio.write((uint8_t*)buf);
				radioN++;
				if(nPacketsNeeded == radioN){
					radioIsMidPrint = false;
					lastRadio = mls;
				}
			//}
		}else{
			radioIsMidPrint = true;
			for(int i=0;i<32;i++){
				if(c1g::mainStateStr[i] == '\0'){
					stateStrHolder[i] = '\0';
					nPacketsNeeded = (i/3) + ((i%3 == 0)?0:1);
					Serial.printf("StStrlng=%d\r\n",i);
					i = 100;
					break;
				}else{
					stateStrHolder[i] = c1g::mainStateStr[i];
				}
			}
			buf[2] = '0';
			for(int i=0;i<3;i++){
				if(stateStrHolder[i] != '\0'){
					buf[i+3] = stateStrHolder[i];
				}else{
					buf[i+3] = '\0';
					break;
				}
			}
			Serial.printf("NRF Printing State Str<%s> 0 of %d\r\n",buf,nPacketsNeeded);
			nrfRadio.write((uint8_t*)buf);
			radioN = 1;
			if(nPacketsNeeded == 1){
				radioIsMidPrint = false;
				lastRadio = mls;
			}
		}
	}
}

void c1::printV(){
	static uint32_t lastPrint = 0;
	c1g::doSendVoltageRange = true;
	uint32_t mls = millis();
	if(mls-lastPrint > SERIAL_PRINT_PERIOD){
		lastPrint = mls;
		Serial1.print("V=");
		Serial1.println(c1g::currentVoltage,6);
		Serial.print("V=");
		Serial.println(c1g::currentVoltage,6);
		if(c1g::doSendVoltageRange){
			Serial.print("VR=");
			Serial.println(c1g::voltRangeStr);
			Serial1.print("VR=");
			Serial1.println(c1g::voltRangeStr);
		}
	}
	static uint32_t lastRadio = 0;
	if(mls-lastRadio > RADIO_PRINT_PERIOD && c1g::doRadioTx && nrfRadio.availableForWrite()){
		uint8_t buf[8];
		lastRadio = mls;
		char nm[2] = {'V','\0'};
		nrfPackRadioPacket(nm, c1g::currentVoltage,buf);
		nrfRadio.write(buf);
	}
}

void c1::printVAC(){
	static uint32_t lastPrint = 0;
	c1g::doSendVoltageRange = true;
	uint32_t mls = millis();
	if(mls-lastPrint > SERIAL_PRINT_PERIOD){
		lastPrint = mls;
		Serial1.print("VA=");
		Serial1.println(c1g::currentACVoltage,6);
		Serial.print("VA=");
		Serial.println(c1g::currentACVoltage,6);
		if(c1g::doSendVoltageRange && !c1g::doSendDisplayVoltage){
			Serial.print("VR=");
			Serial.println(c1g::voltRangeStr);
			Serial1.print("VR=");
			Serial1.println(c1g::voltRangeStr);
		}
	}
	static uint32_t lastRadio = 10;
	if(mls-lastRadio > RADIO_PRINT_PERIOD && c1g::doRadioTx  && nrfRadio.availableForWrite()){
		uint8_t buf[8];
		lastRadio = mls;
		char nm[2] = {'V','A'};
		nrfPackRadioPacket(nm, c1g::currentACVoltage,buf);
		nrfRadio.write(buf);
	}
}

void c1::printR(){
	static uint32_t lastPrint = 0;
	static double slow = 0;
	static double avg=0;
	uint32_t mls = millis();
	c1g::doSendOhmRange = true;

	if(mls-lastPrint > SERIAL_PRINT_PERIOD){
		lastPrint = mls;
		char strOut[24];
		double effR = c1g::currentResistance;
		if(effR >= (double)4000000.0 || effR < (double)-5.0){
			effR = 9999999;
		}
		sprintf(strOut,"R=%0.2f\r\n\0",effR,2);
		int len = strlen(strOut);
		int afw = Serial1.availableForWrite();
		if(afw){
			uint32_t str = micros();
			Serial1.print(strOut);
			uint32_t nd = micros();
			Serial.printf("DST=%d\r\n",nd-str);
		}else{
			Serial.printf("afw=%d\r\n",afw);
		}
		Serial.print("R=");
		Serial.println(effR,2);
		Serial.print("RI=");
		Serial.println(c1g::currentOhmCur*1000,5);
		if(true){//c1g::doSendOhmRange){
			Serial.print("RR=");
			Serial.println(c1g::ohmRangeStr);
			Serial1.print("RR=");
			Serial1.println(c1g::ohmRangeStr);
		}
	}
	static uint32_t lastRadio = 20;
	if(mls-lastRadio > RADIO_PRINT_PERIOD  && c1g::doRadioTx  && nrfRadio.availableForWrite()){
		uint8_t buf[8];
		lastRadio = mls;
		char nm[2] = {'R','\0'};
		nrfPackRadioPacket(nm, c1g::currentResistance,buf);
		nrfRadio.write(buf);
	}
	double div = c1g::ohmRangeSmoothingDiv;
	slow = (slow*(1-div)) + (avg*div);
}
void c1::printRI(){
	static uint32_t lastPrint = 0;
	if(millis()-lastPrint > SERIAL_PRINT_PERIOD){
		lastPrint = millis();
		double avg = 0;
		const int nm = 8;
		static double run[nm];
		static int ind = 0;
		run[ind] = c1g::currentOhmCur;
		for(int i=0;i<nm;i++){
			avg = avg + run[i];
		}
		ind = (ind + 1)%nm;
		avg = avg/nm;
		static double slow = avg;
		slow = slow*0.75 + avg*0.25;
		Serial1.print("IT=");
		Serial1.println(slow,8);
		Serial.print("IT=");
		Serial.println(slow*1000,5);
	}
}
void c1::printI(){
	static uint32_t lastPrint = 0;
	uint32_t mls = millis();
	if(mls-lastPrint > SERIAL_PRINT_PERIOD){
		lastPrint = mls;
		Serial1.print("I=");
		Serial1.println(c1g::currentCurrent,8);
		Serial.print("I=");
		Serial.println(c1g::currentCurrent,8);
    if(true){//range
      Serial.print("IR=");
			Serial.println(c1g::ampRangeStr);
			Serial1.print("IR=");
			Serial1.println(c1g::ampRangeStr);
    }
		//Serial.print("ia=");
		//Serial.println(iadc);
	}
	static uint32_t lastRadio = 30;
	if(mls-lastRadio > RADIO_PRINT_PERIOD  && c1g::doRadioTx  && nrfRadio.availableForWrite()){
		uint8_t buf[8];
		lastRadio = mls;
		char nm[2] = {'I','\0'};
		nrfPackRadioPacket(nm, c1g::currentCurrent,buf);
		nrfRadio.write(buf);
	}
}

void c1::printMod(){
	static uint32_t lastPrint = 0;
	uint32_t mls = millis();
	if(mls-lastPrint > SERIAL_PRINT_PERIOD){
		lastPrint = mls;
		if(c1g::doSendDisplayModVoltage){
			Serial1.print("M0=");
			Serial1.println(c1g::currentM0,6);
			Serial.print("M0=");
			Serial.println(c1g::currentM0,6);
		}
		if(c1g::doSendDisplayModCurrent){
			Serial1.print("M1=");
			Serial1.println(c1g::currentM1,6);
			Serial.print("M1=");
			Serial.println(c1g::currentM1,6);
		}
		if(c1g::doSendDisplayModPower){
			Serial1.print("MP=");
			Serial1.println(c1g::currentM1 * c1g::currentM0,6);
			Serial.print("MP=");
			Serial.println(c1g::currentM1 * c1g::currentM0,6);
		}
		if(c1g::doSendDisplayModResistance){
			Serial1.print("MR=");
			Serial1.println(c1g::currentM0 / c1g::currentM1,6);
			Serial.print("MR=");
			Serial.println(c1g::currentM0  / c1g::currentM1,6);
		}
	}
	static uint32_t lastRadio = 40;
	static boolean trigm0 = false;
	static boolean trigm1 = false;
	static boolean trigmp = false;
	static boolean trigmr = false;
	if((mls-lastRadio) > RADIO_PRINT_PERIOD  && c1g::doRadioTx){
		lastRadio = mls;
		trigm0 = true;
		trigm1 = true;
		trigmp = true;
		trigmr = true;
	}
	uint8_t flags = 0;
	if(c1g::doRadioTx && trigm0  && nrfRadio.availableForWrite()){
		if(c1g::doSendDisplayModVoltage){
			uint8_t buf[8];
			char nm[2] = {'M','0'};
			nrfPackRadioPacket(nm, c1g::currentM0,buf);
			nrfRadio.write(buf);
		}
		trigm0 = false;
	}else if(c1g::doRadioTx && trigm1  && nrfRadio.availableForWrite()){
		if(c1g::doSendDisplayModCurrent){ 
			uint8_t buf[8];
			char nm[2] = {'M','1'};
			nrfPackRadioPacket(nm, c1g::currentM1,buf);
			nrfRadio.write(buf);
		}
		trigm1 = false;
	}else if(c1g::doRadioTx && trigmp  && nrfRadio.availableForWrite()){
		if(c1g::doSendDisplayModPower){ 
			uint8_t buf[8];
			char nm[2] = {'M','P'};
			nrfPackRadioPacket(nm, c1g::currentM1 * c1g::currentM0 ,buf);
			nrfRadio.write(buf);
		}
		trigmp = false;
	}else if(c1g::doRadioTx && trigmr  && nrfRadio.availableForWrite(&flags)){
		if(c1g::doSendDisplayModResistance){ 
			uint8_t buf[8];
			char nm[2] = {'M','R'};
			Serial.print("Flags:");Serial.println(flags,HEX);
			nrfPackRadioPacket(nm, c1g::currentM0 / c1g::currentM1 ,buf);
			nrfRadio.write(buf);
		}
		trigmr = false;
	}
}