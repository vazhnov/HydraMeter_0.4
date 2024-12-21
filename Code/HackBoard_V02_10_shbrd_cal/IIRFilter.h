#include "util.h"

#ifndef IIRFILTER_H
#define IIRFILTER_H

class IIR{
	public:
	double a0;
	double a1;
	double a2;
	double b1;
	double b2;

	IIR(double fs, double f0, double Q){
		compute(fs,f0,Q);
	}

	void compute(double fs,double f0,double Q){
	  double w0 = 2*PI*f0/fs;
	  double a = sin(w0) / (2*Q);
	  double b0 = 1+a;
	  
	  a0 = 1/b0;
	  a1 = (-2*cos(w0))/b0;
	  a2 = 1/b0;

	  b1 = -2*cos(w0)/b0;
	  b2 = (1-a)/b0;
	}

	double apply(double x0){
		static double x1 = 0;
		static double x2 = 0;
		static double y1 = 0;
		static double y2 = 0;
		double y0 = (a0*x0) + (a1*x1) + (a2*x2) - ( (b1*y1) + (b2*y2) );
		y2 = y1;
		x2 = x1;
		y1 = y0;
		x1 = x0;
		return y0;
	}
};
#endif