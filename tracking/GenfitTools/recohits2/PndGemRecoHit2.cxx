//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndGemRecoHit
//      see PndGemRecoHit.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Ralf Kliemt          TUD            (application to PndMvd)
//      Radoslaw Karabowicz  GSI            (conversion to GEM)
//
//-----------------------------------------------------------

// C/C++ Headers ----------------------
// root Headers ----------------------
#include "TMatrixT.h"
#include "TMath.h"
// Collaborating Class Headers --------
#include "FairMCPoint.h"
#include "RKTrackRep.h"
#include "PlanarMeasurement.h"
// This Class' Header ------------------
#include "PndGemRecoHit2.h"
#include "PndGemMCPoint.h"
#include "PndGemHit.h"
//#include "PndGemGeoHandling.h"
#include "TGeoManager.h"
#include "FairRootManager.h"

// Class Member definitions -----------

ClassImp(PndGemRecoHit2);


PndGemRecoHit2::~PndGemRecoHit2()
{
//   if(fGeoH!=0)
//   delete (fGeoH);
}

PndGemRecoHit2::PndGemRecoHit2()
  : genfit::PlanarMeasurement(fNparHitRep)
{
  //  fGeoH = new PndGemGeoHandling(gGeoManager);
}


PndGemRecoHit2::PndGemRecoHit2(PndGemMCPoint* point)
  : genfit::PlanarMeasurement(fNparHitRep)
{
  //  std::cout<<" -I- PndGemRecoHit::PndGemRecoHit(PndGemMCPoint*) called."<<std::endl;

  rawHitCoords_[0] =  point->GetX();
  rawHitCoords_[1] =  point->GetY();

  rawHitCov_[0][0] = 0.01;
  rawHitCov_[1][1] = 0.01;

  TVector3  o(0.,0.,point->GetZ()),
            u(1.,0.,0.),
            v(0.,1.,0.);

  genfit::SharedPlanePtr plane(new genfit::DetPlane(o,u,v));
  setPlane(plane);

}


PndGemRecoHit2::PndGemRecoHit2(PndGemHit* hit, const genfit::TrackCandHit* candHit)
  : genfit::PlanarMeasurement(fNparHitRep)
{

  //   std::cout<<" -I- PndGemRecoHit::PndGemRecoHit(PndGemHit*) called."<<std::endl;
   //  std::cout<<*hit<<std::endl;


  setDetId(candHit->getDetId());
  setHitId(candHit->getHitId());

   FairRootManager* ioman = FairRootManager::Instance();
   TString fGeoFile = ioman->GetInFile()->GetName();
   //  PndGemGeoHandling* fGeoH = new PndGemGeoHandling(fGeoFile.Data());
   //TString path = fGeoH->GetPath(id);

   Double_t hitX = hit->GetX(), hitY = hit->GetY();
   Double_t phiAValue = TMath::ATan(hitX/hitY);
   if ( hitY < 0 ) phiAValue += TMath::Pi();
   else if ( hitX < 0 ) phiAValue +=  2.*TMath::Pi();

   TVector3 oo (0.,0.,hit->GetZ()),
 //            uu ( TMath::Cos(phiAValue),TMath::Sin(phiAValue),0),
 //            vv (-TMath::Sin(phiAValue),TMath::Cos(phiAValue),0);
     uu ( TMath::Sin(phiAValue), TMath::Cos(phiAValue),0),
     vv ( TMath::Cos(phiAValue),-TMath::Sin(phiAValue),0);

   rawHitCoords_[0] = TMath::Sqrt(hitX*hitX+hitY*hitY);
   rawHitCoords_[1] = 0.;

   rawHitCov_[0][0] = hit->GetDr()*hit->GetDr();
   rawHitCov_[1][1] = hit->GetDp()*hit->GetDp();

   genfit::SharedPlanePtr plane(new genfit::DetPlane(oo,uu,vv));
   setPlane(plane, candHit->getPlaneId());
 //============================================================================
}


