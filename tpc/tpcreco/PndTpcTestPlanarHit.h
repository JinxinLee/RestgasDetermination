//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      an xyz reco hit (z being considered as a free parameter)
//      hitCoord=(x,y)^T
//      s=z
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TESTPLANARHIT_HH
#define TESTPLANARHIT_HH

// Base Class Headers ----------------
#include "GFRecoHitIfc.h"
#include "GFPlanarHitPolicy.h"

// Collaborating Class Headers -------

#include"TRandom3.h"

// Collaborating Class Declarations --


typedef GFRecoHitIfc<GFPlanarHitPolicy> PlanarRecoHit;

class PndTpcTestPlanarHit : public PlanarRecoHit {

public:

  // Constructors/Destructors ---------
  PndTpcTestPlanarHit();
  PndTpcTestPlanarHit(const GFDetPlane& pl,
		      double res);

  virtual ~PndTpcTestPlanarHit();

  virtual GFAbsRecoHit* clone();
  
  // Operations ----------------------
  virtual TMatrixT<double> getHMatrix(const GFAbsTrackRep* stateVector);

  
private:

  // Private Data Members ------------
  static const int NparHitRep = 2;
  static TRandom3 rand;
  // Private Methods -----------------

public:
  ClassDef(PndTpcTestPlanarHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
