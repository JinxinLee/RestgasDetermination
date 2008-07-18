//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class CbmFieldAdaptor
//      see CbmFieldAdaptor.hh for details
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
#include "CbmFieldAdaptor.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include<iostream>

// Collaborating Class Headers --------
#include "CbmField.h"

// Class Member definitions -----------


CbmFieldAdaptor::CbmFieldAdaptor(CbmField* field)
  : _field(field)
{}

TVectorT<double> 
CbmFieldAdaptor::get(const TVectorT<double>& pos) const {
  assert(_field!=NULL);
  Double_t f[3];
  _field->GetFieldValue(pos.GetMatrixArray(),f);
  // convert from kG to T
  f[0]*=0.1;
  f[1]*=0.1;
  f[2]*=0.1;
  return TVectorT<double>(3,f);
}
