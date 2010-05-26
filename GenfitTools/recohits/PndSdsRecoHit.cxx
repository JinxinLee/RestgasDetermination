//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndSdsRecoHit
//      see PndSdsRecoHit.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Ralf Kliemt           TUD            (application to PndSds)
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
#include "GFDetPlane.h"
// This Class' Header ------------------
#include "PndSdsRecoHit.h"
#include "PndSdsMCPoint.h"
#include "PndSdsHit.h"
#include "PndGeoHandling.h"
#include "TGeoManager.h"
#include "FairRootManager.h"

// Class Member definitions -----------

ClassImp(PndSdsRecoHit);


PndSdsRecoHit::~PndSdsRecoHit()
{
  if(fGeoH!=0)
  delete (fGeoH);
}

PndSdsRecoHit::PndSdsRecoHit()
  : GFRecoHitIfc<GFPlanarHitPolicy>(fNparHitRep)
{
  fGeoH = new PndGeoHandling();
}


PndSdsRecoHit::PndSdsRecoHit(PndSdsMCPoint* point)
  : GFRecoHitIfc<GFPlanarHitPolicy>(fNparHitRep)
{
  std::cout<<" -I- PndSdsRecoHit::PndSdsRecoHit(PndSdsMCPoint*) called."<<std::endl;

  fHitCoord[0][0] =  point->GetX();
  fHitCoord[1][0] =  point->GetY();

  // we set the covariances to (50mu)^2 by hand.
  fHitCov[0][0] = 0.0050 * 0.0050;//cm //TODO: cm is rigt?
  fHitCov[1][1] = 0.0050 * 0.0050;//cm //TODO: cm is rigt?

  TVector3  o(0.,0.,point->GetZ()),
            u(1.,0.,0.),
            v(0.,1.,0.);

  fPolicy.setDetPlane(GFDetPlane(o,u,v));

}

PndSdsRecoHit::PndSdsRecoHit(PndSdsHit* hit)
  : GFRecoHitIfc<GFPlanarHitPolicy>(fNparHitRep)
{

//  std::cout<<" -I- PndSdsRecoHit::PndSdsRecoHit(PndSdsHit*) called."<<std::endl;
//  std::cout<<*hit<<std::endl;

  Int_t id =  hit->GetSensorID();

//  FairRootManager* ioman = FairRootManager::Instance();
//  TString fGeoFile = ioman->GetInFile()->GetName();
  fGeoH = new PndGeoHandling();
  TString path = fGeoH->GetPath(id);
//  std::cout<<"Detector path: "<<path.Data()<<std::endl;
  TVector3 oo, uu, vv;
  fGeoH->GetOUVId(id, oo,uu,vv);

  TVector3 position = hit->GetPosition();
  TVector3 localpos =  fGeoH->MasterToLocalId(position, id);

  fHitCoord[0][0] = localpos.X();
  fHitCoord[1][0] = localpos.Y();

  TVector3 errPos, errPosLoc;
  hit->PositionError(errPos);  
  errPosLoc = fGeoH->MasterToLocalErrorsId(errPos, id);
  
  fHitCov[0][0] = 0.0050 * 0.0050;
  fHitCov[1][1] = 0.0050 * 0.0050;
 // fHitCov[0][0] = errPosLoc.X() * errPosLoc.X();
 // fHitCov[1][1] = errPosLoc.Y() * errPosLoc.Y();
  
//  std::cout<<" -I- PndSdsRecoHit::PndSdsRecoHit: Wrote a hit with"
//  <<"\n(x,y) = ("<<localpos.X()<<","<<localpos.Y()<<")."
//  <<"\n(dx,dy) = ("<<errPosLoc.X()<<","<<errPosLoc.Y()<<"). \t not used: dz="<<errPosLoc.Z()
//  <<std::endl;

  fPolicy.setDetPlane(GFDetPlane(oo,uu,vv));
}
  //============================================================================



TMatrixT<double>
PndSdsRecoHit::getHMatrix(const GFAbsTrackRep* stateVector)
{

  // !! TODO I copied this from the DemoRecoHit - check validity!!!
  if (dynamic_cast<const GeaneTrackRep*>(stateVector) != NULL) {
    // Uses TrackParP (q/p,v',w',v,w)
    // coordinates are defined by detplane!
    TMatrixT<double> HMatrix(fNparHitRep,5);

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
    // LSLTrackRep (x,y,x',y',q/p) recohits are (Xloc,Yloc,0.)
    // The virtual detector plane in LSL is perpendicular to Zlab
    TMatrixT<double> HMatrix(fNparHitRep,5);
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
    std::cerr << "PndSdsRecoHit can only handle state"
              << " vectors of type LSLTrackRep or GeaneTrackRep -> abort"
        << std::endl;
     throw;
  }

}

Double_t
PndSdsRecoHit::residualScalar(GFAbsTrackRep* stateVector,
          const TMatrixT<Double_t>& state)
{
  throw;
}

