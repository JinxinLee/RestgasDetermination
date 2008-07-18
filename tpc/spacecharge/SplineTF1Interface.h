//-----------------------------------------------------------
// File and Version Information:
// Version 1.0  (Feb 27, 2008)
//
// Description:
//      Spline-Interface for TF1
//      Allows TF1 implementation of a CubSpline 
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer        TUM            (original author)
//
//
//-----------------------------------------------------------


//instructions: (start with CubSpline* csp)

//SplineTF1Interface* stf1 = new SplineTF1Interface();   
//stf1->setCSP(csp);
//TF1* f1 =new TF1("f1",stf1,&SplineTF1Interface::eval,
//                 xlow,xhigh,Ncoeff,"Function", "eval");

//where the last two entries should be optional when uses right.


#ifndef SPTF1IF_H
#define SPTF1IF_H

// Base Class Headers ----------------

// C++ headers -----------------------
#include <vector>

// Collaborating Class Headers -------
#include "CubSpline.h"



class SplineTF1Interface {

public:

  //Constructors/Destructors ---------
  SplineTF1Interface(CubSpline* csp); 
  
  // Accessors -----------------------
  CubSpline* getCSP() {return _csp;}
   
  // Operations ----------------------
  double eval(double *x, double *p); 


private:

  // Private Data Members ------------  
  CubSpline* _csp;
  const std::vector<double>* _knots;
  std::vector<double>* _coeffs;
  int _length;

};

#endif
