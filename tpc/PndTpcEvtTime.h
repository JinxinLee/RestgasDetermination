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
  PndTpcEvtTime(double T0, unsigned int Id):ft0(T0),fid(Id){}
  ~PndTpcEvtTime(){}

  // Operators
 

  // Accessors -----------------------
  double t0() const {return ft0;}
  unsigned int id() const {return fid;}

  // Modifiers -----------------------


  // Operations ----------------------

private:

  // Private Data Members ------------
  double ft0;
  unsigned int fid;

  // Private Methods -----------------

public:
  ClassDef(PndTpcEvtTime,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
