//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class ppstate_initial
//      see ppstate_initial.hh for details
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
#include "TORPPState_Initial.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "TORPadProcessor.h"

// Class Member definitions -----------

ppstate_initial::ppstate_initial(padprocessor* pp)
  : ppstate(pp)
{}

std::string
ppstate_initial::heartbeat()
{
  //std::cout<<"Pad("<<fparent->id()<<").famps.size()="
  //   <<fparent->famps.size()<<std::endl;
  if(fparent->mydata()!=0){
    fparent->setState("compare");
    return "compare";
  }
  else {
    fparent->setState("end");
    return "end";
  }
}

