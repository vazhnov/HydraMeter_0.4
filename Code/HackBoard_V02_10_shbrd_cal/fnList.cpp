#define DEBUG_2
#include "fnList.h"

fnList_t mainFnList[MAIN_FN_LIST_SIZE];
fnList_t secFnList[SEC_FN_LIST_SIZE];

int32_t nullFnCalls = 0;
int32_t mainFnListLength = 0; //# of calls in list
int32_t secFnListLength = 0; //secondary fn list, runs in coprocessor 

void nullFn(){
	nullFnCalls++;
}

int fnListAdd(fnList_t in){
	if(mainFnListLength >= MAIN_FN_LIST_SIZE){
		d2print2L("Tried to reg fn but list too long: ",mainFnListLength);
		return -1;
	}
	if(mainFnListLength < 0){
		d2print2L("Tried to reg fn but negative list lng?: ",mainFnListLength);
		return -1;
	}
	mainFnList[mainFnListLength] = in;
	int32_t ret = mainFnListLength;
	mainFnListLength++;
	Serial.println("Added ");Serial.println(mainFnList[ret].name);
	Serial.print("to list [");Serial.print(ret);Serial.println("]");
	return ret;
}

int secFnListAdd(fnList_t in){
	if(secFnListLength >= SEC_FN_LIST_SIZE){
		d2print2L("tried to reg fn but 2nd list too long:",secFnListLength);
	}
	if(secFnListLength < 0){
		d2print2L("Tried to reg fn but negative list lng?: ",secFnListLength);
		return -1;
	}
	secFnList[secFnListLength] = in;
	int32_t ret = secFnListLength;
	secFnListLength++;
	Serial.println("Added ");Serial.println(secFnList[ret].name);
	Serial.print("to 2nd list [");Serial.print(ret);Serial.println("]");
	return ret;
}
