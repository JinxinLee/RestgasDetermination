//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class DemoSPHit
//      see DemoSPHit.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
// modified for hyp purpose by A.sanchez
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndHypRecoSPHit.h"

// C/C++ Headers ----------------------
#include <assert.h>

// Collaborating Class Headers --------
#include "FairMCPoint.h"
#include "FairHit.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "DetPlane.h"
#include "PndHypHit.h"
// Class Member definitions -----------

ClassImp(PndHypRecoSPHit)


PndHypRecoSPHit::~PndHypRecoSPHit()
{}

PndHypRecoSPHit::PndHypRecoSPHit()
  : SpacepointRecoHit(NparHitRep)
{}

PndHypRecoSPHit::PndHypRecoSPHit(double x, double y, double z,
			   double sigx, double sigy, double sigz)
  : SpacepointRecoHit(NparHitRep)
{
  _hitCoord[0][0] = x;
  _hitCoord[1][0] = y;
  _hitCoord[2][0] = z;
  
  _hitCov[0][0] = sigx*sigx;
  _hitCov[1][1] = sigy*sigy;
  _hitCov[2][2] = sigz*sigz;
}

PndHypRecoSPHit::PndHypRecoSPHit(const TVector3& pos,
		 const TVector3& sig)
  : SpacepointRecoHit(NparHitRep)
{
  _hitCoord[0][0] = pos.X();
  _hitCoord[1][0] = pos.Y();
  _hitCoord[2][0] = pos.Z();
  _hitCov[0][0] = sig.X()*sig.X();
  _hitCov[1][1] = sig.Y()*sig.Y();
  _hitCov[2][2] = sig.Z()*sig.Z();
  
}

PndHypRecoSPHit::PndHypRecoSPHit(PndHypHit* point)//FairMCPoint* point
  : SpacepointRecoHit(NparHitRep)
{
  _hitCoord[0][0] = point->GetX();
  _hitCoord[1][0] = point->GetY();
  _hitCoord[2][0] = point->GetZ();
  // fixed errors on the monte carlo points
  _hitCov[0][0] = 0.5;
  _hitCov[1][1] = 0.5;
  _hitCov[2][2] = 0.5;
}


AbsRecoHit* 
PndHypRecoSPHit::clone(){
  return new PndHypRecoSPHit(*this);
}


void 
PndHypRecoSPHit::setHMatrix(const AbsTrackRep* stateVector,
			 const TMatrixT<double>& state)
{
  assert(stateVector!=NULL);
  if (dynamic_cast<const GeaneTrackRep*>(stateVector) != NULL) {
    // Uses TrackParP (q/p,v',w',v,w)
    // coordinates are defined by detplane!
    _HMatrix.ResizeTo(2,5);

    _HMatrix[0][0] = 0.;
    _HMatrix[0][1] = 0.;
    _HMatrix[0][2] = 0.;
    _HMatrix[0][3] = 1.;
    _HMatrix[0][4] = 0.;

    _HMatrix[1][0] = 0.;
    _HMatrix[1][1] = 0.;
    _HMatrix[1][2] = 0.;
    _HMatrix[1][3] = 0.;
    _HMatrix[1][4] = 1.;
  }
  else if (dynamic_cast<const LSLTrackRep*>(stateVector) != NULL) {
    // Uses TrackParP (u,v,u',v',q/p)
    // coordinates are defined by detplane!
    _HMatrix.ResizeTo(2,5);

    _HMatrix[0][0] = 1.;
    _HMatrix[0][1] = 0.;
    _HMatrix[0][2] = 0.;
    _HMatrix[0][3] = 0.;
    _HMatrix[0][4] = 0.;

    _HMatrix[1][0] = 0.;
    _HMatrix[1][1] = 1.;
    _HMatrix[1][2] = 0.;
    _HMatrix[1][3] = 0.;
    _HMatrix[1][4] = 0.;
  }
  else {
    std::cerr << "PndHypRecoSPHit can only handle state"
              << " vectors of type GeaneTrackRep or LSLTrackRep-> abort" 
	      << std::endl;
    throw;
  }
 
}


