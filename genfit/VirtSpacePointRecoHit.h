//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      a virtual space point reco hit
//      this hit does not acknowledge any special track rep
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

#ifndef VIRTSPACEPOINTRECOHIT_HH
#define VIRTSPACEPOINTRECOHIT_HH

// Base Class Headers ----------------
#include "RecoHitIfc.h"
#include "SpacepointHitPolicy.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --


typedef RecoHitIfc<SpacepointHitPolicy> SpacepointRecoHit;

class VirtSpacePointRecoHit : public SpacepointRecoHit {
public:
 
  // Constructors/Destructors ---------
  VirtSpacePointRecoHit();
  VirtSpacePointRecoHit(double x, double y, double z);
  VirtSpacePointRecoHit(const TVector3& pos);

  virtual ~VirtSpacePointRecoHit();

  virtual AbsRecoHit* clone();

  // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);




private:

  // Private Data Members ------------
  static const int NparHitRep = 3;

  // Private Methods -----------------

public:
  ClassDef(VirtSpacePointRecoHit,1)


};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
