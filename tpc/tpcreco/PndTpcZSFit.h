//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      TrackRep for fit in SZ-plane
//      straight line fit
//      _state[0][0]=slope
//      _state[1][0]=offset
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCZSFIT_HH
#define TPCZSFIT_HH

// Base Class Headers ----------------
#include "AbsTrackRep.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "TMath.h"


class PndTpcZSFit : public AbsTrackRep {
public:

  // Constructors/Destructors ---------
  PndTpcZSFit();
  ~PndTpcZSFit(){;}

  virtual AbsTrackRep* clone()const;
  virtual AbsTrackRep* prototype()const;

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual DetPlane getVirtualDetPlane(const TVector3&){throw;}
  

  virtual double extrapolate(const DetPlane& plane, TMatrixT<double>& statePred);
  
  virtual double extrapolate(const DetPlane& plane,
                           TMatrixT<double>& statePred,
                           TMatrixT<double>& covPred,
                           TMatrixT<double>& jacobian);


  virtual void stepalong(double h);

  virtual TVector3 getPos(const DetPlane& pl);
  virtual TVector3 getMom(const DetPlane& pl);
  void getPosMom(const DetPlane& pl,TVector3& pos,TVector3& mom){}
  virtual double getCharge() const {return 1;}
  virtual int getDim() const {return 2;}

  double getDip() const {return TMath::Cos(TMath::ATan(state[0][0]));} // pt=p*dip

  virtual void setReferencePlane(const DetPlane& pl);
  virtual void switchDirection(){};
private:
  
  // Private Data Members ------------
  
  
  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
