// -------------------------------------------------------------------------
// -----                   PndDchRecoHit2 source file                  -----
// -----                  Created 29.09.2008  by A. Wronska            -----
// -----                  is a copy&paste of the PndSttRecoHit file    -----
// -----                             for documentation, see there      -----
// -------------------------------------------------------------------------

// The STT point is not defined by the (x, y, z) coordinates, but we ONLY know the
// (x, y) coordinates of the wire center (not the z!); moreover we know the drift
// radius. In order to find the MEASURED POINT to be used in the Kalman fit a 
// preliminary propagation must be performed to the point of closest approach to
// the firing wire.
// The plane is a virtual detector plane defined via:
// dj = wvi - vpf     (U coordinate in DetPlane)
// dk = wiredirection (V coordinate in DetPlane)
//
// input: 8 entries
// 0-1-2 ==> x,y,z of the 1st extremity of the firing wire;
// 3-4-5 ==> x,y,z of the 2nd extremity of the firing wire;
// 6     ==> drift radius;
// 7     ==> reconstructed z coordinate (0 for now!)

// Pnd Headers 
#include "PndDchRecoHit2.h"
#include "PndDchCylinderHit.h"
#include "GeaneTrackRep.h"
#include "DetPlane.h"
#include "FairGeanePro.h"
#include "FairGeaneUtil.h"
#include "FitterExceptions.h"

//ROOT includes
#include "TMath.h"

// C/C++ Headers 
#include <assert.h>
#include <exception>
#include <iostream> 
#include <cmath>

using std::cout;
using std::endl;


ClassImp(PndDchRecoHit2)

PndDchRecoHit2::~PndDchRecoHit2()
{}

PndDchRecoHit2::PndDchRecoHit2()
  : WirepointRecoHit(NparHitRep)

{}

//------------------------------------------------------------------------------
PndDchRecoHit2::PndDchRecoHit2(const PndDchCylinderHit* cylHit): WirepointRecoHit(NparHitRep){
  _sangle = TMath::Sin(cylHit->GetWireAngle());
  _cangle = TMath::Cos(cylHit->GetWireAngle());
  TVector2 end1 = cylHit->GetWireEnd1();
  TVector2 end2 = cylHit->GetWireEnd2();
  Double_t z = cylHit->GetWireZcoordGlobal();
  
 _hitCoord[0][0] = end1.X();
 _hitCoord[1][0] = end1.Y();
 _hitCoord[2][0] = z;
 _hitCoord[3][0] = end2.X();
 _hitCoord[4][0] = end2.Y();
 _hitCoord[5][0] = z;
 _hitCoord[6][0] = cylHit->GetDistance();
 _hitCoord[7][0] = 0.0; // should be the coordinate along wire 

  for(int i = 0; i < NparHitRep; i++) 
    for(int j = 0; j < NparHitRep; j++) 
      _hitCov[i][j] = 0.;
  Double_t sigr = cylHit->GetDistanceError();
  _hitCov[6][6] = sigr*sigr;
  Double_t sigz = 3.0;
  _hitCov[7][7] = sigz * sigz; 

  //cylHit->Print();
  //Print();
}

//------------------------------------------------------------------------------
void  PndDchRecoHit2::Print(){
  std::cout<<"hitCoord:"; _hitCoord.Print();
  // std::cout<<"hit HMatrix:";getHMatrix().Print();
//   std::cout<<"hit DetPlane:";getDetPlane(0).Print();
//   std::cout<<"\n hitCov:";getHitCov(getDetPlane(0)).Print();
  std::cout<<"EOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOH"<<std::endl;
  
}

void PndDchRecoHit2::setHMatrix(const AbsTrackRep* stateVector,
				const TMatrixT<double>& state)
{
  if (dynamic_cast<const GeaneTrackRep*>(stateVector) != NULL) {
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
  else {
    std::cerr << "PndDchRecoHit2 can only handle state"
              << " vectors of type GeaneTrackRep -> abort" << std::endl;
    throw;
  }
 
}

double PndDchRecoHit2::residualScalar(AbsTrackRep* stateVector,
			   const TMatrixT<double>& state){
  throw;
}

const DetPlane& PndDchRecoHit2::detPlane(AbsRecoHit* hit, AbsTrackRep* rep){
  
  if (dynamic_cast<const GeaneTrackRep*>(rep) != NULL) {
    
    TMatrixT<double> x=hit->getRawHitCoord();
    assert(x.GetNrows()==8);
    TVector3 wire1(x[0][0],x[1][0],x[2][0]);
    TVector3 wire2(x[3][0],x[4][0],x[5][0]);
  
   
    // propagation to closest approach
    // retrieve GEANE
    FairGeanePro *geane = ((GeaneTrackRep*)rep)->getPropagator(); 

    // call propagation to closest approach to a wire 
    Int_t pca = 2;
    TVector3 point;
    TVector3 start = rep->getPos();
    //  cout << "rep start point " << endl;
    //  start.Print();
    Double_t distance1, distance2;
    distance1 = (wire1 - start).Mag();
    distance2 = (wire2 - start).Mag();
    Double_t maxdistance;
    if(distance1 < distance2) maxdistance = distance2;
    else maxdistance = distance1;
    maxdistance *= 2.;  
    Double_t Rad = 0.;
    TVector3 vpf, vwi;
    Double_t Di = 0.;
    Float_t trklength = 0.;
    Int_t PDG = ((GeaneTrackRep*)rep)->getPDG();

    TMatrixT<double> status = rep->getState();
    DetPlane referenceplane = rep->getReferencePlane();

    TMatrixT<double> covar = rep->getCov();
    FairGeaneUtil util;
    Double_t cov55[5][5];
    for(int i = 0; i < 5; i++) for(int j = 0; j < 5; j++) cov55[i][j] = covar[i][j];
    Double_t CovMatrix[15];
    util.FromMat25ToVec15(cov55, CovMatrix);
    //  for(int i = 0; i < 15; i++) CovMatrix[i] = 0.;
  
    TVector3 posp = rep->getPos();
    TVector3 momp = rep->getMom(); // MARS
    Double_t q = rep->getCharge();
    TVector3 posErr(0.,0.,0.);
    TVector3 MomErr(0.,0.,0.);
    TVector3 o = referenceplane.getO();
    TVector3 dj = referenceplane.getU();
    TVector3 dk = referenceplane.getV();

    FairTrackParP *parrep = new FairTrackParP(posp, momp, posErr, MomErr, q,  o, dj, dk);

    //   posp.Print();
    //   momp.Print();
    //   o.Print();
    //   dj.Print();
    //   dk.Print();
  
    Int_t direction = ((GeaneTrackRep*)rep)->getPropDir();

    geane->ActualFindPCA(pca, parrep, direction);
    Int_t findpca = geane->FindPCA(pca, PDG, point, wire1, wire2, maxdistance, Rad, vpf, vwi, Di, trklength);
  
    if(findpca != 0) { 
      cout << "detector plane determination FAILED!" << endl;  //return kFALSE; 
      FitterException exc("findpca failure", __LINE__,__FILE__);	
      throw exc;    
    }
    /*
    Double_t distance;
    distance = TMath::Sqrt(fabs(((wire1-vpf).Mag2()*(wire2-wire1).Mag2()-pow((wire1-vpf).Dot(wire2-wire1),2))/(wire2-wire1).Mag2()));
    
    // check vpf inside tube 
    if(distance>0.5) {
      cout << "vpf outside the firing tube" << endl; 
      cout << "distance = " << distance<<endl; 
      FitterException exc("distance vpf-wire > 0.5", __LINE__,__FILE__);	
      throw exc;    
    }
    */

    // find plane
    // unitary vector along distance
    // vpf on track, vwi on wire
    TVector3 fromwiretoextr = vpf - vwi;     
    fromwiretoextr.SetMag(1.);
    // unitary vector along the wire
    TVector3 wiredirection = wire2 - wire1; 
    wiredirection.SetMag(1.);

    // check orthogonality
    if(fabs(fromwiretoextr * wiredirection) > 1e-3) {
      cout << "detector plane determination FAILED! not orthogonal" << endl; 
      FitterException exc("fromwiretoextr*wiredirection > 1e-3", __LINE__,__FILE__);	
      throw exc;    
    }
  
    TVector3 U;
    U = fromwiretoextr;
    TVector3 V;
    V = wiredirection;
    U.SetMag(1.);
    V.SetMag(1.);

    //    TVector3 O = vwi; // CHECK
    TVector3 O = (wire1 + wire2) * 0.5;
    _detPlane = DetPlane(O, U, V);

    return _detPlane;
  }
  else {
    std::cerr << "PndDchRecoHit2 can only handle state"
              << " vectors of type GeaneTrackRep -> abort" << std::endl;
    throw;
  }
 
}

const DetPlane& PndDchRecoHit2::getDetPlane(AbsTrackRep* rep){
  
 // AbsRecoHit* thishit = clone();
  //   distance of one (the first) of the wire extremities from the plane
  Double_t distance =  _detPlane.dist(TVector3(_hitCoord[0][0], _hitCoord[1][0],_hitCoord[2][0])).Mag();
  if(distance < 1e-5) return _detPlane;
  
  _detPlane = detPlane((AbsRecoHit *)this, rep);
  return _detPlane;
}
