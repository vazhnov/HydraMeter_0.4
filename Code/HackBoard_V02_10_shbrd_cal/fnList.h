#ifndef FUNCTION_LIST_H
#define FUNCTION_LIST_H

#include "debug.h"
#include <Arduino.h>

#define MAIN_FN_LIST_SIZE 64 //max # of functions in the main operation-selection loop
#define SEC_FN_LIST_SIZE 32 //max # of functions in the main operation-selection loop

extern int32_t nullFnCalls;
//extern boolean alwaysFalse;
//extern boolean alwaysTrue;
void nullFn();
typedef struct fnList_type{
	void (*fn)() = &nullFn;
	volatile bool* run = NULL;
	char name[6] = {'n','u','l','l','\0','\0'};
	//bool* ovrd;
}fnList_t;

int fnListAdd(fnList_t in);
int secFnListAdd(fnList_t in);

extern fnList_t mainFnList[MAIN_FN_LIST_SIZE];
extern int32_t mainFnListLength;
extern fnList_t secFnList[SEC_FN_LIST_SIZE];
extern int32_t secFnListLength;

#endif