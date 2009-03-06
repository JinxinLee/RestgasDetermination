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

/** Abstract Base Class for Hit Representations in GENFIT
 */


#ifndef FITTER_ABSHIT_H
#define FITTER_ABSHIT_H

#include<iostream>

#include "TMatrixT.h"
#include "TObject.h"
#include "AbsTrackRep.h"
#include "DetPlane.h"

    
class AbsRecoHit : public TObject{
protected:
  /// Transformation from TrackRep frame to Hit coordinates
  /// track_in_detplane=_HMatrix*_track_state
  TMatrixT<double> _HMatrix; 

  /// Vector of coordinates of hit given in DetPlane
  TMatrixT<double> _hitCoord;

  /// Covariance of hit coordinates in DetPlane
  TMatrixT<double> _hitCov;

public:
  virtual ~AbsRecoHit();
  AbsRecoHit(int NparHit);
  AbsRecoHit();
  virtual void setHMatrix(const AbsTrackRep* stateVector,
			  const TMatrixT<double>& state)=0;
  
  inline TMatrixT<double> getHMatrix() const {return _HMatrix;}
  
  virtual TMatrixT<double> residualVector(AbsTrackRep* stateVector,
					  const TMatrixT<double>& state) {
    setHMatrix(stateVector,state);
	DetPlane d = getDetPlane(stateVector);
    return ( getHitCoord(d) - (_HMatrix*state ));
  }


  TMatrixT<double> getRawHitCov() const {return _hitCov;}
  TMatrixT<double> getRawHitCoord() const {return _hitCoord;}

  virtual TMatrixT<double> getHitCov(const DetPlane&)=0;
  virtual TMatrixT<double> getHitCoord(const DetPlane&)=0;
  
  // this will call the policy stuff in derived classes
  virtual const DetPlane& getDetPlane(AbsTrackRep*)=0; 
   
  virtual AbsRecoHit* clone() = 0;
  virtual void Print() {_hitCoord.Print();}

  ClassDef(AbsRecoHit,1)

};
  

#endif //FITTER_ABSHIT_H
