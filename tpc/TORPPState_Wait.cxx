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
  : ppstate(pp), _ndata(0), _counter(0)
{}

std::string
ppstate_wait::heartbeat()
{
  //++_counter;
  //if(_counter%2==0)return "wait";

  if(_ndata==_parent->_data.size()){
    // no change in last iteration;
    
    _ndata=0;
    _parent->setState("output");
    return "output";
  }
  else {
    _ndata=_parent->_data.size();
    return "wait";
  }
}

