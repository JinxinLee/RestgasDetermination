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

//-----------------------------------------------------------
//
// WirepointHitPolicy class:
//
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
//-----------------------------------------------------------

#ifndef WIREPOINTHITPOLICY_HH
#define WIREPOINTHITPOLICY_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include "DetPlane.h"
#include "TMatrixT.h"

// Collaborating Class Declarations --
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

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
