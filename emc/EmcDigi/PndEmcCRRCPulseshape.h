//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Pulseshape from an CRRC-Shaper
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//-----------------------------------------------------------
//#pragma once
#ifndef PNDEMCCRRCPULSESHAPE_H
#define PNDEMCCRRCPULSESHAPE_H

// Base Class Headers ----------------
#include "PndEmcAbsPulseshape.h"

// Collaborating Class Headers -------
//#include <ostream> 

class PndEmcCRRCPulseshape  : public PndEmcAbsPulseshape
{
public:

  // Constructors/Destructors ---------
  PndEmcCRRCPulseshape();
  PndEmcCRRCPulseshape(double Tint, double Tdif, double Tsig);
  virtual ~PndEmcCRRCPulseshape(){;}

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual double operator() (const double t, 
			     const double amp, 
			     const double toffset) const;

private:

  // Private Data Members ------------
  double fTint;
  double fTdif;
  double fTsig;
 

  // Private Methods -----------------
  
  double general_solution(const double t, 
		     const double amp, 
		     const double toffset) const;
  double degenerate_solution(const double t,  // for Tdif=Tint
		     const double amp, 
		     const double toffset) const; 

  ClassDef(PndEmcCRRCPulseshape,1)
};

#endif

