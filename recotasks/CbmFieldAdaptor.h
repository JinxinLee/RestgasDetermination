//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Adapts a CbmField to genfit AbsBFieldIfc
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

#ifndef CBMFIELDADAPTOR_HH
#define CBMFIELDADAPTOR_HH

// Base Class Headers ----------------
#include "AbsBFieldIfc.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class CbmField;


class CbmFieldAdaptor : public AbsBFieldIfc {
public:

  // Constructors/Destructors ---------
  CbmFieldAdaptor(CbmField* field);
  
  // Accessors -----------------------
  TVectorT<double> get(const TVectorT<double>& pos) const;


private:

  // Private Data Members ------------
  CbmField* _field;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
