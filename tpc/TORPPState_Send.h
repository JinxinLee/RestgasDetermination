//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      send state 
//      sends data to dominant neighbour pad
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

#ifndef PPSTATE_SEND_HH
#define PPSTATE_SEND_HH

// Base Class Headers ----------------
#include "TORPPState.h"

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class padprocessor;


class ppstate_send : public ppstate {
public:

  // Constructors/Destructors ---------
  ppstate_send(padprocessor* pp);
  virtual ~ppstate_send(){;}

  // Operators
 

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual std::string heartbeat();

private:

  // Private Data Members ------------
  int _c;

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
