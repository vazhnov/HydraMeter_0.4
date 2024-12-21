#include "genVar.h"
#include "util.h"



void genVarOp(genVar_t* res, genVar_t* a, genVar_t* b,conversion::type t){
	int64_t intrA;
	double fltA;
	boolean isAFixed = false;
	boolean isAFloat = false;
	
	switch(a->t){
	case genVar::INT8:  intrA = a->val.int8;     isAFixed = true; break;
	case genVar::INT16: intrA = a->val.int16;    isAFixed = true; break;
	case genVar::INT32: intrA = a->val.int32;    isAFixed = true; break;
	case genVar::INT64: intrA = a->val.int64;    isAFixed = true; break;
	case genVar::UINT8:  intrA = a->val.uint8;    isAFixed = true; break;
	case genVar::UINT16: intrA = a->val.uint16;  isAFixed = true; break;
	case genVar::UINT32: intrA = a->val.uint32;  isAFixed = true; break;
	case genVar::UINT64: intrA = a->val.uint64;  isAFixed = true; break;

	case genVar::INT16PTR:  intrA = *(a->val.int16Ptr);     isAFixed = true; break;
	case genVar::INT32PTR:  intrA = *(a->val.int32Ptr);     isAFixed = true; break;
	case genVar::UINT16PTR: intrA = *(a->val.uint16Ptr);    isAFixed = true; break;
	case genVar::UINT32PTR: intrA = *(a->val.uint32Ptr);    isAFixed = true; break;

	case genVar::DOUBLE:     fltA = a->val.dbl;        isAFloat = true; break;
	case genVar::DOUBLEPTR:  fltA = *(a->val.dblPtr);  isAFloat = true; break;
	case genVar::FLOAT:      fltA = a->val.flt;        isAFloat = true; break;
	case genVar::FLOATPTR:   fltA = *(a->val.fltPtr);  isAFloat = true; break;
	default:
		d1println("genVar op not implemented");
	break;
	}

	int64_t intrB;
	double fltB;
	boolean isBFixed = false;
	boolean isBFloat = false;
	
	switch(b->t){
	case genVar::INT8:  intrB = b->val.int8;     isBFixed = true; break;
	case genVar::INT16: intrB = b->val.int16;    isBFixed = true; break;
	case genVar::INT32: intrB = b->val.int32;    isBFixed = true; break;
	case genVar::INT64: intrB = b->val.int64;    isBFixed = true; break;
	case genVar::UINT8:  intrB = b->val.uint8;    isBFixed = true; break;
	case genVar::UINT16: intrB = b->val.uint16;  isBFixed = true; break;
	case genVar::UINT32: intrB = b->val.uint32;  isBFixed = true; break;
	case genVar::UINT64: intrB = b->val.uint64;  isBFixed = true; break;

	case genVar::INT16PTR:  intrB = *(b->val.int16Ptr);     isBFixed = true; break;
	case genVar::INT32PTR:  intrB = *(b->val.int32Ptr);     isBFixed = true; break;
	case genVar::UINT16PTR: intrB = *(b->val.uint16Ptr);    isBFixed = true; break;
	case genVar::UINT32PTR: intrB = *(b->val.uint32Ptr);    isBFixed = true; break;

	case genVar::DOUBLE:     fltA = b->val.dbl;        isBFloat = true; break;
	case genVar::DOUBLEPTR:  fltA = *(b->val.dblPtr);  isBFloat = true; break;
	case genVar::FLOAT:      fltA = b->val.flt;        isBFloat = true; break;
	case genVar::FLOATPTR:   fltA = *(b->val.fltPtr);  isBFloat = true; break;
	default:
		d1println("genVar op not implemented");
	break;
	}
	
	switch(t){
	case conversion::ADD:
		if(genVarIsFixed(res)){
			if(isAFixed && isBFixed){
				genVarAssignInt(res,intrA + intrB);
			}else if(isAFixed && isBFloat){
				genVarAssignInt(res,intrA + (int64_t)fltB);
			}else if(isAFloat && isBFixed){
				genVarAssignInt(res,(int64_t)fltA + intrB);
			}else if(isAFloat && isBFloat){
				genVarAssignInt(res,(int64_t)fltA + (int64_t)fltB);
			}else{
				d1println("Addition of unknown type");
			}
		}else if(genVarIsFloat(res)){
			if(isAFixed && isBFixed){
				genVarAssignFloat(res,intrA + intrB);
			}else if(isAFixed && isBFloat){
				genVarAssignFloat(res,intrA +  fltB);
			}else if(isAFloat && isBFixed){
				genVarAssignFloat(res,fltA  + intrB);
			}else if(isAFloat && isBFloat){
				genVarAssignFloat(res,fltA  +  fltB);
			}else{
				d1println("Addition of unknown type");
			}
		}
	default:
		d1println("Unimplemented conversion");
	break;
	}
}

boolean genVarIsFloat(genVar_t* var){
	switch(var->t){
	case genVar::DOUBLE: return true;
	case genVar::DOUBLEPTR: return true;
	case genVar::FLOAT: return true;
	case genVar::FLOATPTR: return true;
	default: return false;
	}
	return false;
}

boolean genVarIsFixed(genVar_t* var){
	switch(var->t){
	case genVar::INT8:  return true;
	case genVar::INT16: return true;
	case genVar::INT32: return true;
	case genVar::INT64: return true;
	case genVar::UINT8:  return true;
	case genVar::UINT16: return true;
	case genVar::UINT32: return true;
	case genVar::UINT64: return true;
	case genVar::INT16PTR:  return true;
	case genVar::INT32PTR:  return true;
	case genVar::UINT16PTR: return true;
	case genVar::UINT32PTR: return true;
	default: return false;
	}
	return false;
}

void genVarAssignInt(genVar_t* var,int64_t in){
	switch(var->t){
	case genVar::INT8:  var->val.int8  = (int8_t)in;  break;
	case genVar::INT16: var->val.int16 = (int16_t)in; break;
	case genVar::INT32: var->val.int32 = (int32_t)in; break;
	case genVar::INT64: var->val.int64 = (int64_t)in; break;
	case genVar::UINT8:  var->val.uint8   =  (uint8_t)in;  break;
	case genVar::UINT16: var->val.uint16 =  (uint16_t)in; break;
	case genVar::UINT32: var->val.uint32 =  (uint32_t)in; break;
	case genVar::UINT64: var->val.uint64 =  (uint64_t)in; break;

	case genVar::INT16PTR:  *(var->val.int16Ptr) =  (int16_t)in; break;
	case genVar::INT32PTR:  *(var->val.int32Ptr) =  (int32_t)in; break;
	case genVar::UINT16PTR: *(var->val.uint16Ptr) = (uint16_t)in; break;
	case genVar::UINT32PTR: *(var->val.uint32Ptr) = (uint32_t)in; break;

	default:
		d1println("genVar assign not implemented for Int");
	break;
	}
}

void genVarAssignFloat(genVar_t* var,double in){
	switch(var->t){
	case genVar::DOUBLE:     var->val.dbl;        break;
	case genVar::DOUBLEPTR:  *(var->val.dblPtr);  break;
	case genVar::FLOAT:      var->val.flt;        break;
	case genVar::FLOATPTR:   *(var->val.fltPtr);  break;
	default:
		d1println("genVar assign not implemented for Float");
	break;
	}
}

//TODO actually implement this
genVar_t convert(genVar_t* in,conversion_t* conv){
	switch(conv->t){
	case conversion::ADD:
		
	break;
	}
	return (*in);
}



//this doesn't do much but copy the pointer, may be useful later to separate
void genVarRegArray(genVar_t* var,genVar::type typ, void* array){
	switch(typ){
	case genVar::INT16PTR:
		var->val.int16Ptr = (int16_t*)array;
	break;
	case genVar::UINT16PTR:
		var->val.uint16Ptr = (uint16_t*)array;
	break;
	case genVar::INT32PTR:
		var->val.int32Ptr = (int32_t*)array;
	break;
	case genVar::UINT32PTR:
		var->val.uint32Ptr = (uint32_t*)array;
	break;
	case genVar::FLOATPTR:
		var->val.fltPtr = (float*)array;
	break;
	case genVar::DOUBLEPTR:
		var->val.dblPtr = (double*)array;
	break;
	default:
		d1println("genVarRegArray not array type");
	break;
	}
}

//TODO I don't think this works yet, does it?
void createUserVarCal(userVar_t* var,genVar::type inType, genVar::type outType, void* inAr, void* outAr,int inLng, int outLng){
	var->conv.t = conversion::CAL;
	var->conv.A.t = inType;
	var->conv.B.t = outType;
	genVarRegArray(&(var->conv.A),inType,inAr);
	genVarRegArray(&(var->conv.B),outType,outAr);
	var->conv.A.lng = inLng;
	var->conv.B.lng = outLng;
}

//TODO doesn't work yet
void updateUserVar(userVar_t* var, genVar_t in){
	var->lastUpdate = getTimeStamp();
	
}