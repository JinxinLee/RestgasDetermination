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

#ifndef DEMOSPHIT_HH
#define DEMOSPHIT_HH

// Base Class Headers ----------------
#include "RecoHitIfc.h"
#include "SpacepointHitPolicy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class FairMCPoint;

typedef RecoHitIfc<SpacepointHitPolicy> SpacepointRecoHit;

class DemoSPHit : public SpacepointRecoHit {
public:

  // Constructors/Destructors ---------
  DemoSPHit();
  DemoSPHit(double x, double y, double z,
	       double sigx, double sigy, double sigz);
  DemoSPHit(const TVector3& pos,
	    const TVector3& sig);
  DemoSPHit(FairMCPoint* point);

  virtual ~DemoSPHit();

  virtual AbsRecoHit* clone();
  
  // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);


  virtual double residualScalar(AbsTrackRep* stateVector,
                                const TMatrixT<double>& state);

  
private:

  // Private Data Members ------------
  static const int NparHitRep = 3;

  // Private Methods -----------------

public:
  ClassDef(DemoSPHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
