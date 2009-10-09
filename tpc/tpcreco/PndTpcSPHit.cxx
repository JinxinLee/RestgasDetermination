//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcSPHit
//      see PndTpcSPHit.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcSPHit.h"

// C/C++ Headers ----------------------
#include <assert.h>

// Collaborating Class Headers --------
#include "FairMCPoint.h"
#include "PndTpcCluster.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "GFDetPlane.h"

// Class Member definitions -----------

ClassImp(PndTpcSPHit)


PndTpcSPHit::~PndTpcSPHit()
{}

PndTpcSPHit::PndTpcSPHit()
  : SpacepointRecoHit(NparHitRep), _amp(0.), _cluster(NULL)
{}

PndTpcSPHit::PndTpcSPHit(double x, double y, double z,
			 double sigx, double sigy, double sigz)
  : SpacepointRecoHit(NparHitRep), _amp(0.), _cluster(NULL)
{
  fHitCoord[0][0] = x;
  fHitCoord[1][0] = y;
  fHitCoord[2][0] = z;
  
  fHitCov[0][0] = sigx*sigx;
  fHitCov[1][1] = sigy*sigy;
  fHitCov[2][2] = sigz*sigz;
}

PndTpcSPHit::PndTpcSPHit(const TVector3& pos,
			 const TVector3& sig)
  : SpacepointRecoHit(NparHitRep), _amp(0.), _cluster(NULL)
{
  fHitCoord[0][0] = pos.X();
  fHitCoord[1][0] = pos.Y();
  fHitCoord[2][0] = pos.Z();
  fHitCov[0][0] = sig.X()*sig.X();
  fHitCov[1][1] = sig.Y()*sig.Y();
  fHitCov[2][2] = sig.Z()*sig.Z();
  
}

PndTpcSPHit::PndTpcSPHit(FairMCPoint* point)
  : SpacepointRecoHit(NparHitRep), _amp(0.), _cluster(NULL)
{
  fHitCoord[0][0] = point->GetX();
  fHitCoord[1][0] = point->GetY();
  fHitCoord[2][0] = point->GetZ();
  // fixed errors on the monte carlo points
  fHitCov[0][0] = 0.5;
  fHitCov[1][1] = 0.5;
  fHitCov[2][2] = 0.5;
}

PndTpcSPHit::PndTpcSPHit(PndTpcCluster* cluster)
  : SpacepointRecoHit(NparHitRep)
{
  TVector3 clusterpos = cluster->pos();
  fHitCoord[0][0] = clusterpos.X();
  fHitCoord[1][0] = clusterpos.Y();
  fHitCoord[2][0] = clusterpos.Z();

  TVector3 sig = cluster->sig();
  fHitCov[0][0] = sig.X()*sig.X();
  fHitCov[1][1] = sig.Y()*sig.Y();
  fHitCov[2][2] = sig.Z()*sig.Z();

  _amp = cluster->amp();

  _cluster = cluster;
}


GFAbsRecoHit* 
PndTpcSPHit::clone(){
  return new PndTpcSPHit(*this);
}


TMatrixT<double>
PndTpcSPHit::getHMatrix(const GFAbsTrackRep* stateVector)
{
  assert(stateVector!=NULL);
  if (dynamic_cast<const GeaneTrackRep*>(stateVector) != NULL) {
    // Uses TrackParP (q/p,v',w',v,w)
    // coordinates are defined by detplane!
    TMatrixT<double> HMatrix(2,5);

    HMatrix[0][0] = 0.;
    HMatrix[0][1] = 0.;
    HMatrix[0][2] = 0.;
    HMatrix[0][3] = 1.;
    HMatrix[0][4] = 0.;

    HMatrix[1][0] = 0.;
    HMatrix[1][1] = 0.;
    HMatrix[1][2] = 0.;
    HMatrix[1][3] = 0.;
    HMatrix[1][4] = 1.;
    return HMatrix;
  }
  else if (dynamic_cast<const LSLTrackRep*>(stateVector) != NULL) {
    // Uses TrackParP (u,v,u',v',q/p)
    // coordinates are defined by detplane!
    TMatrixT<double> HMatrix(2,5);
    HMatrix[0][0] = 1.;
    HMatrix[0][1] = 0.;
    HMatrix[0][2] = 0.;
    HMatrix[0][3] = 0.;
    HMatrix[0][4] = 0.;

    HMatrix[1][0] = 0.;
    HMatrix[1][1] = 1.;
    HMatrix[1][2] = 0.;
    HMatrix[1][3] = 0.;
    HMatrix[1][4] = 0.;
    return HMatrix;
  }
  else {
    std::cerr << "PndTpcSPHit can only handle state"
              << " vectors of type GeaneTrackRep or LSLTrackRep-> abort" 
	      << std::endl;
    throw;
  }
 
}


