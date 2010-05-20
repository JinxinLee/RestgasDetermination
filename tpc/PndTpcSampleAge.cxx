//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcSamplePad
//      see PndTpcSamplePad.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcSampleAge.h"
#include "PndTpcSample.h"

bool 
PndTpcSampleAge::operator()(PndTpcSample* s1, PndTpcSample* s2)
{
  return s1->t()<s2->t();
}

