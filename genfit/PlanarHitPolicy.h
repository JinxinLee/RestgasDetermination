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

#ifndef PLANARHITPOLICY_H
#define PLANARHITPOLICY_H


#include "TMatrixT.h"

#include "DetPlane.h"

class AbsRecoHit;
class AbsTrackRep;

/** @brief Policy class implementing a planar hit geometry. 
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
