//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class ppstate_end
//      see ppstate_end.hh for details
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
#include "TORPPState_End.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "TORPadProcessor.h"

// Class Member definitions -----------

ppstate_end::ppstate_end(padprocessor* pp)
  : ppstate(pp)
{}

std::string
ppstate_end::heartbeat()
{
  return "end";
}

