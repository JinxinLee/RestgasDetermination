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

#ifndef TPCSPHIT_HH
#define TPCSPHIT_HH

// Base Class Headers ----------------
#include "RecoHitIfc.h"
#include "SpacepointHitPolicy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class FairMCPoint;
class PndTpcCluster;

typedef RecoHitIfc<SpacepointHitPolicy> SpacepointRecoHit;

class PndTpcSPHit : public SpacepointRecoHit {

public:

  // Constructors/Destructors ---------
  PndTpcSPHit();
  PndTpcSPHit(double x, double y, double z,
	      double sigx, double sigy, double sigz);
  PndTpcSPHit(const TVector3& pos,
	      const TVector3& sig);
  PndTpcSPHit(FairMCPoint* point);
  PndTpcSPHit(PndTpcCluster* cluster);

  virtual ~PndTpcSPHit();

  virtual AbsRecoHit* clone();
  
  // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);

  double amp() {return _amp;}

  PndTpcCluster* getCluster(){return _cluster;}
  
private:

  // Private Data Members ------------
  static const int NparHitRep = 3;
  double _amp;
  PndTpcCluster* _cluster;

  // Private Methods -----------------

public:
  ClassDef(PndTpcSPHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
