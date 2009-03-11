//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      TrackRep for fit in conformal space
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

#ifndef TPCCONFMAPFIT_HH
#define TPCCONFMAPFIT_HH

// Base Class Headers ----------------
#include "AbsTrackRep.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "TVector2.h"

// Collaborating Class Declarations --



class PndTpcConfMapFit : public AbsTrackRep {
public:

  // Constructors/Destructors ---------
  PndTpcConfMapFit();
  ~PndTpcConfMapFit(){;}

  virtual AbsTrackRep* clone()const;
  virtual AbsTrackRep* prototype()const;

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual double extrapolate(const DetPlane& plane, TMatrixT<double>& statePred);
  
  virtual double extrapolate(const DetPlane& plane,
			     TMatrixT<double>& statePred,
			     TMatrixT<double>& covPred);

  
  virtual void stepalong(double h);

  virtual TVector3 getPos(const DetPlane& pl);
  virtual TVector3 getMom(const DetPlane& pl);
  void getPosMom(const DetPlane& pl,TVector3& pos,TVector3& mom){}
  virtual double getCharge() const {return 1;}
  const TVector2& getOrigin() const {return _origin;}

  virtual void setReferencePlane(const DetPlane& pl);
  void setOrigin(double x, double y) {_origin.Set(x,y);}

  double getR2();
  double getR(); // returns R from conformal mapping fit

  void setRotated(bool flag=true);
  bool isRotated() const {return _isRotated;}
  virtual void switchDirection(){};
  
private:
  
  // Private Data Members ------------
  bool _isRotated;

  TVector2 _origin; // origin of conformal map transform
  
  // Private Methods -----------------

};


#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
