//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      compare state 
//      compares pad amp with neighbour amps
//      defines center
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

#ifndef PPSTATE_COMPARE_HH
#define PPSTATE_COMPARE_HH

// Base Class Headers ----------------
#include "TORPPState.h"

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class padprocessor;


class ppstate_compare : public ppstate {
public:

  // Constructors/Destructors ---------
  ppstate_compare(padprocessor* pp);
  virtual ~ppstate_compare(){;}

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
