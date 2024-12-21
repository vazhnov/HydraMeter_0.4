#ifndef CALIBRATION_H
#define CALIBRATION_H

//rMeter: 147: 1.011425 Mohm
//11:1 : 1.10357 MOhm
//1:1 : 

#define AMP_CAL_UA_SIZE 15 //gain of 8
int32_t ampsCalADC_uA[] = {
	-15860,
	-12688,
	-5292,
	-1597,
	-275,
	-62,
	-32,
	-10,
	12,
	42,
	253,
	1574,
	5267,
	12669,
	15839
};
double ampsCalOut_uA[] = {
	(-1418.48/ 1000000.0),
	(-1134.54/ 1000000.0),
	(-472.23 / 1000000.0),
	(-141.73 / 1000000.0),
	(-23.51 / 1000000.0),
	(-4.595 / 1000000.0),
	(-1.77  / 1000000.0),
	(0.0    / 1000000.0),
	(1.77   / 1000000.0),
	(4.595  / 1000000.0),
	(23.508 / 1000000.0),
	(141.73 / 1000000.0),
	(472.25 / 1000000.0),
	(1134.55/ 1000000.0),
	(1418.46/ 1000000.0)
};

#define AMP_CAL_MA_SIZE 7 //gain of 5
int32_t ampsCalADC_mA[] = {
	-12112,
	-2433,
	-742,
	-8,
	726,
	2417,
	12088
};
double ampsCalOut_mA[] = {
	(-501.06 / 1000.0),
	(-100.41 / 1000.0),
	(-30.397 / 1000.0),
	(0.00    / 1000.0),
	(30.4    / 1000.0),
	(100.44  / 1000.0),
	(501.12  / 1000.0)
};

#define AMP_CAL_A_SIZE 7 //gain of 8
/*int32_t ampsCalADC_A[] = {
	-5475,
	-2740,
	-98,
	-8,
	 83,
	 2715,
	 5432
};
double ampsCalOut_A[] = {
	-6.0054,
	-3.0033,
	-0.10042,
	 0
	 0.10046,
	 3.0035,
	 6.006
};*/
int32_t ampsCalADC_A[] = {
	-3377,
	-1132,
	-132,
	-8,
	 115,
	 1111,
	 3347
};
double ampsCalOut_A[] = {
	-2.998,
	-0.9988,
	-0.1098,
	 0,
	 0.1096,
	 0.9988,
	 2.998
};
	
#define OHMS_CAL_I_0_SIZE 4
int32_t ohmsCalADC_RNG0[] = { //510 ohm -> current
	6,
  872,
	1443,
  1518
};
double ohmsCalOut_RNG0[] = { //amps
	(0.0     / 1000.0),
  (409.88  / 1000.0),
	(678.3   / 1000.0),
  (714.0    / 1000.0)
};

#define OHMS_CAL_I_1_SIZE 3
int32_t ohmsCalADC_RNG1[] = { //10K ohm -> current
	14,
	6549,
	12661
};
double ohmsCalOut_RNG1[] = { //amps
	(0.0     / 1000.0),
	(155.51  / 1000.0),
	(299.91  / 1000.0)
};

#define OHMS_CAL_I_2_SIZE 3
int32_t ohmsCalADC_RNG2[] = { //100K ohm -> current
	82,
	5307,
	15917
};
double ohmsCalOut_RNG2[] = { //amps
	(0.0 / 1000.0),
	(12.46   / 1000.0),
	(37.60   / 1000.0)
};
#define OHMS_CAL_I_3_SIZE 3
int32_t ohmsCalADC_RNG3[] = { //1M ohm -> current
	  763,
	 6204,
	 18277
};
double ohmsCalOut_RNG3[] = { //amps
	(0.0     / 1000.0),
	(1.33    / 1000.0),
	(4.20    / 1000.0)
};

#define RNG3_CAL_AR_SIZE 3
int32_t voltageCalADC_RNG3[] = {
	-2894,
	 4,
   2895
};
double voltageCalOut_RNG3[] = {
	-30.989,
	  0.0,
	 30.989
};
#define RNG2_CAL_AR_SIZE 5
int32_t voltageCalADC_RNG2[] = {
  -19937,
	-8722,
	 3,
   8717,
	 19932
};
double voltageCalOut_RNG2[] = {
  -16.000,
	 -7.000,
    0.000,
    7.000,
	 16.000
};
#define RNG1_CAL_AR_SIZE 5
int32_t voltageCalADC_RNG1[] = {
	-20051,
   -7511,
    9,
    7526,
   20060,
};
double voltageCalOut_RNG1[] = {
	-1.6015,
  -0.6007,
   0.0000,
   0.6007,
	 1.6015
};
#define M0_CAL_AR_SIZE 5
int32_t M0CalADC[] = {
	-4708,
	-906,
	14,
	941,
	4746
};
double M0CalOut[] = {
	-23.291,
	-4.5014,
	0,
	4.5105,
	23.29
};
#define M1_CAL_AR_SIZE 5
int32_t M1CalADC[]{
	-2718,
	-1713,
	-32,
	1720,
	2950
};
double M1CalOut[] = {
	-5.0004,
	-2.9994,
	0,
	2.998,
	4.998
};

#endif