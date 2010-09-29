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

#ifndef TPCSAMPLEAMP_HH
#define TPCSAMPLEAMP_HH

class PndTpcSample;

class PndTpcSampleAmp {
public:

  // Constructors/Destructors ---------
  PndTpcSampleAmp(){;}
  ~PndTpcSampleAmp(){;}

  // Operators
  bool operator()(PndTpcSample* s1, PndTpcSample* s2);
  
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
