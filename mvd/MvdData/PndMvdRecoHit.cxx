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
#include "CbmMCPoint.h"
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
//   if(fGeoH!=0) 
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
 
  TVector3 o, u, v, p;
//   if(fGeoH==0) 
  fGeoH = new PndMvdGeoHandling(gGeoManager);
  fGeoH->GetOUVId(point->GetDetName(),o,u,v);
  setDetPlane(DetPlane(o,u,v));

  //  convert from global to local frame, use the middle of the sensor
  p = fGeoH->MasterToLocalId( 
        0.5*(point->GetPosition() + point->GetPositionOut()) , 
        point->GetDetName());

  _hitCoord[0][0] = p.X();
  _hitCoord[1][0] = p.Y();
  _hitCoord[2][0] = 0.; // local point!

  Double_t sigx=0.1/TMath::Sqrt(12);
  Double_t sigy=0.1/TMath::Sqrt(12);

  Double_t cost=1., sint=0.;

  _hitCov[0][0] = cost*sigx*sigx;
  _hitCov[0][1] = sint*sigx*sigy;
  _hitCov[1][0] = sint*sigy*sigx;
  _hitCov[1][1] = cost*sigy*sigy;
}

PndMvdRecoHit::PndMvdRecoHit(PndMvdHit* hit)
  : RecoHitIfc<PlanarHitPolicy>(fNparHitRep)
{

  std::cout<<" -I- PndMvdRecoHit::PndMvdRecoHit(PndMvdHit*) called."<<std::endl;
  std::cout<<*hit<<std::endl;
//   if(fGeoH==0) 
  fGeoH = new PndMvdGeoHandling(gGeoManager);

  TVector3 o, u, v, p;
  fGeoH->GetOUVId(hit->GetDetName(),o,u,v);
  setDetPlane(DetPlane(o,u,v));
  std::cout << "o: " << o.X() << " " << o.Y() << " " << o.Z() << std::endl;
  std::cout << "u: " << u.X() << " " << u.Y() << " " << u.Z() << std::endl;
  std::cout << "v: " << v.X() << " " << v.Y() << " " << v.Z() << std::endl;
  setDetPlane(DetPlane(o,u,v));

  // get local hit values
  //  convert from global to local frame, use the middle of the sensor
  p = fGeoH->MasterToLocalId(hit->GetPosition() , hit->GetDetName());
  _hitCoord[0][0] = p.X();
  _hitCoord[1][0] = p.Y();
  _hitCoord[2][0] = 0.; // local point!

  // get covarianve values 
  _hitCov[0][0] = hit->GetDx();
  _hitCov[1][1] = hit->GetDy();
  _hitCov[2][2] = hit->GetDz();

}


// PndMvdRecoHit::PndMvdRecoHit(const TVector3& hit, const TMatrixT<Double_t>& cov,
//                        const DetPlane* detplane)
//   : RecoHitIfc<PlanarHitPolicy>(fNparHitRep)
// {
//     std::cout<<" -I- PndMvdRecoHit::PndMvdRecoHit() called."<<std::endl;
//   // get hit values from the CbmHit part of PndMvdCluster
//   _hitCoord[0][0] = hit.x();
//   _hitCoord[1][0] = hit.y();
//   _hitCoord[2][0] = hit.z(); // put to 0?
//   _hitCov = cov;
//   setDetPlane(*detplane);
// }

// PndMvdRecoHit::PndMvdRecoHit(const TVector3& hit, const TMatrixT<Double_t>& cov,
//                        const TVector3& o,const TVector3& u,const TVector3& v)
//   : RecoHitIfc<PlanarHitPolicy>(fNparHitRep)
// {
//     std::cout<<" -I- PndMvdRecoHit::PndMvdRecoHit() called."<<std::endl;
//   // get hit values from the CbmHit part of PndMvdCluster
//   _hitCoord[0][0] = hit.x();
//   _hitCoord[1][0] = hit.y();
//   _hitCoord[2][0] = hit.z(); // put to 0?
//   _hitCov = cov; // LOCAL errors inthe sensor plane
//   DetPlane detplane(o,u,v);
//   setDetPlane(detplane);
// }


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
    // LSLTrackRep (x,y,x',y',q/p)
    _HMatrix.ResizeTo(fNparHitRep,5);
    //       x
    _HMatrix[0][0] = getDetPlane(0).getU().X(); 
    _HMatrix[0][1] = getDetPlane(0).getV().X();
    _HMatrix[0][2] = 0.;
    _HMatrix[0][3] = 0.;
    _HMatrix[0][4] = 0.;
    //       y
    _HMatrix[1][0] = getDetPlane(0).getU().Y(); 
    _HMatrix[1][1] = getDetPlane(0).getV().Y();
    _HMatrix[1][2] = 0.;
    _HMatrix[1][3] = 0.;
    _HMatrix[1][4] = 0.;
    //       z
    _HMatrix[2][0] = getDetPlane(0).getU().Z(); 
    _HMatrix[2][1] = getDetPlane(0).getV().Z();
    _HMatrix[2][2] = 0.; 
    _HMatrix[2][3] = 0.;
    _HMatrix[2][4] = 0.;
    //Set shifting vector to coordinates
    //avoid division with a small number)
    if(state[0][0] > 1E-4) {
      _HMatrix[0][0] += getDetPlane(0).getO().X() / state[0][0] ;
      _HMatrix[1][0] += getDetPlane(0).getO().Y() / state[0][0] ;
      _HMatrix[2][0] += getDetPlane(0).getO().Z() / state[0][0] ;
    } else {
      _HMatrix[0][1] += getDetPlane(0).getO().X() / state[1][0] ;
      _HMatrix[1][1] += getDetPlane(0).getO().Y() / state[1][0] ;
      _HMatrix[2][1] += getDetPlane(0).getO().Z() / state[1][0] ;
    }

//     _HMatrix[0][0] = 1.;
//     _HMatrix[0][1] = 0.;
//     _HMatrix[0][2] = 0.;
//     _HMatrix[0][3] = 0.;
//     _HMatrix[0][4] = 0.;
// 
//     _HMatrix[1][0] = 0.;
//     _HMatrix[1][1] = 1.;
//     _HMatrix[1][2] = 0.;
//     _HMatrix[1][3] = 0.;
//     _HMatrix[1][4] = 0.;
//     
//     _HMatrix[2][0] = 0.;
//     _HMatrix[2][1] = 0.;
//     _HMatrix[2][2] = 1.;
//     _HMatrix[2][3] = 0.;
//     _HMatrix[2][4] = 0.;

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

