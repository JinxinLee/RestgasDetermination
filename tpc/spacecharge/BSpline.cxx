//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class BSpline
//      see BSpline.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            
//      Felix Boehmer        TUM
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "BSpline.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------


// Class Member definitions -----------


BSpline::BSpline() 
  :
  _i(0),
  _p(NULL)
{;}

BSpline::BSpline(const std::vector<double>* const p, int index)
  :
  _i(index)
{
  _p = p;
  for(int i=0;i<5;i++){
    _nu[i]=1;
  }
  for(int s=1;s<5;s++){
    for(int r=0;r<5;r++){
      if(s==r)continue;
      _nu[s]*=_p->at(_i-s)-_p->at(_i-r);
    }
    _nu[s]=1/_nu[s];
  }
  _nu[0]=0;
}

BSpline::~BSpline()
{}


double 
BSpline::eval(double x){

  if(x>(_p->at(_i)) || x<(_p->at(_i-4))) {
    //std::cout<<"\nBSPline::out of range of BSpline - should never get here"
    //	     <<std::endl;
    //std::cout<<"x was "<<x<<", lamda_max would be "<<_i<<std::endl;
    return 0;
  }

  double result=0;

  for(int s=1;s<5;s++){
    double pol = x - (_p->at(_i-s));
    if(pol<0)continue;
    pol=pol*pol*pol; // pol^3
    result+=_nu[s]*pol;
  }
  return result;    
}

//for testing purposes
void
BSpline::print() {
  std::cout<<"\nBSpline: Knot index "<<_i<<", x_i-position: "
	   <<_p->at(_i)<<std::endl;
  std::cout<<"Coefficients (from i to i-4): "<<std::endl;
  for(int j=0; j<5; j++)
    std::cout<<_nu[j]<<"   ";
}


ClassImp(BSpline)
