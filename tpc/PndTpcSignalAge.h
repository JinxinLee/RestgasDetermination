//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Functor for comparison of pointers to PndTpcSignals
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

#ifndef TPCSIGNALAGE_HH
#define TPCSIGNALAGE_HH

class PndTpcSignal;

class PndTpcSignalAge {
public:

  // Constructors/Destructors ---------
  PndTpcSignalAge(){;}
  ~PndTpcSignalAge(){;}

  // Operators
  bool operator()(PndTpcSignal* s1, PndTpcSignal* s2);
  
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
