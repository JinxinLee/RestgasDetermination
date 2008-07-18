//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcDigiAge
//      see PndTpcDigiAge.hh for details
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
#include "PndTpcDigiAge.h"
#include "PndTpcDigi.h"

bool 
PndTpcDigiAge::operator()(PndTpcDigi* s1, PndTpcDigi* s2)
{
  return s1->t()<s2->t();
}

