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
#pragma once
#ifndef PNDEMCABSPULSESHAPE_H
#define PNDEMCABSPULSESHAPE_H

#include "TObject.h"
// Collaborating Class Headers -------

// Collaborating Class Declarations --

/**
 * @brief pulseshape interface
 * @ingroup PndEmc
 */
class PndEmcAbsPulseshape : public TObject
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
