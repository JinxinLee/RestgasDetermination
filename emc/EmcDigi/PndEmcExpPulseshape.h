//-----------------------------------------------------------
// Description:
//      Exponential Pulseshape
//-----------------------------------------------------------
//#pragma once
#ifndef PndEmcExpPulseshape_H
#define PndEmcExpPulseshape_H

// Base Class Headers ----------------
#include "PndEmcAbsPulseshape.h"

// Collaborating Class Headers -------
//#include <ostream> 

class PndEmcExpPulseshape  : public PndEmcAbsPulseshape
{
public:

  // Constructors/Destructors ---------
  PndEmcExpPulseshape():fTdif(0){;}
  PndEmcExpPulseshape(double Tdif);
  virtual ~PndEmcExpPulseshape(){;}

  // Operations ----------------------
  virtual double operator() (const double t, 
			     const double amp, 
			     const double toffset) const;

private:

  // Private Data Members ------------
  double fTdif;
 

  // Private Methods -----------------
  
  double general_solution(const double t, 
		     const double amp, 
		     const double toffset) const;
};

#endif

