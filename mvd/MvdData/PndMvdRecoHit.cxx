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
#include "TGeoManager.h"
#include "FairRootManager.h"

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

  // we set the covariances to (50mu)^2 by hand.
  _hitCov[0][0] = 0.0050 * 0.0050;//cm //TODO: cm is rigt?
  _hitCov[1][1] = 0.0050 * 0.0050;//cm //TODO: cm is rigt?

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

  TString id =  hit->GetDetName();

  FairRootManager* ioman = FairRootManager::Instance();
  TString fGeoFile = ioman->GetInFile()->GetName();
  PndMvdGeoHandling* fGeoH = new PndMvdGeoHandling(fGeoFile.Data());
  TString path = fGeoH->GetPath(id);
  std::cout<<"Detector path: "<<path.Data()<<std::endl;
  TVector3 oo, uu, vv;
  fGeoH->GetOUVId(id, oo,uu,vv);

  TVector3 position = hit->GetPosition();
  TVector3 localpos =  fGeoH->MasterToLocalId(position, id);

  _hitCoord[0][0] = localpos.X();
  _hitCoord[1][0] = localpos.Y();

  TVector3 errPos, errPosLoc;
  hit->PositionError(errPos);  
  errPosLoc = fGeoH->MasterToLocalErrorsId(errPos, id);
  
  _hitCov[0][0] = 0.0050 * 0.0050;
  _hitCov[1][1] = 0.0050 * 0.0050;
 // _hitCov[0][0] = errPosLoc.X() * errPosLoc.X();
 // _hitCov[1][1] = errPosLoc.Y() * errPosLoc.Y();
  
  std::cout<<" -I- PndMvdRecoHit::PndMvdRecoHit: Wrote a hit with"
  <<"\n(x,y) = ("<<localpos.X()<<","<<localpos.Y()<<")."
  <<"\n(dx,dy) = ("<<errPosLoc.X()<<","<<errPosLoc.Y()<<"). \t not used: dz="<<errPosLoc.Z()
  <<std::endl;

  setDetPlane(DetPlane(oo,uu,vv));
}
  //============================================================================



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
    std::cerr << "PndMvdRecoHit can only handle state"
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

