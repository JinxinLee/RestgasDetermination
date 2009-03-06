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
// File and Version Information:
// $Id$
//
// Description:
//      a local straight line track representation
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef LSLTRACKREP_HH
#define LSLTRACKREP_HH

// Base Class Headers ----------------
#include "AbsTrackRep.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "TVectorT.h"
#include "TMatrixT.h"


// Collaborating Class Declarations --
class AbsBFieldIfc;
class AbsNystromEQM;

class LSLTrackRep : public AbsTrackRep {
public:

  // Constructors/Destructors ---------
  LSLTrackRep();
  LSLTrackRep(double z, double x, double y, 
	      double dxdz, double dydz, double invp,
	      double sigx, double sigy, 
	      double sigdxdz, double sigdydz, 
	      double siginvp, 
	      AbsBFieldIfc* field);
  LSLTrackRep(const LSLTrackRep& rep); // copy ctor

  virtual ~LSLTrackRep();


  virtual AbsTrackRep* clone()const {return new LSLTrackRep(*this);}

  virtual AbsTrackRep* prototype()const{return new LSLTrackRep();}

  // Operators
  friend std::ostream& operator<< (std::ostream& s, const LSLTrackRep& me);

  // Accessors -----------------------
  virtual int getDim() const {return 5;}
  void init(const TVector3& pos,
	    double dxdz, double dydz, double invp,
	    double sigx, double sigy,
	    double sigdxdz, double sigdydz, 
	    double siginvp,
	    AbsBFieldIfc* field);

  // Modifiers
  void SetBField(AbsBFieldIfc* b);
  void setReferencePlane(const DetPlane& pl) {s=pl.getO().Z();
                                              _refPlane=pl;}
  void setRungeKuttaAcc(double acc){_acc=acc;}
  void setRungeKuttaAdaptive(bool f){_adaptive=f;}
  // Operations ----------------------
  virtual void predict(const DetPlane&,TMatrixT<double>& statePred,
		       TMatrixT<double>& covPred,TMatrixT<double>& jacobian);
  virtual double extrapolate(const DetPlane&, TMatrixT<double>& statePred);
  //virtual void extrapolate(const DetPlane&, 
  //			   const TMatrixT<double>& stateFrom, 
  //			   TMatrixT<double>& stateResult);

  virtual double extrapolate(const DetPlane&, 
			   TMatrixT<double>& statePred,
			   TMatrixT<double>& covPred,
			   TMatrixT<double>& jacobian);

  virtual TVector3 extrapolateToPoca(const TVector3&, 
				     TMatrixT<double>& statePred,
				     TMatrixT<double>& covPred,
				     DetPlane& planePred);


  virtual void stepalong(double h);

  virtual DetPlane getVirtualDetPlane(const TVector3& hit);

  virtual TVector3 getPos(const DetPlane&) ;
  virtual TVector3 getMom(const DetPlane&) ;
  virtual void getPosMom(const DetPlane&,TVector3& pos, TVector3& mom) ;
  virtual TVectorT<double> getGlobal(); // (x,y,z,px,py,pz)
  virtual TMatrixT<double> getGlobalCov(); // covariances

  virtual double getCharge()const {double fact= inverted ? -1.:1.;
    return state[4][0]<0 ? -fact : fact;}

  virtual void switchDirection(){};
  
private:

  // Private Data Members ------------

  // Private Methods -----------------
  // equation of motion in a magnetic field
  //static TVectorT<double> eqm(const TVectorT<double>& u,
  //			      const TVectorT<double>& uprim,
  //			      const TVectorT<double>& par);

  double s;

  double _acc; // accuracy of runge kutta; default: 1E-2cm
  bool _adaptive;

  AbsNystromEQM* _eqm; //!

  // calculate jacobian of extrapolation
  void Jacobian(const DetPlane& pl,
		const TMatrixT<double>& statePred,
		TMatrixT<double>& jacResult);

 public:
  ClassDef(LSLTrackRep,1)

};


#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
