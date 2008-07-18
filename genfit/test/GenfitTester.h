//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Tester Class to test from ROOT Script
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

#ifndef GENFITTESTER_HH
#define GENFITTESTER_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --



class GenfitTester : public TObject {
public:

  // Constructors/Destructors ---------
  GenfitTester();
  virtual ~GenfitTester();

  
  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  void test_RecoHit();
  void test_locTrkRep();
  void test_DetPlane();
  void test_LSLTrkRep(double acc=1E-2, bool adaptive=true);
  void test_LSLtoGLOB();
  void test_PCA();

private:

  // Private Data Members ------------


  // Private Methods -----------------


public:
  ClassDef(GenfitTester,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
