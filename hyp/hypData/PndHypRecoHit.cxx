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
#include "PndHypRecoHit.h"
#include "PndHypPoint.h"
#include "PndHypHit.h"
#include "PndHypHit.h"
#include "PndHypGeoHandling.h"
#include "FairRootManager.h"
// Class Member definitions -----------

ClassImp(PndHypRecoHit);


PndHypRecoHit::~PndHypRecoHit()
{
 if(fGeoH!=0) delete (fGeoH);
}

PndHypRecoHit::PndHypRecoHit()
  : RecoHitIfc<PlanarHitPolicy>(fNparHitRep)
{
  fGeoH = new PndHypGeoHandling(gGeoManager);
}


PndHypRecoHit::PndHypRecoHit(PndHypPoint* point)
  : RecoHitIfc<PlanarHitPolicy>(fNparHitRep)
{
  std::cout<<" -I- PndHypRecoHit::PndHypRecoHit(PndHypMCPoint*) called."<<std::endl;
 

  _hitCoord[0][0] = point->GetX();
  _hitCoord[1][0] =  point->GetY();
 
  _hitCov[0][0] = 0.01;//cost*sigx*sigx;
  _hitCov[1][1] = 0.01;//cost*sigy*sigy;

   TVector3 o(0.,0.,point->GetZ()), 
    u(1.,0.,0.), 
    v(0.,1.,0.);

  setDetPlane(DetPlane(o,u,v));
 
}

PndHypRecoHit::PndHypRecoHit(PndHypHit* hit)
  : RecoHitIfc<PlanarHitPolicy>(fNparHitRep)
{

  std::cout<<" -I- PndHypRecoHit::PndHypRecoHit(PndHypHit*) called."<<std::endl;
  std::cout<<*hit<<std::endl;

   TString id =  hit->GetDetName();
  
  FairRootManager* ioman = FairRootManager::Instance();
  TString fGeoFile = ioman->GetInFile()->GetName();
  fGeoH = new PndHypGeoHandling(fGeoFile.Data());

  TVector3 oo, uu, vv;
  fGeoH->GetOUVId(id, oo,uu,vv);

  TVector3 position = hit->GetPosition();
  TVector3 localpos =  fGeoH->MasterToLocalId(position, id);
  
  _hitCoord[0][0] = localpos.X();
  _hitCoord[1][0] = localpos.Y();

  _hitCov[0][0] = 0.0050 * 0.0050;
  _hitCov[1][1] = 0.0050 * 0.0050;

  setDetPlane(DetPlane(oo,uu,vv));


}


void 
PndHypRecoHit::setHMatrix(const AbsTrackRep* stateVector,
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
    // LSLTrackRep (x,y,x',y',q/p)
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
PndHypRecoHit::residualScalar(AbsTrackRep* stateVector,
          const TMatrixT<Double_t>& state)
{
  throw;
}



