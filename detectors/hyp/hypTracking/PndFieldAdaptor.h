//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Adapts a PndField to genfit GFAbsBFieldIfc
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//      adapted by Alicia Sanchez for HYP purpose
//-----------------------------------------------------------

#ifndef PNDFIELDADAPTOR_HH
#define PNDFIELDADAPTOR_HH

// Base Class Headers ----------------
#include "GFAbsBField.h"
#include"TObject.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class FairField;


class PndFieldAdaptor : public GFAbsBField, public TObject {
public:

  // Constructors/Destructors ---------
  PndFieldAdaptor(){fField=NULL;}
  PndFieldAdaptor(FairField* field);
  
  // Accessors -----------------------
  TVector3 get(const TVector3&) const;


private:

  // Private Data Members ------------
  FairField* fField;

  // Private Methods -----------------

 public:
  ClassDef(PndFieldAdaptor,1);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
