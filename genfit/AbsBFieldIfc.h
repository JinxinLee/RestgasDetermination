//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Abstract interface to a BField
//      User has to inherit his adaptor from this class
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef ABSBFIELDIFC_HH
#define ABSBFIELDIFC_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "TVectorT.h"

// Collaborating Class Declarations --


class AbsBFieldIfc {
public:

  // Constructors/Destructors ---------
  AbsBFieldIfc(){;}
  virtual ~AbsBFieldIfc(){;}

  // Accessors -----------------------
  virtual TVectorT<double> get(const TVectorT<double>& pos) const =0;
 
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
