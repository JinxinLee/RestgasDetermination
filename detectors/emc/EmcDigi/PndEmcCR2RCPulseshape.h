//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Pulseshape from an CR2RC-Shaper
//
//-----------------------------------------------------------
//#pragma once
#ifndef PNDEMCCR2RCPULSESHAPE_H
#define PNDEMCCR2RCPULSESHAPE_H

// Base Class Headers ----------------
#include "PndEmcAbsPulseshape.h"

// Collaborating Class Headers -------
//#include <ostream> 

/**
 * @brief Pulseshape from an CR2RC-Shaper
 * @ingroup PndEmc
 */
class PndEmcCR2RCPulseshape  : public PndEmcAbsPulseshape
{
public:

  // Constructors/Destructors ---------
  PndEmcCR2RCPulseshape();
  PndEmcCR2RCPulseshape(double Tint, double Tdif, double Tsig);
  virtual ~PndEmcCR2RCPulseshape(){;}

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
};

#endif

