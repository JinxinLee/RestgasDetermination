//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Pulseshape from an CRRC-Shaper
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCCRRCPULSESHAPE_HH
#define TPCCRRCPULSESHAPE_HH

// Base Class Headers ----------------
#include "PndTpcAbsPulseshape.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --



class PndTpcCRRCPulseshape  : public PndTpcAbsPulseshape{
public:

  // Constructors/Destructors ---------
  PndTpcCRRCPulseshape(){;}
  PndTpcCRRCPulseshape(double Tint, double Tdif, double Tsig);
  virtual ~PndTpcCRRCPulseshape(){;}

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual double operator() (const double t, 
			     const double amp, 
			     const double toffset) const;

private:

  // Private Data Members ------------
  double _Tint;
  double _Tdif;
  double _Tsig;
 

  // Private Methods -----------------
  
  double general_solution(const double t, 
		     const double amp, 
		     const double toffset) const;
  double degenerate_solution(const double t,  // for Tdif=Tint
		     const double amp, 
		     const double toffset) const; 

};

#endif

//--------------------------------------------------------------
// $Log$
//-----------------------------------
