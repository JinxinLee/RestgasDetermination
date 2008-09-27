// Panda Headers ----------------------

// This Class' Header ------------------
#include "WirepointHitPolicy.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include "TMath.h"
#include "TVector3.h"
#include <cmath>

// Collaborating Class Headers --------
#include "AbsRecoHit.h"

// Class Member definitions -----------


TMatrixT<double> 
WirepointHitPolicy::hitCoord(AbsRecoHit* hit,const DetPlane& plane)
{
  TMatrixT<double> returnMat(2,1);

  checkPlane(hit, plane);

  // raw x1, y1, z1, x2, y2, z2, rdrift, zreco
  TMatrixT<double> rC = hit->getRawHitCoord();

  returnMat[0][0] = rC[6][0];
  returnMat[1][0] = rC[7][0];
  return returnMat;
}

TMatrixT<double> 
WirepointHitPolicy::hitCov(AbsRecoHit* hit,const DetPlane& plane)
{
  checkPlane(hit, plane);

  TMatrixT<double> returnCov(2,2);
  TMatrixT<double> rawCov = hit->getRawHitCov();

  returnCov[0][0] = rawCov[6][6];
  returnCov[1][0] = rawCov[7][6];
  returnCov[0][1] = rawCov[6][7];
  returnCov[1][1] = rawCov[7][7];

  return  returnCov;
}



void WirepointHitPolicy::checkPlane(AbsRecoHit* hit,const DetPlane& plane)
{
  // raw x1, y1, z1, x2, y2, z2, rdrift, zreco
  TMatrixT<double> rC = hit->getRawHitCoord();

  assert(rC.GetNrows()==8);
  
  TVector3 wire1(rC[0][0], rC[1][0], rC[2][0]);
  TVector3 wire2(rC[3][0], rC[4][0], rC[5][0]);
  TVector3 wiredirection = wire1 - wire2;
  
  TVector3 vaxis = plane.getV();
  wiredirection.SetMag(1.);
  vaxis.SetMag(1.);

  if(fabs(TMath::Abs(wiredirection.Dot(vaxis)) - 1) > 1e-3)
    {
     
      std::cout << "plane not valid for STT!!" << std::endl;
    }
}
