//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class ppstate_output
//      see ppstate_output.hh for details
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
#include "TORPPState_Output.h"

// C/C++ Headers ----------------------
#include "assert.h"

// Collaborating Class Headers --------
#include "TORPadProcessor.h"


// Class Member definitions -----------

ppstate_output::ppstate_output(padprocessor* pp)
  : ppstate(pp)
{}

std::string
ppstate_output::heartbeat()
{
  assert(_parent->_cluster_buffer!=NULL);
  std::vector<PndTpcDigi*>* list=new std::vector<PndTpcDigi*>(_parent->_data);
  _parent->_cluster_buffer->push_back(list);
  _parent->_data.clear();
  _parent->setState("end");
  return "end";
}

