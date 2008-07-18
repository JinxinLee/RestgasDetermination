//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Functor for comparison of pointers to PndTpcDigis
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

#ifndef TPCDIGIAGE_HH
#define TPCDIGIAGE_HH

class PndTpcDigi;

class PndTpcDigiAge {
public:

  // Constructors/Destructors ---------
  PndTpcDigiAge(){;}
  ~PndTpcDigiAge(){;}

  // Operators
  bool operator()(PndTpcDigi* s1, PndTpcDigi* s2);
  
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
