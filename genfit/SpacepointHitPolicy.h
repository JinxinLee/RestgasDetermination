/** @addtogroup genfit
 * @{
 */

#ifndef SPACEPOINTHITPOLICY_H
#define SPACEPOINTHITPOLICY_H


#include "TMatrixT.h"

#include "DetPlane.h"

class AbsRecoHit;
class AbsTrackRep;

/** @brief Policy class implementing a space point hit geometry. 
 *
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 * RecoHits for detectors measuring 3D space points should inherit 
 * from RecoHitIfc<SpacepointHitPolicy>.
 *
 * For a space point the detector plane has to be defined with respect to
 * a track representation. SpacepointHitPolicy implements a scheme where the
 * detectorplane is chosen perpendicular to the track.
 * In a track fit only 2 of the three coordinates of a space point are 
 * independent (the track is a one-dimensional object). Therefore the 3D
 * data of the hit is used to define a proper detector plane into which the
 * hit coordinates are then projected.
 */

class SpacepointHitPolicy {
public:


  SpacepointHitPolicy(){;}
  
  // Operations ----------------------
   /** @brief Get detector plane perpendicular to track.
    *
    * The detector plane is contructed from the position of the hit and
    * the track representation. For this the track is extrapolated to the
    * point of closest approach to the hit.
    */
  const DetPlane& detPlane(AbsRecoHit*, AbsTrackRep*);

  /** @brief Hit coordinates in detector plane.
   */
  TMatrixT<double> hitCoord(AbsRecoHit*,const DetPlane&);

  /** @brief Hit covariances in detector plane.
   */
  TMatrixT<double> hitCov(AbsRecoHit*,const DetPlane&);

protected:
  /** @brief policy destructors have to be protected.
   * see Alexandrescu, 2004
   */
  virtual ~SpacepointHitPolicy(){;}

private:

  // Private Data Members ------------
  DetPlane _plane;
  
  // Private Methods -----------------

};

#endif

/* @} **/
