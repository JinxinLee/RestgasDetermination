//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      output state 
//      outputs for data from other pads
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

#ifndef PPSTATE_OUTPUT_HH
#define PPSTATE_OUTPUT_HH

// Base Class Headers ----------------
#include "TORPPState.h"

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class padprocessor;


class ppstate_output : public ppstate {
public:

  // Constructors/Destructors ---------
  ppstate_output(padprocessor* pp);
  virtual ~ppstate_output(){;}

  // Operators
 

  // Accessors -----------------------
  

  // Modifiers -----------------------
  
  // Operations ----------------------
  virtual std::string heartbeat();

private:

  // Private Data Members ------------
  
  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
