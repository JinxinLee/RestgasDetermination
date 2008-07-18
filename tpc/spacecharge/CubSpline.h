//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Cubic spline on BSpline basis
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            
//      Felix Boehmer        TUM
//
//-----------------------------------------------------------

#ifndef CUBSPLINE_H
#define CUBSPLINE_H

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <vector>
#include "BSpline.h"



class CubSpline {

friend class CubSplineFitter;

public:

  // Constructors/Destructors ---------
  CubSpline(const std::vector<double>* k, std::vector<double>* c=NULL);
  ~CubSpline();
  //NOTE: There have to be 4 ADDITIONAL points before (after) the first
  //(last) knot outside the data area!!!

  // Operators

  // Accessors -----------------------
  const std::vector<double>* getKnots() {return _k;}
  std::vector<double>* getCoeffs() {return _c;}


  // Modifiers -----------------------
  void setCoeffs(std::vector<double>* c) {_c = c;}    //Parameter Interface
  void setCoeffsByArray(double*);


  // Operations ----------------------
  double eval(double x);

private:

  // Private Data Members ------------
  std::vector<BSpline*> _M;
  const std::vector<double>* _k; // knots
  std::vector<double>* _c; // coefficients
  int _n;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
