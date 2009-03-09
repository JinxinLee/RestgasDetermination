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

#ifndef SPACEPOINTHITPOLICY_H
#define SPACEPOINTHITPOLICY_H


#include "TMatrixT.h"

#include "DetPlane.h"

class AbsRecoHit;
class AbsTrackRep;

/** @brief Policy class implementing a space point hit geometry. 
 * RecoHits for detectors measuring 3D space points should inherit 
 * from RecoHitIfc<SpacepointHitPolicy>.
 *
 * For a space point the detector plane has to be defined with respect to
 * a track representation. SpacepointHitPolicy implements a scheme where the
 * detectorplane is chosen perpendicular to the track.
 */

class P
class SpacepointHitPolicy {
public:


  SpacepointHitPolicy(){;}
  

  // Modifiers -----------------------

  /** @brief Set 3D positon of hit. Hit coordinates will be computed from this.
   *
   * In a track fit only 2 of the three coordinates of a space point are 
   * independent (the track is a one-dimensional object). Therefore the 3D
   * data of the hit is used to define a proper detector plane into which the
   * hit coordinates are then projected.
   */
  void setPos(const TVector3& p){_pos=p;}

  // Acessors ------------------------
  /** @brief Get raw hit positoin.
   */
  const TVector3& getPos() {return _pos;}

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

  /** @brief 3D position of the hit
   */
  TVector3 _pos; // position of spacepoint;

private:

  // Private Data Members ------------
  DetPlane _plane;
  
  // Private Methods -----------------

};

#endif

/* @} **/
