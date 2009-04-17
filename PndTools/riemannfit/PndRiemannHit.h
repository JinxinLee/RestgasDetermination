//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Conformal Mapping of a PndTpc Cluster (x,y)->(r,phi)-> riemann sphere
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

#ifndef PNDRIEMANNHIT_HH
#define PNDRIEMANNHIT_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "TVector3.h"
#include "TMatrixD.h"
#include "FairHit.h"

// Collaborating Class Declarations --
class PndTpcCluster;
class PndRiemannTrack;

class PndRiemannHit : public TObject{
public:

  // Constructors/Destructors ---------
  PndRiemannHit() : fCovX(3,3){};
  PndRiemannHit(double r,double phi, double z = 0);
  PndRiemannHit(FairHit* cl);
  ~PndRiemannHit();


  // Accessors -----------------------
  void setXYZ(double x, double y, double z);
  void setDXYZ(double dx, double dy, double dz);
  void setHit(FairHit* cl);
  const TVector3& x() const {return fX;}
  const FairHit* hit() const {return fHit;}
  double s() const {return fS;}
  double z() const;
  double alpha() const {return fAlpha;}
  double sigmaXY() const;
  double sigmaX() const{return fCovX[0][0];}
  double sigmaY() const{return fCovX[1][1];}
  const TMatrixD& covX() const {return fCovX;}
  const double covX(int row, int col) const {return fCovX[row][col];}

  // Modifiers -----------------------


  // Operations ----------------------
  void calcPosOnTrk(PndRiemannTrack* trk);

private:

  // Private Data Members ------------
  TVector3 fX;
  TVector3 fSigmaX;			///< Sigma Error of fX (should be replaced by covariance matrix)
  TMatrixD fCovX;			///< Covariance Matrix of the hit
  FairHit* fHit; 			//->  //no ownership over this pointer!
  int fHitID;
  double fS;     			///< pathlength along track
  double fZ;				///< z-coordinate of hit
  double fDeltaZ;			///< error in z
  double fAlpha; 			///< angle along track

  int fVerbose;

  // Private Methods -----------------

public:
  ClassDef(PndRiemannHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
