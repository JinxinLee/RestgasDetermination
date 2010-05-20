//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Functor for comparison of pointers to PndTpcSample
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Maxence Vandenbroucke    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCSAMPLEAGE_HH
#define TPCSAMPLEAGE_HH

class PndTpcSample;

class PndTpcSampleAge {
public:

  // Constructors/Destructors ---------
  PndTpcSampleAge(){;}
  ~PndTpcSampleAge(){;}

  // Operators
  bool operator()(PndTpcSample* s1, PndTpcSample* s2);
  
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
