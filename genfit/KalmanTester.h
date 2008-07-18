//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      A Tester class for the Kalman filter (see Kalman.h)
//      friend of class Kalman
//      Use this to write tests for the Kalman!
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

#ifndef KALMANTESTER_HH
#define KALMANTESTER_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------


// Collaborating Class Declarations --



class KalmanTester {
public:

  // Constructors/Destructors ---------
  KalmanTester(){;}
  ~KalmanTester(){;}

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  void test_gain();
  void test_processHit();
  void test_processTrack();


private:

  // Private Data Members ------------


  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
