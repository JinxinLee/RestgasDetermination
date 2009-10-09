// -------------------------------------------------------------------------
// -----                   PndDchRecoHit2 source file                  -----
// -----                  Created 29.09.2008  by A. Wronska            -----
// -----                  is a copy&paste of the PndSttRecoHit file    -----
// -----                             for documentation, see there      -----
// -------------------------------------------------------------------------

// The DCH point is not defined by the (x, y, z) coordinates, but we ONLY know the
// (x, y) coordinates of the wire center (not the z!); moreover we know the drift
// radius. In order to find the MEASURED POINT to be used in the Kalman fit a 
// preliminary propagation must be performed to the point of closest approach to
// the firing wire.
// The plane is a virtual detector plane defined via:
// dj = wvi - vpf     (U coordinate in GFDetPlane)
// dk = wiredirection (V coordinate in GFDetPlane)
//
// input: 7 entries
// 0-1-2 ==> x,y,z of the 1st extremity of the firing wire;
// 3-4-5 ==> x,y,z of the 2nd extremity of the firing wire;
// 6     ==> drift radius;

// Pnd Headers 
#include "PndDchRecoHit2.h"
#include "PndDchCylinderHit.h"
#include "GeaneTrackRep.h"
#include "GFDetPlane.h"
#include "FairGeanePro.h"
#include "FairGeaneUtil.h"
#include "GFException.h"

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
  : WireRecoHit(NparHitRep)

{}

//------------------------------------------------------------------------------
PndDchRecoHit2::PndDchRecoHit2(const PndDchCylinderHit* cylHit): WireRecoHit(NparHitRep){
  _sangle = TMath::Sin(cylHit->GetWireAngle());
  _cangle = TMath::Cos(cylHit->GetWireAngle());
  TVector2 end1 = cylHit->GetWireEnd1();
  TVector2 end2 = cylHit->GetWireEnd2();
  Double_t z = cylHit->GetWireZcoordGlobal();
  
 fHitCoord[0][0] = end1.X();
 fHitCoord[1][0] = end1.Y();
 fHitCoord[2][0] = z;
 fHitCoord[3][0] = end2.X();
 fHitCoord[4][0] = end2.Y();
 fHitCoord[5][0] = z;
 fHitCoord[6][0] = cylHit->GetDistance();

  for(int i = 0; i < NparHitRep; i++) 
    for(int j = 0; j < NparHitRep; j++) 
      fHitCov[i][j] = 0.;
  Double_t sigr = cylHit->GetDistanceError();
  fHitCov[6][6] = sigr*sigr;

  //cylHit->Print();
  //Print();
}

//------------------------------------------------------------------------------
void  PndDchRecoHit2::Print(){
  std::cout<<"hitCoord:"; fHitCoord.Print();
  // std::cout<<"hit HMatrix:";getHMatrix().Print();
//   std::cout<<"hit GFDetPlane:";getDetPlane(0).Print();
//   std::cout<<"\n hitCov:";getHitCov(getDetPlane(0)).Print();
  std::cout<<"EOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOHEOH"<<std::endl;
  
}

TMatrixT<double> PndDchRecoHit2::getHMatrix(const GFAbsTrackRep* stateVector)
{
  if (dynamic_cast<const GeaneTrackRep*>(stateVector) != NULL) {
    TMatrixT<double> HMatrix(1,5);

    HMatrix[0][0] = 0.;
    HMatrix[0][1] = 0.;
    HMatrix[0][2] = 0.;
    HMatrix[0][3] = 1.;
    HMatrix[0][4] = 0.;
    return HMatrix;
  }
  else {
    std::cerr << "PndDchRecoHit2 can only handle state"
              << " vectors of type GeaneTrackRep -> abort" << std::endl;
    throw;
  }
 
}

