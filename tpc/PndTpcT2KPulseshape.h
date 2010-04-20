//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Pulseshape from an CRRC^6-Shaper describing the AFTER-T2K shapper
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Maxence Vandenbroucke    TUM            (original author)
//
//-----------------------------------------------------------

#ifndef TPCT2KPULSESHAPE_HH
#define TPCT2KPULSESHAPE_HH

// Base Class Headers ----------------
#include "PndTpcAbsPulseshape.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --



class PndTpcT2KPulseshape  : public PndTpcAbsPulseshape{
public:

  // Constructors/Destructors ---------
  PndTpcT2KPulseshape(int Tp);
  virtual ~PndTpcT2KPulseshape(){;}

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual double operator() (const double t, 
			     const double amp, 
			     const double toffset) const;

private:

  // Private Data Members ------------
  int fTp; //value of the T2K register definding peaking time
              //must have one these value : 100,200,400,1000
 

  // Private Methods -----------------
  
double solution_100(const double t, 
		    const double amp, 
		    const double toffset) const;

double solution_200(const double t, 
		    const double amp, 
		    const double toffset) const;

double solution_400(const double t, 
		    const double amp, 
		    const double toffset) const;

double solution_1000(const double t, 
		    const double amp, 
		    const double toffset) const;
};
#endif

//--------------------------------------------------------------
// $Log$
//-----------------------------------
