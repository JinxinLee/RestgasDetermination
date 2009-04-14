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
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "DetPlane.h"
// This Class' Header ------------------
#include "PndGemRecoHit.h"
#include "PndGemMCPoint.h"
#include "PndGemHit.h"
//#include "PndGemGeoHandling.h"
#include "TGeoManager.h"
#include "FairRootManager.h"

// Class Member definitions -----------

ClassImp(PndGemRecoHit);


PndGemRecoHit::~PndGemRecoHit()
{
//   if(fGeoH!=0)
//   delete (fGeoH);
}

PndGemRecoHit::PndGemRecoHit()
  : RecoHitIfc<PlanarHitPolicy>(fNparHitRep)
{
  //  fGeoH = new PndGemGeoHandling(gGeoManager);
}


PndGemRecoHit::PndGemRecoHit(PndGemMCPoint* point)
  : RecoHitIfc<PlanarHitPolicy>(fNparHitRep)
{
  std::cout<<" -I- PndGemRecoHit::PndGemRecoHit(PndGemMCPoint*) called."<<std::endl;

  _hitCoord[0][0] =  point->GetX();
  _hitCoord[1][0] =  point->GetY();

  _hitCov[0][0] = 0.01;
  _hitCov[1][1] = 0.01;

  TVector3  o(0.,0.,point->GetZ()),
            u(1.,0.,0.),
            v(0.,1.,0.);

  setDetPlane(DetPlane(o,u,v));

}

PndGemRecoHit::PndGemRecoHit(PndGemHit* hit)
  : RecoHitIfc<PlanarHitPolicy>(fNparHitRep)
{

  std::cout<<" -I- PndGemRecoHit::PndGemRecoHit(PndGemHit*) called."<<std::endl;
  std::cout<<*hit<<std::endl;

  TString id =  hit->GetDetName();

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

  _hitCoord[0][0] = TMath::Sqrt(hitX*hitX+hitY*hitY);
  _hitCoord[1][0] = 0.;

  _hitCov[0][0] = hit->GetDr();
  _hitCov[1][1] = hit->GetDp();

  setDetPlane(DetPlane(oo,uu,vv));
//============================================================================


}


void
PndGemRecoHit::setHMatrix(const AbsTrackRep* stateVector,
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
PndGemRecoHit::residualScalar(AbsTrackRep* stateVector,
          const TMatrixT<Double_t>& state)
{
  throw;
}

