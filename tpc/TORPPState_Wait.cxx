//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class ppstate_wait
//      see ppstate_wait.hh for details
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
#include "TORPPState_Wait.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "TORPadProcessor.h"

// Class Member definitions -----------

ppstate_wait::ppstate_wait(padprocessor* pp)
  : ppstate(pp), fndata(0), fcounter(0)
{}

std::string
ppstate_wait::heartbeat()
{
  //++fcounter;
  //if(fcounter%2==0)return "wait";

  if(fndata==fparent->fdata.size()){
    // no change in last iteration;
    
    fndata=0;
    fparent->setState("output");
    return "output";
  }
  else {
    fndata=fparent->fdata.size();
    return "wait";
  }
}

