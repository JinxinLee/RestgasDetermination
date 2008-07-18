//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      
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

#ifndef BSPLINE_HH
#define BSPLINE_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <vector>
#include <iostream>

// Collaborating Class Declarations --



class BSpline {
public:

  // Constructors/Destructors ---------
  BSpline(const std::vector<double>* const p, int index);
  ~BSpline(){}

  // Operators
  void print();  

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  double eval(double x);

private:

  // Private Data Members ------------
  const std::vector<double>* _p; // knots
  int _i;        // spline index //should be made CONST later
  double _nu[5]; // coefficients

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
