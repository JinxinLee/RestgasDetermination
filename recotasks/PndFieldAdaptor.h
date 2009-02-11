//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Adapts a PndField to genfit AbsBFieldIfc
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

#ifndef PNDFIELDADAPTOR_HH
#define PNDFIELDADAPTOR_HH

// Base Class Headers ----------------
#include "AbsBFieldIfc.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class FairField;


class PndFieldAdaptor : public AbsBFieldIfc {
public:

  // Constructors/Destructors ---------
  PndFieldAdaptor(FairField* field);
  
  // Accessors -----------------------
  TVectorT<double> get(const TVectorT<double>& pos) const;


private:

  // Private Data Members ------------
  FairField* _field;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
