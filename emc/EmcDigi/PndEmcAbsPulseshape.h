//-----------------------------------------------------------
// File and Version Information:
// $Id:$
//
// Description:
//      EMC Digitization
//      abstract Base class for Pulseshapes
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//-----------------------------------------------------------

#ifndef PNDEMCABSPULSESHAPE_HH
#define PNDEMCABSPULSESHAPE_HH

// Collaborating Class Headers -------

#include "TObject.h"

// Collaborating Class Declarations --

class PndEmcAbsPulseshape: public TObject 
{
public:

  // Constructors/Destructors ---------
  PndEmcAbsPulseshape(){;}
  virtual ~PndEmcAbsPulseshape(){;}

  // Operators
  
  virtual double operator() (const double t, 
			     const double amp, const double toffset) const = 0;
  virtual double value(const double t, 
		       const double amp, const double toffset) const 
  {return operator()(t,amp,toffset);}

  ClassDef(PndEmcAbsPulseshape,1)
};

#endif
