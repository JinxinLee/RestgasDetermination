/** @addtogroup genfit
 * @{
 */

#ifndef PLANARHITPOLICY_H
#define PLANARHITPOLICY_H


#include "TMatrixT.h"

#include "DetPlane.h"

class AbsRecoHit;
class AbsTrackRep;

/** @brief Policy class implementing a planar hit geometry. 
 *
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 * RecoHits for planar detectors should inherit 
 * from RecoHitIfc<PlanarHitPolicy>.
 *
 * The main feature of this type of hit is, that the detector plane
 * is completely defined by the detector hardware. Derived RecoHits need only
 * to supply the physical detector plane from their geometry database.
 */

class PlanarHitPolicy {
public:

  // Constructors/Destructors ---------
  PlanarHitPolicy(){;}
  

  // Accessors -----------------------
  
  /** @brief Returns the physical detector plane.
   */
  const DetPlane& detPlane(AbsRecoHit*,const AbsTrackRep*);
  

  // Modifiers -----------------------

  /** @brief Set physical detector plane. 
   * Needs to be called before hit can be used.
   *
   * For the planar detector the detector plane is fixed by the geometry of the
   * detector hardware. This method should be called in the constructor of
   * any derived RecoHit in order to setup the geometry of this hit.
   */
  void setDetPlane(const DetPlane& p){_physicalDetPlane=p;}

  // Operations ----------------------
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
  virtual ~PlanarHitPolicy(){;}


private:

  // Private Data Members ------------
  
  /** @brief Physical detector plane. Given by detector hardware.
   */
  DetPlane _physicalDetPlane;

  // Private Methods -----------------

};

#endif

/* @} **/
