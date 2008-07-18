//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcSignalAge
//      see PndTpcSignalAge.h for details
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
#include "PndTpcSignalAge.h"
#include "PndTpcSignal.h"

bool 
PndTpcSignalAge::operator()(PndTpcSignal* s1, PndTpcSignal* s2)
{
  return s1->t()<s2->t();
}

