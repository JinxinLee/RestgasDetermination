#ifndef FIT_H
#define FIT_H

#include "TMinuit.h"
#include "TROOT.h"

#include <iostream>
#include <cmath>

#include "Config.h"


void residuals(double ax, double bx,
			   double ay, double by,
			   double x, double y, double z,
			   double &dx, double &dy, double &dz);


void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);



struct fitArg_t{
  int nHits;
  double x[MAXHITS]; 
  double y[MAXHITS]; 
  double z[MAXHITS];
  double xerr[MAXHITS];
  double yerr[MAXHITS]; 
  double zerr[MAXHITS];
  double ax_s;
  double bx_s;
  double ay_s; 
  double by_s;
};

struct fitReturn_t {
  double chi2;
  double ax;
  double bx;
  double ay;
  double by;
};



fitReturn_t doFit(fitArg_t fitArg);



#endif // FIT_H
 
 
