//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      end state 
//      does nothing
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

#ifndef PPSTATE_END_HH
#define PPSTATE_END_HH

// Base Class Headers ----------------
#include "TORPPState.h"

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class padprocessor;


class ppstate_end : public ppstate {
public:

  // Constructors/Destructors ---------
  ppstate_end(padprocessor* pp);
  virtual ~ppstate_end(){;}

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
