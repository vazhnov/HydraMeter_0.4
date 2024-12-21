
namespace c0{
	double ADCToCurrent(int32_t adcCnt);
	double ADCToVoltage(int32_t adcCnt);
	double ohmsFbToAmps(int32_t adcCnt);
	double getM0VoltageFromADC(int32_t adcCnt);
	double getM1CurrentFromADC(int32_t adcCnt);
};

namespace c1{
	void setOhmsRange(ohmRange_t rng);
	void setAtten(voltRange_t rng);
	void setRange(voltRange_t in);
};
double linint(double from, double lowerFrom, double upperFrom,double lowerTo, double upperTo);

//ohmRange_t ohmsRange = ohmRange_disabled;
void c1::setOhmsRange(ohmRange_t rng){
	c1g::currentOhmRange = rng;
	double rDivOhms = 99999999999999;
	switch(rng){
	case ohmRange_0:
		digitalWriteSR(OHMS_510_SRPIN,HIGH);
		digitalWriteSR(OHMS_10K_SRPIN,LOW);
		digitalWriteSR(OHMS_100K_SRPIN,LOW);
		digitalWriteSR(OHMS_1M_SRPIN,LOW);
		digitalWriteSR(OHM_PATH_DIS_SRPIN,0);
		digitalWriteSR(OHM_BOOST_EN_SRPIN,1);
		digitalWriteSR(OHMS_LOWV_EN_SRPIN,1);
		digitalWriteSR(OHMS_ELOWV_EN_SRPIN,1);
		rDivOhms = 510.00001;
		c1g::ohmRangeSmoothingDiv = 0.1;
		c1g::ohmRangeStr = &ohmRangeNames[0][0];
	break;
	case ohmRange_1:
		digitalWriteSR(OHMS_510_SRPIN,LOW);
		digitalWriteSR(OHMS_10K_SRPIN,HIGH);
		digitalWriteSR(OHMS_100K_SRPIN,LOW);
		digitalWriteSR(OHMS_1M_SRPIN,LOW);
		digitalWriteSR(OHM_PATH_DIS_SRPIN,0);
		digitalWriteSR(OHM_BOOST_EN_SRPIN,1);
		digitalWriteSR(OHMS_LOWV_EN_SRPIN,1);
		digitalWriteSR(OHMS_ELOWV_EN_SRPIN,1);
		rDivOhms = 10000.0001;
		c1g::ohmRangeSmoothingDiv = 0.1;
		c1g::ohmRangeStr = &ohmRangeNames[1][0];
	break;
	case ohmRange_2:
		digitalWriteSR(OHMS_510_SRPIN,LOW);
		digitalWriteSR(OHMS_10K_SRPIN,LOW);
		digitalWriteSR(OHMS_100K_SRPIN,HIGH);
		digitalWriteSR(OHMS_1M_SRPIN,LOW);
		digitalWriteSR(OHM_PATH_DIS_SRPIN,0);
		digitalWriteSR(OHM_BOOST_EN_SRPIN,1);
		digitalWriteSR(OHMS_LOWV_EN_SRPIN,1);
		digitalWriteSR(OHMS_ELOWV_EN_SRPIN,1);
		rDivOhms = 100000.001;
		c1g::ohmRangeSmoothingDiv = 0.03;
		c1g::ohmRangeStr = &ohmRangeNames[2][0];
	break;
	case ohmRange_3:
		digitalWriteSR(OHMS_510_SRPIN,LOW);
		digitalWriteSR(OHMS_10K_SRPIN,LOW);
		digitalWriteSR(OHMS_100K_SRPIN,LOW);
		digitalWriteSR(OHMS_1M_SRPIN,HIGH);
		digitalWriteSR(OHM_PATH_DIS_SRPIN,0);
		digitalWriteSR(OHM_BOOST_EN_SRPIN,1);
		digitalWriteSR(OHMS_LOWV_EN_SRPIN,1);
		digitalWriteSR(OHMS_ELOWV_EN_SRPIN,1);
		rDivOhms = 1000000.01;
		c1g::ohmRangeSmoothingDiv = 0.01;
		c1g::ohmRangeStr = &ohmRangeNames[3][0];
	break;
	case ohmRange_0_highV:
		digitalWriteSR(OHMS_510_SRPIN,HIGH);
		digitalWriteSR(OHMS_10K_SRPIN,LOW);
		digitalWriteSR(OHMS_100K_SRPIN,LOW);
		digitalWriteSR(OHMS_1M_SRPIN,LOW);
		digitalWriteSR(OHM_PATH_DIS_SRPIN,0);
		digitalWriteSR(OHM_BOOST_EN_SRPIN,1);
		digitalWriteSR(OHMS_LOWV_EN_SRPIN,0);
		digitalWriteSR(OHMS_ELOWV_EN_SRPIN,0);
		rDivOhms = 510.0;
		c1g::ohmRangeSmoothingDiv = 0.2;
		c1g::ohmRangeStr = &ohmRangeNames[4][0];
	break;
	case ohmRange_1_highV:
		digitalWriteSR(OHMS_510_SRPIN,LOW);
		digitalWriteSR(OHMS_10K_SRPIN,HIGH);
		digitalWriteSR(OHMS_100K_SRPIN,LOW);
		digitalWriteSR(OHMS_1M_SRPIN,LOW);
		digitalWriteSR(OHM_PATH_DIS_SRPIN,0);
		digitalWriteSR(OHM_BOOST_EN_SRPIN,1);
		digitalWriteSR(OHMS_LOWV_EN_SRPIN,0);
		digitalWriteSR(OHMS_ELOWV_EN_SRPIN,0);
		rDivOhms = 10000.0;
		c1g::ohmRangeSmoothingDiv = 0.1;
		c1g::ohmRangeStr = &ohmRangeNames[5][0];
	break;
	case ohmRange_2_highV:
		digitalWriteSR(OHMS_510_SRPIN,LOW);
		digitalWriteSR(OHMS_10K_SRPIN,LOW);
		digitalWriteSR(OHMS_100K_SRPIN,HIGH);
		digitalWriteSR(OHMS_1M_SRPIN,LOW);
		digitalWriteSR(OHM_PATH_DIS_SRPIN,0);
		digitalWriteSR(OHM_BOOST_EN_SRPIN,1);
		digitalWriteSR(OHMS_LOWV_EN_SRPIN,0);
		digitalWriteSR(OHMS_ELOWV_EN_SRPIN,0);
		rDivOhms = 100000.0;
		c1g::ohmRangeSmoothingDiv = 0.03;
		c1g::ohmRangeStr = &ohmRangeNames[6][0];
	break;
	case ohmRange_3_highV:
		digitalWriteSR(OHMS_510_SRPIN,LOW);
		digitalWriteSR(OHMS_10K_SRPIN,LOW);
		digitalWriteSR(OHMS_100K_SRPIN,LOW);
		digitalWriteSR(OHMS_1M_SRPIN,HIGH);
		digitalWriteSR(OHM_PATH_DIS_SRPIN,0);
		digitalWriteSR(OHM_BOOST_EN_SRPIN,1);
		digitalWriteSR(OHMS_LOWV_EN_SRPIN,0);
		digitalWriteSR(OHMS_ELOWV_EN_SRPIN,0);
		rDivOhms = 1000000.0;
		c1g::ohmRangeSmoothingDiv = 0.01;
		c1g::ohmRangeStr = &ohmRangeNames[7][0];
	break;
	case ohmRange_disabled:
	default:
		digitalWriteSR(OHMS_510_SRPIN,LOW);
		digitalWriteSR(OHMS_10K_SRPIN,LOW);
		digitalWriteSR(OHMS_100K_SRPIN,LOW);
		digitalWriteSR(OHMS_1M_SRPIN,LOW);
		digitalWriteSR(OHM_PATH_DIS_SRPIN,0);
		digitalWriteSR(OHM_BOOST_EN_SRPIN,0);
		digitalWriteSR(OHMS_LOWV_EN_SRPIN,0);
		digitalWriteSR(OHMS_ELOWV_EN_SRPIN,0);
		rDivOhms = 999999999999.9;
		c1g::ohmRangeSmoothingDiv = 0.5;
		c1g::ohmRangeStr = &ohmRangeNames[8][0];
	break;
	}
}
//double rMeter = 0;

//this is a little dumb because of the way it was before, think about edge cases and redo at some point
void c1::setAtten(voltRange_t rng){
	double Rgnd;
	switch(rng){
	case voltRange_2:
		Rgnd = 6800;
		c1g::vRange = voltRange_2;
	break;
	case voltRange_1:
		Rgnd = 100000;
		c1g::vRange = voltRange_1;
	break;
	case voltRange_0:
	default:
		Rgnd = 1e15;
		c1g::vRange = voltRange_0;
	break;
	}
	double lower = ((double)1) / ( (((double)1) / 9100000 ) + (((double)1) / Rgnd ) );
	double inputR = 1000000;
	c1g::rMeter = lower + inputR;
}

void c1::setRange(voltRange_t in){
	switch(in){
	case voltRange_0://no atten
		digitalWriteSR(ATTEN_11_SRPIN ,0);
		digitalWriteSR(ATTEN_147_SRPIN ,0);
		c1g::voltRangeStr = &voltRangeNames[0][0];
		writeSRPins();
	break;
	case voltRange_1:
		digitalWriteSR(ATTEN_11_SRPIN ,1);
		digitalWriteSR(ATTEN_147_SRPIN ,0);
		c1g::voltRangeStr = &voltRangeNames[1][0];
		writeSRPins();
	break;
	case voltRange_2:
		digitalWriteSR(ATTEN_11_SRPIN ,0);
		digitalWriteSR(ATTEN_147_SRPIN ,1);
		c1g::voltRangeStr = &voltRangeNames[2][0];
		writeSRPins();
	break;
	}
	setAtten(in);
}
double linint(double from, double lowerFrom, double upperFrom,double lowerTo, double upperTo){
	double dif = (from-lowerFrom) / (upperFrom-lowerFrom);
	return (dif * (upperTo-lowerTo)) + lowerTo;
}

double c0::ADCToCurrent(int32_t adcCnt){
	int32_t* adc;
	double* amps;
	int size = -1;
	switch(c0g::currentAmpRange){
	case ampRange_uA:
		adc = &ampsCalADC_uA[0];
		amps = &ampsCalOut_uA[0];
		size = AMP_CAL_UA_SIZE;
	break;
	case ampRange_mA:
		adc = &ampsCalADC_mA[0];
		amps = &ampsCalOut_mA[0];
		size = AMP_CAL_MA_SIZE;
	break;
	case ampRange_A:
		adc = &ampsCalADC_A[0];
		amps = &ampsCalOut_A[0];
		size = AMP_CAL_A_SIZE;
	break;
	default:
		return 0.2451;
	break;
	}
	if(size >= 2){
		if(adcCnt <= adc[0]){
			return linint(adcCnt,adc[0],adc[1],amps[0],amps[1]);
		}else if(adcCnt >= adc[size-1]){
			return linint(adcCnt,adc[size-2],adc[size-1],amps[size-2],amps[size-1]);
		}else{
			int index;
			for(index=1;index<size-1;index++){
				if(adcCnt <= adc[index]){
					break;
				}
			}
			//Serial.println("ADC:");Serial.println(adcCnt);
			//Serial.println("adcl:");Serial.print(adc[index-1]);
			//Serial.println("\tadch:");Serial.println(adc[index]);
			//Serial.println("ampl:");Serial.println(adcCnt);
			//Serial.println("amph:");Serial.println(adcCnt);
			return linint(adcCnt,adc[index-1],adc[index],amps[index-1],amps[index]);
		}
	}else{
		Serial.println("Can't use cal, not enough elements");
		return 0;
	}
	return 0;
}
/*
	double volts = ((double)adcCnt)*((double)2.4)/((double)(1<<15));
	switch(ampRange){
	case ampRange_uA:
		return volts / 100.34;
	break;
	case ampRange_mA:
		return volts / 0.33;
	break;
	case ampRange_A:
		return volts / 0.01;
	break;
	case ampRange_disabled:
		return -0.2451;
	break;
	}
	return 0;
}*/

double c0::ADCToVoltage(int32_t adcCnt){
	int32_t* adc;
	double* volts;
	int size = -1;
	switch(c0g::vRange){
	case voltRange_0:
		adc = &voltageCalADC_RNG1[0];
		volts = &voltageCalOut_RNG1[0];
		size = RNG1_CAL_AR_SIZE;
	break;
	case voltRange_1:
		adc = &voltageCalADC_RNG2[0];
		volts = &voltageCalOut_RNG2[0];
		size = RNG2_CAL_AR_SIZE;
	break;
	case voltRange_2:
		adc = &voltageCalADC_RNG3[0];
		volts = &voltageCalOut_RNG3[0];
		size = RNG3_CAL_AR_SIZE;
	break;
	default:
		return 0;
	break;
	}
	if(size >= 2){
		if(adcCnt <= adc[0]){
			//Serial.println("<min");
			return linint(adcCnt,adc[0],adc[1],volts[0],volts[1]);
		}else if(adcCnt >= adc[size-1]){
			//Serial.println(">max");
			return linint(adcCnt,adc[size-2],adc[size-1],volts[size-2],volts[size-1]);
		}else{
			int index;
			for(index=1;index<size-1;index++){
				if(adcCnt <= adc[index]){
					break;
				}
			}
			//Serial.print(adc[index-1]);Serial.print("\t-\t");
			//Serial.print(adc[index]);Serial.print("\t-->");
			//Serial.print(volts[index-1],2);Serial.print("\t-");
			//Serial.println(volts[index],2);
			return linint(adcCnt,adc[index-1],adc[index],volts[index-1],volts[index]);
		}
	}else{
		Serial.println("Can't use cal, not enough elements");
		return 0;
	}
	return 0;
}

double c0::ohmsFbToAmps(int32_t adcCnt){
	int32_t* adc;
	double* amps;
	int size = -1;
	switch(c0g::currentOhmRange){
	case ohmRange_0_highV:
	case ohmRange_0:
		adc = ohmsCalADC_RNG0;
		amps = &ohmsCalOut_RNG0[0];
		size = OHMS_CAL_I_0_SIZE;
	break;
	case ohmRange_1_highV:
	case ohmRange_1:
		adc = ohmsCalADC_RNG1;
		amps = &ohmsCalOut_RNG1[0];
		size = OHMS_CAL_I_1_SIZE;
	break;
	case ohmRange_2_highV:
	case ohmRange_2:
		adc = ohmsCalADC_RNG2;
		amps = &ohmsCalOut_RNG2[0];
		size = OHMS_CAL_I_2_SIZE;
	break;
	case ohmRange_3_highV:
	case ohmRange_3:
		adc = ohmsCalADC_RNG3;
		amps = &ohmsCalOut_RNG3[0];
		size = OHMS_CAL_I_3_SIZE;
	break;
	default:
		return 0;
	break;
	}
	if(size >= 2){
		if(adcCnt <= adc[0]){
			return linint(adcCnt,adc[0],adc[1],amps[0],amps[1]);
		}else if(adcCnt >= adc[size-1]){
			double out = linint(adcCnt,adc[size-2],adc[size-1],amps[size-2],amps[size-1]);
			//Serial.print("\tout:");Serial.println(out);
			return out;
		}else{
			int index;
			for(index=1;index<size-1;index++){
				if(adcCnt <= adc[index]){
					break;
				}
			}
			double out = linint(adcCnt,adc[index-1],adc[index],amps[index-1],amps[index]);
			//Serial.print("\tout:");Serial.println(out);
			return out;
		}
	}else{
		return 0;
	}
	return 0;
}

double c0::getM1CurrentFromADC(int32_t adcCnt){
	int32_t* adc;
	double* volts;
	int size = -1;
	adc = M1CalADC;
	volts = M1CalOut;
	size = M1_CAL_AR_SIZE;
	if(size >= 2){
		if(adcCnt <= adc[0]){
			return linint(adcCnt,adc[0],adc[1],volts[0],volts[1]);
		}else if(adcCnt >= adc[size-1]){
			double out = linint(adcCnt,adc[size-2],adc[size-1],volts[size-2],volts[size-1]);
			return out;
		}else{
			int index;
			for(index=1;index<size-1;index++){
				if(adcCnt <= adc[index]){
					break;
				}
			}
			double out = linint(adcCnt,adc[index-1],adc[index],volts[index-1],volts[index]);
			return out;
		}
	}else{
		return 0;
	}
	return 0;
}

double c0::getM0VoltageFromADC(int32_t adcCnt){
	int32_t* adc;
	double* volts;
	int size = -1;
	adc = M0CalADC;
	volts = M0CalOut;
	size = M0_CAL_AR_SIZE;
	if(size >= 2){
		if(adcCnt <= adc[0]){
			return linint(adcCnt,adc[0],adc[1],volts[0],volts[1]);
		}else if(adcCnt >= adc[size-1]){
			double out = linint(adcCnt,adc[size-2],adc[size-1],volts[size-2],volts[size-1]);
			return out;
		}else{
			int index;
			for(index=1;index<size-1;index++){
				if(adcCnt <= adc[index]){
					break;
				}
			}
			double out = linint(adcCnt,adc[index-1],adc[index],volts[index-1],volts[index]);
			return out;
		}
	}else{
		return 0;
	}
	return 0;
}