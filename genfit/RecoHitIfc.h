/** @addtogroup genfit
 * @{
 */

#ifndef RECOHITIFC_H
#define RECOHITIFC_H


#include "TMatrixT.h"

#include "AbsRecoHit.h"
#include "DetPlane.h"


/** @brief RecoHit interface template class. Provides comfortable 
 * interface to create RecoHits
 *
 *  @author Christian H&ouml;ppner (Technische Universit&auml;t M&uuml;nchen, original author)
 *  @author Sebastian Neubert  (Technische Universit&auml;t M&uuml;nchen, original author)
 * 
 * This class defines a comfortable interface to create hit classes in genfit.
 * It is a template class. The template parameter is used to specify a certain
 * basic type of hit:
 *  - RecoHitIfc<PlanarHitPolicy> a basic planar hit
 *  - RecoHitIfc<SpacepointHitPolicy> a basic space point hit
 *  - RecoHitIfc<WirepointHitPolicy> a basic hit on a wire
 *
 * To create a hit for a detector simply inherit from one of the options 
 * above and fill in your data. For details look at the respective 
 * HitPolicy documentations. You can also directly inherit from 
 * AbsRecoHit though this is not recommended. If a new hit geometry is needed
 * One should think about implementing a new HitPolicy for this type of hit.
 *
 * @sa PlanarHitPolicy
 * @sa SpacepointHitPolicy
 * @sa WirepointHitPolicy
 *
 * Implementation details: The actual implementations of the methods 
 * declared here can be found in the HitPolicy objects. Since RecoHitIfc
 * inhertis from its template parameter class detplane_policy the methods
 * defined therein become part of the sepcific RecoHitIfc. To learn more about
 * Policy classes we recommend the book: 
 * A.Alexandrescu, "Modern C++ Design", Addison-Wesley 2004
 */
template<class HitPolicy>
class RecoHitIfc : public AbsRecoHit, public HitPolicy{
 public:
  
  /** @brief Constructor specifying dimension of hit coordinate vector
   */
  RecoHitIfc(int dim) : AbsRecoHit(dim){;}
  virtual ~RecoHitIfc(){;}

  /** @brief Returns the detector plane object for this hit and a given track
   * representation.
   *
   * The actutal code for this method depends on the hit geometry and is 
   * implemented in the HitPolicy
   * @sa PlanarHitPolicy
   * @sa SpacepointHitPolicy
   * @sa WirepointHitPolicy
   */
  virtual const DetPlane& getDetPlane(AbsTrackRep* rep){return detPlane(this,rep);}

  /** @brief Get hit coordinates in a specific detector plane
   *
   * Implementation in the HitPolicy
   */
  virtual TMatrixT<double> getHitCoord(const DetPlane& plane)
  {return hitCoord(this,plane);}

  /** @brief Get hit covariances in a specific detector plane
   *
   * Implementation in the HitPolicy
   */
  virtual TMatrixT<double> getHitCov(const DetPlane& plane)
  {return hitCov(this,plane);}
};

#endif


/* @} **/


