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

#ifndef DEMORECOHIT_HH
#define DEMORECOHIT_HH

// Base Class Headers ----------------
#include "GFRecoHitIfc.h"
#include "GFPlanarHitPolicy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class FairMCPoint;

typedef GFRecoHitIfc<GFPlanarHitPolicy> PlanarRecoHit;

class DemoRecoHit : public PlanarRecoHit {
public:

  // Constructors/Destructors ---------
  DemoRecoHit();
  DemoRecoHit(double x, double y, double z,
	       double sigx, double sigy);
  DemoRecoHit(const TVector3& pos,
	       double sigx, double sigy);
  DemoRecoHit(FairMCPoint* point);

  virtual ~DemoRecoHit();

  virtual GFAbsRecoHit* clone();
  
  // Operations ----------------------
  virtual TMatrixT<double> getHMatrix(const GFAbsTrackRep* stateVector);


  
private:

  // Private Data Members ------------
  static const int NparHitRep = 2;

  // Private Methods -----------------

public:
  ClassDef(DemoRecoHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
