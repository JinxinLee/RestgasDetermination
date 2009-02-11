//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndMvdRecoHit
//      see PndMvdRecoHit.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Ralf Kliemt           TUD            (application to PndMvd)
//
//-----------------------------------------------------------

// C/C++ Headers ----------------------
// root Headers ----------------------
#include "TMatrixT.h"
#include "TMath.h"
// Collaborating Class Headers --------
#include "FairMCPoint.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "DetPlane.h"
// This Class' Header ------------------
#include "PndMvdRecoHit.h"
#include "PndMvdMCPoint.h"
#include "PndMvdHit.h"
#include "PndMvdHit.h"
#include "PndMvdGeoHandling.h"

// Class Member definitions -----------

ClassImp(PndMvdRecoHit);


PndMvdRecoHit::~PndMvdRecoHit()
{
  if(fGeoH!=0)
  delete (fGeoH);
}

PndMvdRecoHit::PndMvdRecoHit()
  : RecoHitIfc<PlanarHitPolicy>(fNparHitRep)
{
  fGeoH = new PndMvdGeoHandling(gGeoManager);
}


PndMvdRecoHit::PndMvdRecoHit(PndMvdMCPoint* point)
  : RecoHitIfc<PlanarHitPolicy>(fNparHitRep)
{
  std::cout<<" -I- PndMvdRecoHit::PndMvdRecoHit(PndMvdMCPoint*) called."<<std::endl;

  _hitCoord[0][0] =  point->GetX();
  _hitCoord[1][0] =  point->GetY();

  _hitCov[0][0] = 0.01;
  _hitCov[1][1] = 0.01;

  TVector3  o(0.,0.,point->GetZ()),
            u(1.,0.,0.),
            v(0.,1.,0.);

  setDetPlane(DetPlane(o,u,v));

}

PndMvdRecoHit::PndMvdRecoHit(PndMvdHit* hit)
  : RecoHitIfc<PlanarHitPolicy>(fNparHitRep)
{

  std::cout<<" -I- PndMvdRecoHit::PndMvdRecoHit(PndMvdHit*) called."<<std::endl;
  std::cout<<*hit<<std::endl;

  _hitCoord[0][0] =  hit->GetX();
  _hitCoord[1][0] =  hit->GetY();

//   _hitCov[0][0] = hit->GetDx();
//   _hitCov[1][1] = hit->GetDy();

//TODO this is cheating the error bars larger for the kalman
  _hitCov[0][0] = 0.01;
  _hitCov[1][1] = 0.01;

  TVector3  o(0.,0.,hit->GetZ()),
            u(1.,0.,0.),
            v(0.,1.,0.);

  setDetPlane(DetPlane(o,u,v));

}


void
PndMvdRecoHit::setHMatrix(const AbsTrackRep* stateVector,
       const TMatrixT<Double_t>& state)
{

  // !! TODO I copied this from the DemoRecoHit - check validity!!!
  if (dynamic_cast<const GeaneTrackRep*>(stateVector) != NULL) {
    // Uses TrackParP (q/p,v',w',v,w)
    // coordinates are defined by detplane!
    _HMatrix.ResizeTo(fNparHitRep,5);

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
    //I know, since this is the same everytime, it could be done in the
    //the constructor, but I do it here anyway, to make clear that in the
    //case of several track-reps per hit, it would have to be done here
    // LSLTrackRep (x,y,x',y',q/p) recohits are (Xloc,Yloc,0.)
    // The virtual detector plane in LSL is perpendicular to Zlab
    _HMatrix.ResizeTo(fNparHitRep,5);
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
    std::cerr << "DemoRecoHit can only handle state"
              << " vectors of type LSLTrackRep or GeaneTrackRep -> abort"
        << std::endl;
     throw;
  }

}

Double_t
PndMvdRecoHit::residualScalar(AbsTrackRep* stateVector,
          const TMatrixT<Double_t>& state)
{
  throw;
}

