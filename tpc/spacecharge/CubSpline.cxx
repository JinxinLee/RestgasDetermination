//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class CubSpline
//      see CubSpline.h for details
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
#include "CubSpline.h"


// C/C++ Headers ----------------------

// Collaborating Class Headers --------
#include "TError.h"

// Class Member definitions -----------

CubSpline::CubSpline(const std::vector<double>* k, std::vector<double>* c) 
{
  _k=k;
  _n=_k->size();
  if(_n<5)
    Fatal("CubSpline::CubSpline()", "not enough knots, aborting...");
  if(c==NULL)
    _c = new std::vector<double>(_n-4, 1.);   //init standard coeffs
  else
    _c=c;
  if(_k->size() != _c->size() + 4)
    Fatal("CubSpline::CubSpline()", "coeffs-knots consistency error!");
  for(int i=0;i<_n;i++)    {
      // build Basis Functions on knots k
      if(i>3){ // note the additional knots needed!!!
	_M.push_back(new BSpline(_k,i)); 
      }
  }
}



//TODO: define a flag that marks if _c was created by CubSpline itself
//in this case, delete _c
CubSpline::~CubSpline() 
{
  for(int i=0; i< _M.size(); i++)
    delete _M[i];
}


void
CubSpline::setCoeffsByArray(double* c) {    //TODO: size check!
  //have to follow TF1 interface restrictions and take a double* !
  //user is responsible for proper format
  //happy segmentating
  for(int i=0; i<_n-4; i++)
    _c->at(i) = c[i];
}


double
CubSpline::eval(double x){
  //search appropriate BSplines
  int i=0;
  for(i=0; i<_n; i++) {
    if(_k->at(i)>x)
      break;
  }
  
  // catch out of bound cases
  if(i>=_n-4 || i<4) {  //x was out of range
    Warning("CubSpline::eval()", "x was out of range!");
    return 0;
  }
  
  double result=0;
  
  for(int j=i; j<i+4; j++) {
    //std::cout<<"\nProcessing B-Spline # "<<j-4<<" for x = "
    //     <<x<<" and i = "<<i<<std::endl;
    //  std::cout<<"connected weighting parameter: "<<_a->at(j)<<std::endl;
    //NOTE: _M[0] is the BSpline at knot #5!
    result+=(_M[j-4]->eval(x)) * _c->at(j-4);
  }
  
  return result;
}
