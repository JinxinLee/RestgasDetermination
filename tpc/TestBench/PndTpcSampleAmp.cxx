//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcSampleAge
//      see PndTpcSampleAmp.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Maxence Vandenbroucke    TUM            (original author)
//
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndTpcSampleAmp.h"
#include "PndTpcSample.h"

bool 
PndTpcSampleAmp::operator()(PndTpcSample* s1, PndTpcSample* s2)
{
  return s1->amp()<s2->amp();
}

