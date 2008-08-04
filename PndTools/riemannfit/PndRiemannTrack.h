//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Track on Riemann Sphere
//      Circle parameters can be calculated from plane parameters
//      plane(c,nx,ny,nz);
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

#ifndef PNDRIEMANNTRACK_HH
#define PNDRIEMANNTRACK_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <vector>
#include "TVectorD.h"
#include "TMatrixD.h"

// Collaborating Class Declarations --
#include "PndRiemannHit.h"


class PndRiemannTrack : public TObject{
public:

  // Constructors/Destructors ---------
  PndRiemannTrack();
  ~PndRiemannTrack(){};

  
  // Accessors -----------------------
  const TVectorD& n() const {return fn;}
  double c() const {return fc;}
  const TVectorD& av() const {return fav;}

  TVectorD orig() const;
  double r() const;
  double dip();
  double sign() const;
  double getSZm() const {return fm;}
  double getSZt() const {return ft;}
  unsigned int getNumHits() {return fHits.size();}
  PndRiemannHit* getHit(unsigned int i) {PndRiemannHit* myHit = &(fHits[i]); return myHit;}
  PndRiemannHit* getLastHit() {return getHit(getNumHits()-1);}

  double m() const {return fm;}
  double t() const {return ft;}

  // Modifiers -----------------------
  void addHit(PndRiemannHit* hit);
  void init(double x0, double y0, double R, 
	    double dip, double z0);

  // Operations ----------------------
  void refit();
  double dist(PndRiemannHit* hit);
  void szFit();
  double calcSZChi2(PndRiemannHit* hit); //calculates the chi2 of the track plus the additional hit
  double szDist(PndRiemannHit* hit);
  double szError(PndRiemannHit* hit);
  double szChi2(){return fChi2;};
  
  void SetVerbose(int i){fVerbose = i;}

private:

  // Private Data Members ------------
  TVectorD fn;  		///< normal vector to plane;
  double fc;     		///< distance of plane to origin

  double fm; 			///< parameters of sz-fit
  double ft;
  double fmError;		///< Error of fit
  double ftError;		///< Error of fit
  double fChi2;			///< Chisquare of sz fit
  bool fFitDone;
  bool fSZFitDone;

  std::vector<PndRiemannHit> fHits;
  TVectorD fav;  		///< average over all hits
  double fweight; 		///< sum over all weights (1/(sigmaXY*sigmaXY))
  TMatrixD fcovPlane; 	///< full covarince matrix of the plane;
  TMatrixD fjacRXY;  	///< jacobian matrix to transform from c,n1,n2,n2 to r,x,y
  TMatrixD fcovRXY;
  int fVerbose;
  
  void calcJacRXY(); 	///< calcualtes fjacRXY


  // Private Methods -----------------
 

public:
  ClassDef(PndRiemannTrack,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
