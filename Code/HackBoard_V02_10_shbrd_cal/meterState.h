#ifndef METERSTATE_H
#define METERSTATE_H

#include "genVar.h"

typedef struct meterPart{
	char name[12];
	genVar::type output;
	genVar::type input;
}meterPart;





typedef enum ampRange_t{
	ampRange_uA,
	ampRange_mA,
	ampRange_A,
	ampRange_disabled
}ampRange_t;

extern char ampRangeNames[4][16];

typedef enum ohmRange_t{
	ohmRange_0,
	ohmRange_1,
	ohmRange_2,
	ohmRange_3,
	ohmRange_0_highV,
	ohmRange_1_highV,
	ohmRange_2_highV,
	ohmRange_3_highV,
	ohmRange_disabled
}ohmRange_t;

extern char ohmRangeNames[9][16];


typedef enum voltRange_t{
	voltRange_0,
	voltRange_1,
	voltRange_2,
	voltRange_disabled
}voltRange_t;

extern char voltRangeNames[4][16];

#endif