/*  Copyright © 2008, Christian Hoeppner & Sebastian Neubert
    All rights reserved.

    This file is part of Genfit.

    Genfit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Genfit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Public License for more details.

    You should have received a copy of the GNU Lesser Public License
    along with Genfit.  If not, see <http://www.gnu.org/licenses/>. */

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


