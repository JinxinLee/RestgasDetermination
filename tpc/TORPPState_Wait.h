//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      wait state 
//      waits for data from other pads
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PPSTATE_WAIT_HH
#define PPSTATE_WAIT_HH

// Base Class Headers ----------------
#include "TORPPState.h"

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class padprocessor;


class ppstate_wait : public ppstate {
public:

  // Constructors/Destructors ---------
  ppstate_wait(padprocessor* pp);
  virtual ~ppstate_wait(){;}

  // Operators
 

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual std::string heartbeat();

private:

  // Private Data Members ------------
  int _ndata;
  int _counter;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
