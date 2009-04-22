//-----------------------------------------------------------
// File and Version Information:
// Version 1.0  (Apr 7, 2008)
//
// Description:
//      Spline-Interface for TF2
//      Allows TF2 implementation of a BiCubSpline 
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//
//-----------------------------------------------------------

//instructions: see SplineTF1Interface.h

#ifndef SPTF2IF_H
#define SPTF2IF_H

// Base Class Headers ----------------

// C++ headers -----------------------
#include <vector>

// Collaborating Class Headers -------
#include "BiCubSpline.h"


// helper class for TF implementation ----------------------------
class SplineTF2Interface {


public:

  //Constructors/Destructors ---------
  SplineTF2Interface();
  SplineTF2Interface(BiCubSpline* bcsp); 
  ~SplineTF2Interface(){}

  // Accessors -----------------------
  BiCubSpline* getBCSP() {return _bcsp;}
  void setReadOnly(bool opt) {_read_only=opt;}

  // Operations ----------------------
  double eval(double *x, double *p); 



private:

  // Private Data Members ------------
  BiCubSpline* _bcsp;
  const std::vector<double>* _kx;
  const std::vector<double>* _ky;
  std::vector<std::vector<double>*>* _coeffs;
  int _xl, _yl;

  bool _read_only;  //if set ignore parameters in eval argument and just read 
                    //the splines own coefficients

 
};

#endif

 
