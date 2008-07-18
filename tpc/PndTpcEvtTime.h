//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      simple class to hold the eventtime for event mixing
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

#ifndef PNDTPCEVTTIME_HH
#define PNDTPCEVTTIME_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------

// Collaborating Class Declarations --



class PndTpcEvtTime : public TObject{
public:

  // Constructors/Destructors ---------
  PndTpcEvtTime(){}
  PndTpcEvtTime(double t0, unsigned int id):_t0(t0),_id(id){}
  ~PndTpcEvtTime(){}

  // Operators
 

  // Accessors -----------------------
  double t0() const {return _t0;}
  unsigned int id() const {return _id;}

  // Modifiers -----------------------


  // Operations ----------------------

private:

  // Private Data Members ------------
  double _t0;
  unsigned int _id;

  // Private Methods -----------------

public:
  ClassDef(PndTpcEvtTime,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
