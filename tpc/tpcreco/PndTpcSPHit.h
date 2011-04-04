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
#include "GFRecoHitIfc.h"
#include "GFSpacepointHitPolicy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

#include"TRandom3.h"

// Collaborating Class Declarations --
#ifndef STANDALONE
class FairMCPoint;
class PndTpcCluster;
#endif

typedef GFRecoHitIfc<GFSpacepointHitPolicy> SpacepointRecoHit;

class PndTpcSPHit : public SpacepointRecoHit {

public:

  // Constructors/Destructors ---------
  PndTpcSPHit();
  PndTpcSPHit(double x, double y, double z,
	      double sigx, double sigy, double sigz);
  PndTpcSPHit(const TVector3& pos,
	      const TVector3& sig,
	      bool smear=false);
 #ifndef STANDALONE
  PndTpcSPHit(FairMCPoint* point);
  PndTpcSPHit(PndTpcCluster* cluster);
#endif

  virtual ~PndTpcSPHit();

  virtual GFAbsRecoHit* clone();
  
  // Operations ----------------------
  virtual TMatrixT<double> getHMatrix(const GFAbsTrackRep* stateVector);

  double amp() {return _amp;}
#ifndef STANDALONE
  PndTpcCluster* getCluster(){return _cluster;}
  #endif
private:

  // Private Data Members ------------
  static const int NparHitRep = 3;
  double _amp;
#ifndef STANDALONE
  PndTpcCluster* _cluster;//!
#else
  void* _cluster;
#endif
  static TRandom3 rand;
  // Private Methods -----------------

public:
  ClassDef(PndTpcSPHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
