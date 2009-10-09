//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class DemoRecoHit
//      see DemoRecoHit.hh for details
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
#include "DemoRecoHit.h"

// C/C++ Headers ----------------------
#include <assert.h>

// Collaborating Class Headers --------
#include "FairMCPoint.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "GFDetPlane.h"

// Class Member definitions -----------

ClassImp(DemoRecoHit)


DemoRecoHit::~DemoRecoHit()
{}

DemoRecoHit::DemoRecoHit()
  : PlanarRecoHit(NparHitRep)
{}

DemoRecoHit::DemoRecoHit(double x, double y, double z,
			   double sigx, double sigy)
  : PlanarRecoHit(NparHitRep)
{
  fHitCoord[0][0] = x;
  fHitCoord[1][0] = y;
  fPolicy.setDetPlane(GFDetPlane(TVector3(0,0,z),
		       TVector3(1,0,0),
		       TVector3(0,1,0)));
  fHitCov[0][0] = sigx*sigx;
  fHitCov[1][1] = sigy*sigy;
}

DemoRecoHit::DemoRecoHit(const TVector3& pos,
			   double sigx, double sigy)
  : PlanarRecoHit(NparHitRep)
{
  fHitCoord[0][0] = pos.X();
  fHitCoord[1][0] = pos.Y();
  fPolicy.setDetPlane(GFDetPlane(TVector3(0,0,pos.Z()),
		       TVector3(1,0,0),
		       TVector3(0,1,0)));
  fHitCov[0][0] = sigx*sigx;
  fHitCov[1][1] = sigy*sigy;
}

DemoRecoHit::DemoRecoHit(FairMCPoint* point)
  : PlanarRecoHit(NparHitRep)
{
  fHitCoord[0][0] = point->GetX();
  fHitCoord[1][0] = point->GetY();
  fPolicy.setDetPlane(GFDetPlane(TVector3(0,0,point->GetZ()),
		       TVector3(1,0,0),
		       TVector3(0,1,0)));
  double sigx=0.1;
  double sigy=0.1;
  fHitCov[0][0] = sigx*sigx;
  fHitCov[1][1] = sigy*sigy;
}


GFAbsRecoHit* 
DemoRecoHit::clone(){
  return new DemoRecoHit(*this);
}


TMatrixT<double>
DemoRecoHit::getHMatrix(const GFAbsTrackRep* stateVector)
{
  assert(stateVector!=NULL);
  if (dynamic_cast<const LSLTrackRep*>(stateVector) != NULL) {
    // LSLTrackRep (x,y,x',y',q/p)
    TMatrixT<double> HMatrix(NparHitRep,5);
    
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
  else if (dynamic_cast<const GeaneTrackRep*>(stateVector) != NULL) {
    // Uses TrackParP (q/p,v',w',v,w)
    // coordinates are defined by detplane!
    TMatrixT<double> HMatrix(NparHitRep,5);
    
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
  else {
    std::cerr << "DemoRecoHit can only handle state"
              << " vectors of type LSLTrackRep or GeaneTrackRep -> abort" 
	      << std::endl;
    throw;
  }
 
}


