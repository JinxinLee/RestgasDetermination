//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      initial state 
//      waits for data from frontend
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

#ifndef PPSTATE_INITIAL_HH
#define PPSTATE_INITIAL_HH

// Base Class Headers ----------------
#include "TORPPState.h"

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class padprocessor;


class ppstate_initial : public ppstate {
public:

  // Constructors/Destructors ---------
  ppstate_initial(padprocessor* pp);
  virtual ~ppstate_initial(){;}

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
