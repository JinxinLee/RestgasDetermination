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
#include "GFAbsTrackRep.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "TMath.h"


class PndTpcZSFit : public GFAbsTrackRep {
public:

  // Constructors/Destructors ---------
  PndTpcZSFit();
  ~PndTpcZSFit(){;}

  virtual GFAbsTrackRep* clone()const;
  virtual GFAbsTrackRep* prototype()const;

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual double extrapolate(const GFDetPlane& plane, TMatrixT<double>& statePred);
  
  virtual double extrapolate(const GFDetPlane& plane,
			     TMatrixT<double>& statePred,
			     TMatrixT<double>& covPred);



  virtual TVector3 getPos(const GFDetPlane& pl);
  virtual TVector3 getMom(const GFDetPlane& pl);
  void getPosMom(const GFDetPlane& pl,TVector3& pos,TVector3& mom){}
  virtual double getCharge() const {return 1;}

  double getDip() const {return TMath::Cos(TMath::ATan(fState[0][0]));} // pt=p*dip

  virtual void setReferencePlane(const GFDetPlane& pl);
  virtual void switchDirection(){};
private:
  
  // Private Data Members ------------
  
  
  // Private Methods -----------------

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
