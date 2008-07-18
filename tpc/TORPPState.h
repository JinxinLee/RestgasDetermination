//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      abstract base class for padprocessor states
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

#ifndef PPSTATE_HH
#define PPSTATE_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <string>

// Collaborating Class Declarations --
class padprocessor;


class ppstate {
public:

  // Constructors/Destructors ---------
  ppstate(padprocessor* const parent) : _parent(parent){}
  virtual ~ppstate(){;}

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual std::string heartbeat() = 0;

protected:

  // Protected Data Members ------------
  padprocessor* _parent;

private:

  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
