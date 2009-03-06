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

//! Base Class for GENFIT Track representations
/*! All Track Representations must inherit from this class to be available in genfit
Algorithms in GENFIT use this class as interface to access track parameters
*/

#ifndef FITTER_ABSTRACKREP_H
#define FITTER_ABSTRACKREP_H

#include<vector>
#include<list>

#include "TMatrixT.h"
#include "TVector3.h"

#include "FitParams.h"
#include "DetPlane.h"

class AbsRecoHit;    

class AbsTrackRep : public TObject{
 protected:
  //! The vector of track parameters
  TMatrixT<double> state;
  
  //! The covariance matrix
  TMatrixT<double> cov;   
  
  TMatrixT<double> startState; 
  TMatrixT<double> startCov;
  
  // detector plane where the track parameters are given
  DetPlane         _refPlane;
  
  //! deprecated
  double           startS; 
  
  //! chiSqu of the track fit
  double           chiSqu;

  //! book keeping for kalman filter
  FitParams        params; 

  //! status of track representation: 0 means everything's OK
  int statusFlag; 
  
  //! specifies the direction of flight of the particle
  bool inverted; 



 public:

  AbsTrackRep();
  AbsTrackRep(int);
  virtual ~AbsTrackRep();

  virtual AbsTrackRep* clone() const = 0;

  virtual AbsTrackRep* prototype() const = 0;

  // TODO: make the following method abstract!!
  virtual void init(const TVector3& startpoint, const TVector3& startmomentum, const TVector3& locBField);

  virtual DetPlane getVirtualDetPlane(const TVector3& hit)=0;

  virtual void predict(const DetPlane& plane,
		       TMatrixT<double>& statePred,
		       TMatrixT<double>& covPred,
		       TMatrixT<double>& jacobian)=0;

  //! returns the tracklength spanned in this extrapolation
  virtual double extrapolate(const DetPlane& plane, TMatrixT<double>& statePred)=0;

  virtual TVector3 extrapolateToPoca(const TVector3& point, 
				     TMatrixT<double>& statePred,
				     TMatrixT<double>& covPred,
				     DetPlane& planePred)=0;
  
  
  //! changes the state of the trkrep!
  virtual double extrapolate(const DetPlane& plane); 
  
  //! make step of h cm along the track
  virtual void stepalong(double h)=0; 

  //! Extrapolates the track to the given detetorplane
  /*! Results are put into statePred and covPred
      This method does NOT alter the state of the object!
   */ 
  virtual double extrapolate(const DetPlane& plane,
			   TMatrixT<double>& statePred,
			   TMatrixT<double>& covPred,
			   TMatrixT<double>& jacobian)=0;

  //! returns dimension of state vector
  virtual int getDim() const =0;  
  
  virtual void Print() const;

  //! Accessors:
  inline TMatrixT<double> getState() const {
    return state;
  }
  inline TMatrixT<double> getCov() const {
    return cov;
  }

  double getStateElem(int i) const {return state(i,0);}
  double getCovElem(int i, int j) const {return cov(i,j);}

  

  virtual TVector3 getPos(const DetPlane& pl)=0; 
  virtual TVector3 getMom(const DetPlane& pl)=0; 
  virtual void getPosMom(const DetPlane& pl,TVector3& pos,TVector3& mom)=0;
  virtual double getCharge()const =0;

  TVector3 getPos() {return getPos(_refPlane);}
  TVector3 getMom() {return getMom(_refPlane);}

  inline TMatrixT<double> getStartState() const {
    return startState;
  }
  inline TMatrixT<double> getStartCov() const {
    return startCov;
  }
  //inline double getS() const {
  //  return s;
  //}
  inline double getStartS() const {
    return startS;
  }
  inline double getChiSqu() const {
    return chiSqu;
  }
  inline FitParams* getFitParams() {
  	return &params;
  }

  inline void setState(const TMatrixT<double>& aState) {
    state = aState;
  }
  inline void setCov(const TMatrixT<double>& aCov) {
    cov = aCov;
  }
  inline void setStartState(const TMatrixT<double>& aState) {
    startState = aState;
  }
  inline void setStartCov(const TMatrixT<double>& aCov) {
    startCov = aCov;
  }
  virtual void setReferencePlane(const DetPlane& pl)=0;
  const DetPlane& getReferencePlane() const {return _refPlane;}

  inline void setStartS(double anS) {
    startS = anS;
  }
  inline void setChiSqu(double aChiSqu) {
    chiSqu = aChiSqu;
  }
  inline void addChiSqu(double aChiSqu) {
    chiSqu += aChiSqu;
  }
  inline void setStatusFlag(int _val) {
	statusFlag = _val;
  }
  
  virtual void switchDirection() = 0;
  bool setInverted(bool f=true){inverted=f;}
  
  inline bool getStatusFlag() {
	return statusFlag;
  }

  virtual void reset();


  ClassDef(AbsTrackRep,2)

};



#endif
