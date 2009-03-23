// ----------------------------------------------------------
// Please see WirepointHitPolicy.h  before using this class.
// ----------------------------------------------------------

#include "WirepointHitPolicy.h"

#include "assert.h"
#include <cmath>

#include "TMath.h"
#include "TVector3.h"

#include "AbsRecoHit.h"
#include "FitterExceptions.h"

WirepointHitPolicy::WirepointHitPolicy() : _wireCoordFlag(false), _maxdistance(1.E50) {;}

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
     
      std::cout << "WirepointHitPolicy: plane not valid!!" << std::endl;
    }
}


const DetPlane& 
WirepointHitPolicy::detPlane(AbsRecoHit* hit, AbsTrackRep* rep)
{

  TMatrixT<double> x=hit->getRawHitCoord();
  assert(x.GetNrows()==8);
  TVector3 wire1(x[0][0],x[1][0],x[2][0]);
  TVector3 wire2(x[3][0],x[4][0],x[5][0]);

  //  distance of one (the first) of the wire extremities from the plane
  Double_t d_from_refplane =  _detPlane.dist(wire1).Mag();
  if(d_from_refplane < 1e-5) return _detPlane;

  
  // point of closest approach
  TVector3 poca, poca_onwire, dirInPoca;
  
  rep->extrapolateToLine(wire1, wire2, poca, dirInPoca, poca_onwire);
  
  
  Double_t distance;
  distance = TMath::Sqrt(fabs(((wire1-poca).Mag2()*(wire2-wire1).Mag2()-pow((wire1-poca).Dot(wire2-wire1),2))/(wire2-wire1).Mag2()));
  
  // check poca inside tube 
  if(distance > _maxdistance) {
    FitterException exc("distance poca-wire > maxdistance", __LINE__,__FILE__);	
    throw exc;    
  }
  
  // find plane
  // unitary vector along distance
  // poca (on track), poca_onwire (on wire)
  TVector3 fromwiretoextr = poca - poca_onwire;     
  fromwiretoextr.SetMag(1.);
  // unitary vector along the wire
  TVector3 wiredirection = wire2 - wire1; 
  wiredirection.SetMag(1.);
  
  // check orthogonality
  if(fabs(fromwiretoextr * wiredirection) > 1e-3) {
    FitterException exc("fromwiretoextr*wiredirection > 1e-3", __LINE__,__FILE__);	
    throw exc;    
  }
  
  TVector3 U;
  U = fromwiretoextr;
  TVector3 V;
  V = wiredirection;
  U.SetMag(1.);
  V.SetMag(1.);
  
  TVector3 O;
  if(_wireCoordFlag == false) O = poca_onwire;
  else                        O = (wire1 + wire2) * 0.5;

  
  _detPlane = DetPlane(O, U, V);
  
  return _detPlane;
}
