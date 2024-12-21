#include "meterState.h"


char ohmRangeNames[9][16] = {
	"0:0-330R\0",
	"1:330-15K\0",
	"2:15K-150K\0",
	"3:150K+\0",
	"510R_HV\0",
	"10KR_HV\0",
	"100K_HV\0",
	"1MR_HV\0",
	"Disabled\0"
};

char ampRangeNames[4][16] = {
	"uA\0",
	"mA\0",
	"A\0",
	"Disabled\0"
};

char voltRangeNames[4][16] = {
	"1V5\0",
	"15V\0",
	"200V\0",
	"Disabled\0"
};
