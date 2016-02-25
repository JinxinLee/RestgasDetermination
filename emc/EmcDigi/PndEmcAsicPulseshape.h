//-----------------------------------------------------------
// Description:
//      Pulseshape from an APFEL ASIC preamplifier shaper
//      Circuit is described in EMC TDR and consists of charge-sensitive preamplifier,
//      with the following pole-zero cancelation of its feedback ad 3rd order integrator.
//		  The pulseshape is determined by the integration time of single intagrator stage 
//      and scintillation time of the crystal, with exponential input convoluted with
//      response of the circuit  
//
//-----------------------------------------------------------
//#pragma once
#ifndef PNDEMCASICPULSESHAPE_H
#define PNDEMCASICPULSESHAPE_H

// Base Class Headers ----------------
#include "PndEmcAbsPulseshape.h"

// Collaborating Class Headers -------
//#include <ostream> 

/**
 * @brief Pulseshape from an APFEL ASIC preamplifier shaper
 * 
 * Circuit is described in EMC TDR and consists of charge-sensitive preamplifier,
 * with the following pole-zero cancelation of its feedback ad 3rd order integrator.
 * The pulseshape is determined by the integration time of single intagrator stage 
 * and scintillation time of the crystal, with exponential input convoluted with
 * response of the circuit
 * @ingroup PndEmc
 */
class PndEmcAsicPulseshape  : public PndEmcAbsPulseshape
{
public:

  // Constructors/Destructors ---------
  PndEmcAsicPulseshape();
  PndEmcAsicPulseshape(double Tint, double Tsig);
  virtual ~PndEmcAsicPulseshape(){;}

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
  double fTsig;
 

  // Private Methods -----------------
  
  double general_solution(const double t, 
		     const double amp, 
		     const double toffset) const;
  double degenerate_solution(const double t,  // for Tdif=Tint
		     const double amp, 
		     const double toffset) const; 
  
  ClassDef(PndEmcAsicPulseshape,1)
};

#endif

