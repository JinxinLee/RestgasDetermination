/**
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 */
#ifndef VIRTSPACEPOINTRECOHIT_H
#define VIRTSPACEPOINTRECOHIT_H

#include "RecoHitIfc.h"
#include "SpacepointHitPolicy.h"

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

