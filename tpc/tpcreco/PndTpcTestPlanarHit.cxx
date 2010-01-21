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
#include "PndTpcTestPlanarHit.h"

// C/C++ Headers ----------------------
#include <assert.h>

// Collaborating Class Headers --------
#include "FairMCPoint.h"
#include "PndTpcCluster.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "GFDetPlane.h"

// Class Member definitions -----------

ClassImp(PndTpcTestPlanarHit)

TRandom3 PndTpcTestPlanarHit::rand(0);

PndTpcTestPlanarHit::~PndTpcTestPlanarHit()
{}

PndTpcTestPlanarHit::PndTpcTestPlanarHit()
  : PlanarRecoHit(NparHitRep)
{}

PndTpcTestPlanarHit::PndTpcTestPlanarHit(const GFDetPlane& pl,double res)
  : PlanarRecoHit(NparHitRep)
{
  fPolicy.setDetPlane(pl);  
  fHitCoord[0][0] = rand.Gaus(0.,res);
  fHitCoord[1][0] = rand.Gaus(0.,res);
  fHitCov[0][0] = res*res;
  fHitCov[1][1] = res*res;
}


GFAbsRecoHit* 
PndTpcTestPlanarHit::clone(){
  return new PndTpcTestPlanarHit(*this);
}


TMatrixT<double>
PndTpcTestPlanarHit::getHMatrix(const GFAbsTrackRep* stateVector)
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
    std::cerr << "PndTpcTestPlanarHit can only handle state"
              << " vectors of type GeaneTrackRep or LSLTrackRep-> abort" 
	      << std::endl;
    throw;
  }
 
}


