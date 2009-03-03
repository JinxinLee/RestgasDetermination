//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      test reco hit
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

#ifndef TESTRECOHIT_HH
#define TESTRECOHIT_HH

// Base Class Headers ----------------
#include "RecoHitIfc.h"
#include "PlanarHitPolicy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --

typedef RecoHitIfc<PlanarHitPolicy> PlanarRecoHit;

class TestRecoHit : public PlanarRecoHit {
public:

  // Constructors/Destructors ---------
  TestRecoHit(): PlanarRecoHit(2){}
  virtual ~TestRecoHit(){}

  TestRecoHit(double u, double v, double z);
  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
			  const TMatrixT<double>& state);

  
  virtual AbsRecoHit* clone();


private:

  // Private Data Members ------------


  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
