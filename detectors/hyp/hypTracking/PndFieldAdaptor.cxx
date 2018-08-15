//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndFieldAdaptor
//      see PndFieldAdaptor.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndFieldAdaptor.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include<iostream>

// Collaborating Class Headers --------
#include "FairField.h"

// Class Member definitions -----------


PndFieldAdaptor::PndFieldAdaptor(FairField* field)
  : fField(field)
{}

TVector3
PndFieldAdaptor::get(const TVector3& pos) const {
  assert(fField!=NULL);
  Double_t x[3];
  x[0] = pos.X();
  x[1] = pos.Y();
  x[2] = pos.Z();

  Double_t f[3];
  fField->GetFieldValue(x,f);
  //std::cout << "PndFieldAdapter::get() field at "
  //	    <<x[0]<<","<<x[1]<<","<<x[2]<<" has value "
  //        <<f[0]<<","<<f[1]<<","<<f[2]<<std::endl;
  return TVector3(f[0],f[1],f[2]);
}

ClassImp(PndFieldAdaptor);
