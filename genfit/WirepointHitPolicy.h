// policy class for hits in wire detectors (STT and DCH)
//
// ************************ WARNING ************************
// This policy is not valid for any kind of plane orientation
// choice: to use it you MUST choose a plane described by u 
// and v axes with v coincident with the wire (and u orthogonal
// to it, obviously).
// The hit will be described by 8 coordinates:
// w_x1, w_y1, w_z1, w_x2, w_y2, w_z2, rdrift, zreco
// where w_ji (with j = x, y, z and i = 1, 2) are the wire
// extremities coordinates; rdrift = distance from the wire (u 
// coordinate in the plane) and zreco = coordinate along the
// wire (in the plane reference frame, v coordinate).
//
// [to see an example of this reco hit implementation see PndSttRecoHit]
//

#ifndef WIREPOINTHITPOLICY_H
#define WIREPOINTHITPOLICY_H

#include "TMatrixT.h"

#include "DetPlane.h"

class AbsRecoHit;
class AbsTrackRep;

class WirepointHitPolicy {
public:


  WirepointHitPolicy(){;}
 
  // Operations ----------------------
  const DetPlane& detPlane(AbsRecoHit*, AbsTrackRep*);
  TMatrixT<double> hitCoord(AbsRecoHit*,const DetPlane&);
  TMatrixT<double> hitCov(AbsRecoHit*,const DetPlane&);
  void checkPlane(AbsRecoHit*,const DetPlane&);

protected:
  // policy destructors have to be protected
  // see Alexandrescu
  virtual ~WirepointHitPolicy(){;}


private:

  // Private Data Members ------------
 
  // Private Methods -----------------

};

#endif

