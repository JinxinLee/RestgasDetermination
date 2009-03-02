///////
/** Abstract Base Class for Hit Representations in GENFIT
 */
///////

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

  virtual double residualScalar(AbsTrackRep* stateVector,
				const TMatrixT<double>& state)=0;

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
