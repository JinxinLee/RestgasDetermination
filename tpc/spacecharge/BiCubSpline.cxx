//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class BiCubSpline
//      see BiCubSpline.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "BiCubSpline.h"

// C/C++ Headers ----------------------

// Collaborating Class Headers --------
#include "TError.h"
// Class Member definitions -----------

BiCubSpline::BiCubSpline(const std::vector<double>* kx,
			 const std::vector<double>* ky,
			 std::vector<std::vector<double>*>* c)
    
{
  _xoutofrange=false;
  _youtofrange=false;	
  _kx = kx;
  _ky = ky;
  _xLength = _kx->size();
  _yLength = _ky->size();
  
  if(_xLength<5 || _yLength<5)
    Fatal("BiCubSpline::BiCubSpline()", "not enough knots, aborting...");

  if(c!=NULL)
    _coeffs = c;
  else {    //build standard coeffs...
    _coeffs = new std::vector<std::vector<double>*>(_yLength-4);
    for(int y=0; y<_yLength-4; y++) {
      for(int x=0; x<_xLength-4; x++)
	_coeffs->at(y)->push_back(1);
    }
  }          
  //first 4 knots in x and y dir. have no attached Splines...
  if(_coeffs->size()+4==_yLength && _coeffs->at(0)->size()+4==_xLength)
    ;
  else
    Fatal("BiCubSpline::BiCubSpline()", "coeffs-knots inconsistent! Aborting");
  //build B-Spline basis
  for(int i=0; i<_xLength; ++i) {
    if(i>3) 
      _M.push_back(new BSpline(_kx,i));
  }
  for(int j=0; j<_yLength; ++j) {
    if(j>3) 
      _N.push_back(new BSpline(_ky,j));
  }
}

BiCubSpline::~BiCubSpline() {   //TODO: everything :)
}
  
void
BiCubSpline::setCoeffsByArray(double* ca) {   //TO DO: checks. (impossible!)
  //have to follow TF2 interface restrictions and take a double* !
  //user is responsible for proper array format!
  for(int y=0; y<_yLength-4; y++) {
    for(int x=0; x<_xLength-4; x++) 
      _coeffs->at(y)->at(x) = ca[y*(_xLength-4) + x];
  }
}  


double 
BiCubSpline::eval(double x, double y) {
  //search for the right BSplines:
  int i,j;
  for(i=0; i<_xLength; ++i) {
    if(_kx->at(i)>x)
      break;
  }
  for(j=0; j<_yLength; ++j) {
    if(_ky->at(j)>y)
      break;
  }
  
  //catch out of bound cases
  if(i>=_xLength-4 || i<4) {  //x was out of range
    if(_xoutofrange==false) {
      Warning("BiCubSpline::eval()", "x was out of range!");
      _xoutofrange=true;
    }
    return 0;
  }
  if(j>=_yLength-4 || j<4) {  //y was out of range
	if(_youtofrange==false) {
	  Warning("BiCubSpline::eval()", "y was out of range!");
	  _youtofrange=true;
	}
    return 0;
  }

  double result=0;

  for(int k=i; k<i+4; k++) {
    for(int l=j; l<j+4; l++) 
      result+=_M[k-4]->eval(x) * _N[l-4]->eval(y) * _coeffs->at(l-4)->at(k-4);
  }

  return result;
}


