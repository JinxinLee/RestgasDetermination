//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TNumRecipes				    				//
//                                                                      //
// Functions from num. recipes in C					//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoMath/TNumRecipes.h"

#include <assert.h>
#include <math.h> 
#include <stdlib.h>

ClassImp(TNumRecipes)

TBuffer &operator>>(TBuffer &buf, TNumRecipes *&obj)
{
   obj = (TNumRecipes *) buf.ReadObject(TNumRecipes::Class());
   return buf;
}

#include <iostream>
using namespace std;

Double_t TNumRecipes::Gammln(Double_t xx)
{
    Double_t x,y,tmp,ser;
    static Double_t cof[6]={76.18009172947146,-86.50532032941677,
	24.01409824083091,-1.231739572450155,
	0.1208650973866179e-2,-0.5395239384953e-5};
    int j;
    
    y=x=xx;
    tmp=x+5.5;
    tmp -= (x+0.5)*log(tmp);
    ser=1.000000000190015;
    for (j=0;j<=5;j++) ser += cof[j]/++y;
    return -tmp+log(2.5066282746310005*ser/x);
}

Double_t 
TNumRecipes::Gammp(Double_t a, Double_t x)
{
    Double_t gamser,gammcf,gln;
    
    if (x < 0.0 || a <= 0.0) cerr <<" Invalid arguments in routine gammp x=" << x << " a=" << a << endl;
    if (x < (a+1.0)) {
	Gser(&gamser,a,x,&gln);
	return gamser;
    } else {
	Gcf(&gammcf,a,x,&gln);
	return 1.0-gammcf;
    }
}

Double_t 
TNumRecipes::Gammq(Double_t a, Double_t x)
{
    Double_t gamser,gammcf,gln;
    
    if (x < 0.0 || a <= 0.0) RecipesErr(" Invalid arguments in routine GAMMQ");
    if (x < (a+1.0)) {
	Gser(&gamser,a,x,&gln);
	return 1.0-gamser;
    } else {
	Gcf(&gammcf,a,x,&gln);
	return gammcf;
    }
}

void TNumRecipes::Gcf(Double_t* gammcf, Double_t a, Double_t x, Double_t* gln)
{
    int n;
    Double_t gold=0.0,g,fac=1.0,b1=1.0;
    Double_t b0=0.0,anf,ana,an,a1,a0=1.0;
    
    *gln=Gammln(a);
    a1=x;
    for (n=1;n<=NUMREC_ITMAX;n++) {
	an=(Double_t) n;
	ana=an-a;
	a0=(a1+a0*ana)*fac;
	b0=(b1+b0*ana)*fac;
	anf=an*fac;
	a1=x*a0+anf*a1;
	b1=x*b0+anf*b1;
	if (a1) {
	    fac=1.0/a1;
	    g=b1*fac;
	    if (fabs((g-gold)/g) < NUMREC_EPS) {
		*gammcf=exp(-x+a*log(x)-(*gln))*g;
		return;
	    }
	    gold=g;
	}
    }
    RecipesErr(" a too large, NUMREC_ITMAX too small in routine GCF");
}

void TNumRecipes::Gser(Double_t* gamser, Double_t a, Double_t x, Double_t* gln)
{
    int n;
    Double_t sum,del,ap;
    
    *gln=Gammln(a);
    if (x <= 0.0) {
	if (x < 0.0) RecipesErr(" x less than 0 in routine GSER");
	*gamser=0.0;
	return;
    } else {
	ap=a;
	del=sum=1.0/a;
	for (n=1;n<=NUMREC_ITMAX;n++) {
	    ap += 1.0;
	    del *= x/ap;
	    sum += del;
	    if (fabs(del) < fabs(sum)*NUMREC_EPS) {
		*gamser=sum*exp(-x+a*log(x)-(*gln));
		return;
	    }
	}
	RecipesErr(" a too large, NUMREC_ITMAX too small in routine GSER");
	return;
    }
}

void TNumRecipes::RecipesErr(const char* c)
{
    cerr << " Numerical Recipes run-time error...\n" << c 
	<< "\n ...now exiting to system..." << endl;
}

