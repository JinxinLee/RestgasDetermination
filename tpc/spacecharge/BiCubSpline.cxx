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
#include "TF2.h"
#include "TArrayD.h"
#include "SplineTF2Interface.h"
#include <sstream>
#include <string>




// Class Member definitions -----------



BiCubSpline::BiCubSpline() {

  _xoutofrange=false;        //TODO: implement some kind of "initialized" flag
  _youtofrange=false;	
  _kx = NULL;
  _ky = NULL;
  _coeffs = NULL;
  _func = NULL;
  _ifc = NULL;
  _xLength = 0;
  _yLength = 0;
    
}  

BiCubSpline::BiCubSpline(const std::vector<double>* kx,
			 const std::vector<double>* ky,
			 std::vector<std::vector<double>*>* c)
    
{
  _xoutofrange=false;
  _youtofrange=false;
  _func = NULL;
  _ifc = NULL;
  _kx = new std::vector<double>(*kx);
  _ky = new std::vector<double>(*ky);
  _xLength = _kx->size();
  _yLength = _ky->size();
  
  if(_xLength<5 || _yLength<5)
    Fatal("BiCubSpline::BiCubSpline()", "not enough knots, aborting...");

  if(c!=NULL)
    _coeffs = c;
  else {    //build standard coeffs...
    _coeffs = new std::vector<std::vector<double>*>(_yLength-4);
    for(int y=0; y<_yLength-4; y++) 
      _coeffs->at(y) = new std::vector<double>(_xLength-4,1.);
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

BiCubSpline::~BiCubSpline() {
  delete _kx;
  delete _ky;  
  for(int m=0; m<_M.size(); ++m) 
    delete _M[m];
  for(int n=0; n<_N.size(); ++n) 
    delete _N[n];
  if(_func!=NULL)
    delete _func;
  if(_ifc!=NULL)
    delete _ifc;
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

TF2*
BiCubSpline::getTF2(double xMin, double xMax, double yMin, double yMax) {
  
  bool fail = false;
  
  //check for consistency:
  if(xMin < _kx->at(0) || xMax > _kx->at(_kx->size()-1))
    fail = true;
  if(yMin < _ky->at(0) || yMax > _ky->at(_ky->size()-1))
    fail = true;
  if(fail) {
    std::cerr<<"\nBiCubSpline::getTF2: invalid range! returning NULL"<<std::endl;
    return NULL;
  }
  
  //need a unique name for the TF2, will use stream cast of the this pointer
  std::stringstream ss;
  ss<<this;
  std::string name = ss.str();
  
  _ifc =  new SplineTF2Interface(this);
  _ifc->setReadOnly(true);
  
  _func = new TF2(name.c_str(),_ifc,&SplineTF2Interface::eval,xMin,xMax,
		  yMin,yMax,(_xLength+4)*(_yLength+4),"Function", "eval");
  
  //dummy parameters, we are in read-only mode
  TArrayD blub = TArrayD((_xLength+4)*(_yLength+4));
  double* dummy = blub.GetArray();
  _func->SetParameters(dummy);
  _func->SetNpx(50);
  _func->SetNpy(50);

  return _func;
}
  

  
  
  


ClassImp(BiCubSpline)
