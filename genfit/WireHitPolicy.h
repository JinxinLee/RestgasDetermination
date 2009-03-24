/** @addtogroup genfit
 * @{ */



#ifndef WIREHITPOLICY_H
#define WIREHITPOLICY_H

#include "TMatrixT.h"

#include "DetPlane.h"

class AbsRecoHit;
class AbsTrackRep;

/** @brief policy class for hits in wire detectors (STT and DCH) 
 *  which do not measure the coordinate along the wire
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Lia Lavezzi (INFN Pavia roiginal author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 *
 * This policy is not valid for any kind of plane orientation
 * choice: to use it you MUST choose a plane described by u 
 * and v axes with v coincident with the wire (and u orthogonal
 * to it, obviously).
 * The hit will be described by 7 coordinates:
 * w_x1, w_y1, w_z1, w_x2, w_y2, w_z2, rdrift
 * where w_ji (with j = x, y, z and i = 1, 2) are the wire
 * extremities coordinates; rdrift = distance from the wire (u 
 * coordinate in the plane)
 *
 * to see an example of this reco hit implementation see PndDchRecoHit2
 */
class WireHitPolicy {
public:


  WireHitPolicy();
 
  // Operations ----------------------
   /** @brief Get detector plane 
    */
  const DetPlane& detPlane(AbsRecoHit*, AbsTrackRep*);

  /** @brief Hit coordinates in detector plane.
   */
  TMatrixT<double> hitCoord(AbsRecoHit*,const DetPlane&);

  /** @brief Hit covariances in detector plane.
   */
  TMatrixT<double> hitCov(AbsRecoHit*,const DetPlane&);

  /** @brief Check if the detector plane is valid
   */
  void checkPlane(AbsRecoHit*,const DetPlane&);

protected:
  // policy destructors have to be protected
  // see Alexandrescu
  virtual ~WireHitPolicy(){;}

  /**
     maximum acceptable distance from the wire
  */
  double _maxdistance;
  
private:

  // Private Data Members ------------
  DetPlane _detPlane;
  // Private Methods -----------------

};

#endif

/** @} */
