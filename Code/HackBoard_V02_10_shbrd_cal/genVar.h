#ifndef GENVAR_H
#define GENVAR_H
#include <Arduino.h>
#include "debug.h"


namespace genVar{
	typedef enum type{
		INT8,UINT8,INT16,UINT16,INT32,UINT32,
		INT64,UINT64,FLOAT,DOUBLE,STRPTR,STRING,CHAR,
		INT16PTR,UINT16PTR,INT32PTR,UINT32PTR,
		FLOATPTR,DOUBLEPTR,
		BOOLEAN,
		NONE
	}type;
}
typedef struct genVar_t{
	union{
		int8_t    int8;
		uint8_t   uint8;
		int16_t   int16;
		uint16_t  uint16;
		int32_t   int32;
		uint32_t  uint32;
		int64_t   int64;
		uint64_t  uint64;
		float     flt;
		double    dbl;
		char*     strptr;
		char      str[8];
		char      chr;
		int16_t*  int16Ptr;
		uint16_t* uint16Ptr;
		int32_t*  int32Ptr;
		uint32_t* uint32Ptr;
		float*    fltPtr;
		double*   dblPtr;
	}val;
	//val v;
	genVar::type t;
	uint16_t lng;
}genVar_t;

namespace conversion{
	typedef enum type{
		NONE,
		PASS,
		ADD, // in + B
		SUBTRACT, // in - B
		MULTIPLY, //in * A
		DIVIDE,   //in / A
		MAC,      //(in*A) + B
		SQRT,     //sqrt(in*A)+B
		EXP,      //e^(in*A)+B
		LOG,      //log(in*A+B)
		CAL       //pointer to an pair of arrays, mapping from A to B
		          //so if A is [0 1 3], B is [0.1, 0.2, 0.3], then 2->2.5
	}type;
}
typedef struct conversion_t {
	genVar_t A; //defaults to 1
	genVar_t B; //defaults to 0
	conversion::type t;
	genVar::type outType;
}conversion_t;

typedef struct globalVar_t{
	genVar_t var;
	uint8_t lastUpdateCore;
	uint64_t timestamp;
}globalVar_t;

typedef struct userVar_t{
	genVar_t gv;
	conversion_t conv;
	uint64_t lastUpdate;
}userVar_t;

void genVarOp(genVar_t* res, genVar_t* a, genVar_t* b,conversion::type t);
boolean genVarIsFloat(genVar_t* var);
boolean genVarIsFixed(genVar_t* var);
void genVarAssignInt(genVar_t* var,int64_t in);
void genVarAssignFloat(genVar_t* var,double in);
genVar_t convert(genVar_t* in,conversion_t* conv);
void genVarRegArray(genVar_t* var,genVar::type typ, void* array);
void createUserVarCal(userVar_t* var,genVar::type inType, genVar::type outType, void* inAr, void* outAr,int inLng, int outLng);
void updateUserVar(userVar_t* var, genVar_t in);
#endif