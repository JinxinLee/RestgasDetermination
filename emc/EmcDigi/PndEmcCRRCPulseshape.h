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

#ifndef PNDEMCCRRCPULSESHAPE_HH
#define PNDEMCCRRCPULSESHAPE_HH

// Base Class Headers ----------------
#include "PndEmcAbsPulseshape.h"

// Collaborating Class Headers -------
#include <ostream> 

class PndEmcCRRCPulseshape  : public PndEmcAbsPulseshape
{
public:

  // Constructors/Destructors ---------
  PndEmcCRRCPulseshape(){;}
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

  ClassDef(PndEmcCRRCPulseshape,1)
};

#endif

