//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRiemannTrack
//      see PndTpcRiemannTrack.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcRiemannTrack.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <iterator>
#include <assert.h>
#include <cmath>


// Collaborating Class Headers --------
#include "PndTpcCluster.h"

#include "TMatrixD.h"
#include "TVectorD.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TF1.h"
#include "TMath.h"
#include "TPolyMarker3D.h"
#include "PndTpcRiemannHit.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TPolyLine3D.h"
#include "TSystem.h"

// Class Member definitions -----------

bool sortByZ(PndTpcRiemannHit* hit1, PndTpcRiemannHit* hit2){
  return (hit1->z() < hit2->z());
}
bool sortByZInv(PndTpcRiemannHit* hit1, PndTpcRiemannHit* hit2){
  return (hit1->z() > hit2->z());
}
bool sortByAngle(PndTpcRiemannHit* hit1, PndTpcRiemannHit* hit2){
  return (hit1->getAngleOnHelix() < hit2->getAngleOnHelix());
}
bool sortByAngleInv(PndTpcRiemannHit* hit1, PndTpcRiemannHit* hit2){
  return (hit1->getAngleOnHelix() > hit2->getAngleOnHelix());
}


ClassImp(PndTpcRiemannTrack)

PndTpcRiemannTrack::PndTpcRiemannTrack()
: _n(0.,0.,0.), _c(0),
  _av(0.,0.,0.),  _sumOfWeights(0),
  _m(0), _t(0),
  _dip(0), _sinDip(0), _rms(0),
  fRiemannScale(24.6), _isFitted(false), _doSort(true)
{}

PndTpcRiemannTrack::PndTpcRiemannTrack(double scale)
: _n(0.,0.,0.), _c(0),
  _av(0.,0.,0.),  _sumOfWeights(0),
  _m(0), _t(0),
  _dip(0), _sinDip(0), _rms(0),
  fRiemannScale(scale), _isFitted(false), _doSort(true)
{}

void
PndTpcRiemannTrack::init(double x0_, double y0_, double R_, 
			 double Dip, double z0){
  double x0=x0_/100;
  double y0=y0_/100;
  double R=R_/100;

  double R2=R*R;
  double lambda=R2-x0*x0-y0*y0;
  if(lambda==0)lambda=0.0001;
  double D=lambda/sqrt(4*R2+(1-lambda)*(1-lambda));
  double DC=-D/lambda;
  double C=DC-D;
  double A=-2*x0*DC;
  double B=-2*y0*DC;
  
  _n.SetXYZ(A, B, C);
  _c=D;
  _dip=Dip;
  _sinDip=TMath::Sin(_dip);
  _t=z0;
}


bool 
PndTpcRiemannTrack::checkScale(PndTpcRiemannHit* hit) const {
  bool result= hit->getScale()==this->getScale();
  if(!result){
    std::cerr << "RiemannScale not matching!" << std::endl;
    std::cerr << "Hit   : "<< hit->getScale() << std::endl
	      << "Track : "<< this->getScale() << std::endl;
  }
  return result;
}

int
PndTpcRiemannTrack::getClosestHit(PndTpcRiemannHit* hit, 
				                          double& Dist, 
				                          TVector3& outdir) const {
  int it2=getClosestHit(hit,Dist);
  if(_hits.size()>1){
    // catch the case where we are at boundary
    int it1=it2;
    if(it1>0) --it1;

    int it3=it2;
    if(it3 < _hits.size()-1) ++it3;
    
    TVector3 pos1=_hits[it1]->cluster()->pos(); //next point
    TVector3 pos3=_hits[it3]->cluster()->pos();

    // construct general direction of track from these three
    outdir=(pos3-pos1);
    outdir.SetMag(1);
  }
  else {
    TVector3 pos=hit->cluster()->pos(); //next point
    TVector3 pos2=_hits[it2]->cluster()->pos();
    outdir=(pos2-pos);
    outdir.SetMag(1);
  }
  return it2;
}


int
PndTpcRiemannTrack::getClosestHit(PndTpcRiemannHit* hit, double& Dist, int from, int to) const {
  TVector3 posX=hit->cluster()->pos();
  TVector3 pos2;

  int found;
  double mindis=9.E99;
  double dis;

  if(from<0) from = 0;
  if(to>_hits.size()) to = _hits.size();

  for(int it=from; it<to; ++it){
    pos2=_hits[it]->cluster()->pos(); 
    dis=(pos2-posX).Mag();
    if(dis<mindis){
      found=it;
      mindis=dis;
    }
  }
  Dist = mindis;
  return found;
}


int
PndTpcRiemannTrack::getClosestRiemannHit(PndTpcRiemannHit* hit, double& Dist) const {
  TVector3 posX=hit->x();
  TVector3 pos2;

  int found;
  double mindis=9.E99;
  double dis;

  for(int it=0; it<_hits.size(); ++it){
    pos2=_hits[it]->x(); 
    dis=(pos2-posX).Mag();
    if(dis<mindis){
      found=it;
      mindis=dis;
    }
  }
  Dist = mindis;
  return found;
}


void
PndTpcRiemannTrack::addHit(PndTpcRiemannHit* hit){
  if(!checkScale(hit)) throw; // check if Riemannscale of hits matches that of the track!
  int nbefore=_hits.size();
  _mcid.AddIDCollection(hit->cluster()->mcId(),1.);

  // update average
  double weightFactor = hit->cluster()->sig().Perp();
  if (weightFactor>1.E-3) weightFactor=1./weightFactor;
  else weightFactor=1.E3;
  
  _av *= _sumOfWeights;
  _av += hit->x() * weightFactor;
  _sumOfWeights += weightFactor;
  _av *= 1./_sumOfWeights;

  _hits.push_back(hit);
}


void
PndTpcRiemannTrack::removeHit(unsigned int ihit){
  delete _hits[ihit];

  // todo: update average & MCID Collection!!!

  _hits.erase(_hits.begin()+ihit);
}


int
PndTpcRiemannTrack::winding() const { // returns winding sense along z-axis
  if (_hits.back()->getAngleOnHelix() > _hits.front()->getAngleOnHelix()) return 1;
  return -1;
}


double
PndTpcRiemannTrack::dist(PndTpcRiemannHit* hit, TVector3 n2, double c2, bool useArguments) const {
  if(!useArguments){
    if(!_isFitted) return 0.;
    n2 = _n;
    c2 = _c; 
  }
  // distance plane - center of sphere
  TVector3 cent3(0,0,0.5); // center of sphere
  double l = c2 + n2*cent3; // distance plane to center
  double thetaPlane=TMath::ACos(2.*l); // angle 
      
  // construct vector of hit relative to center of sphere
	TVector3 vh=hit->x()-cent3;
	vh.SetMag(1);
	double cos1=(-1.*n2)*vh;
	double thetaHit=TMath::ACos(cos1);
	return (thetaHit-thetaPlane); // positive when hit is outside of the helix, negative when hit is inside helix
}


void
PndTpcRiemannTrack::refit(){ // helix fit
  _isFitted = false;
  unsigned int nhits=_hits.size();
  if(nhits<3) return; // need at least 3 points to make a planefit

  TMatrixT<double> Av(3,1);
  Av[0][0]=_av[0];
  Av[1][0]=_av[1];
  Av[2][0]=_av[2];

  TMatrixD sampleCov(3,3);
  
  double nh=0;
  double weightFactor;
  for(int it=0; it<nhits; ++it){
    TMatrixD h(3,1);
    // weigh hits with 1/cluster error
    weightFactor = _hits[it]->cluster()->sig().Perp();
    if (weightFactor>1.E-3) weightFactor=1./weightFactor;
    else weightFactor=1.E3;
    nh += weightFactor;
    h[0][0]=_hits[it]->x().X();
    h[1][0]=_hits[it]->x().Y();
    h[2][0]=_hits[it]->x().Z();
    TMatrixD d(3,1);
    d=h-Av;
    TMatrixD dt(TMatrixD::kTransposed,d);
    TMatrixD ddt(d,TMatrixD::kMult,dt);
    ddt *= weightFactor;
    sampleCov+=ddt;  
  }

  if(sampleCov==0) {
    // can happen if a pad fires continuously and the resulting clusters have the same xy coords
    // -> they are mapped to one single point on the riemann sphere
    std::cerr<<"PndTpcRiemannTrack::refit() - can't fit plane, covariance matrix is zero"<<std::endl;
    return;
  }

  sampleCov*=1./nh;
  
  TVectorD eigenValues(3);
  TMatrixD eigenVec=sampleCov.EigenVectors(eigenValues);
  
  // eigenvalues are sorted according to their value
  // in descending order -> last one is smallest
  
  // check smallest and second smallest eigenvector
  // for this we use the rms distance of hits to section of plane with sphere on the sphere

  _rms=1.E32;
  unsigned int imin=2;
  
  double norm, c1, rms;
  TVectorD planeN(3);
  TVectorD planeNmin(3);

  for(unsigned int i=1;i<3;++i){
    planeN=TMatrixDColumn(eigenVec,i);
    norm=TMath::Sqrt(planeN.Norm2Sqr());
    if (norm<1E-10) {
      std::cerr<<"PndTpcRiemannTrack::refit() - eigenvector too small"<<std::endl;
      return;
    }
    planeN*=1./norm;
    TVector3 plane3(planeN[0],planeN[1],planeN[2]);
    c1=-1.*plane3*_av; // distance plane to origin
    rms=calcRMS(plane3, c1);
    if(rms<_rms){
	    _rms=rms;
	    planeNmin = planeN;
    }
  }
	
  _n.SetXYZ(planeNmin[0],planeNmin[1],planeNmin[2]);
  _c=-1.*_n*_av; 

  _isFitted = true;
  // finished planefit


  //
  // fit dip
  //

  // calc angles on helix (hits must be sorted at least roughly!)
  centerR(); // _isFitted must be true for this to work!! Calculate center and radius

  TVector3 hit0 = _hits[0]->cluster()->pos() - _center;

  double lastangle = hit0.Phi(); // [-pi, pi]
  _hits[0]->setAngleOnHelix(lastangle); // set angle of first hit relative to x axis

  TVector3 hiti;

  // phi goes counterclockwise and can be > 2Pi for curlers
  for(int i=1; i<nhits; ++i){
    hiti = _hits[i]->cluster()->pos() - _center;

    double angle = hiti.DeltaPhi(hit0);
    lastangle += angle;

    _hits[i]->setAngleOnHelix(lastangle);

    hit0 = hiti;
  }

  // get phis and zs
  TGraph g(nhits);
  for(unsigned int it=0; it<nhits; ++it){
    g.SetPoint(it,_hits[it]->getAngleOnHelix(),_hits[it]->z());
  }
  int errorcode;
  g.LeastSquareLinearFit(nhits,_t,_m,errorcode,-999,999);

  if (errorcode!=0) {
    _isFitted = false;
    std::cerr<<"PndTpcRiemannTrack::refit() - can't fit dip"<<std::endl;
    return; // phi z fit did not work
  }
  
  // limit the range of m
  if (_m > 1.E6) _m=1E6;
  if (_m < -1.E6) _m=-1E6;
  
  // calc dip
  _dip = TMath::ATan(_m/r()) + TMath::PiOver2();
  _sinDip=TMath::Sin(_dip);
}


void
PndTpcRiemannTrack::fitAndSort(){
  // sort by z so that angle calculation in refit() is possible
  if(_doSort) {
    // keep rough sorting!
    if((_hits.front())->z() < (_hits.back())->z())
      std::sort(_hits.begin(), _hits.end(), sortByZ);
    else
      std::sort(_hits.begin(), _hits.end(), sortByZInv);
  }

  refit(); // fit plane and dip

  // if dip [90°+-50°], resort by angle
  if(_doSort && _isFitted && _dip>0.698131701 && _dip<2.44346095) {
    // keep rough sorting!
    if (winding()>0)
      std::sort(_hits.begin(), _hits.end(), sortByAngle);
    else
      std::sort(_hits.begin(), _hits.end(), sortByAngleInv);
  }

  // update dip
  if (winding()<0){
    _dip = TMath::Pi()-_dip; // doensn't affect _sinDip; if not fitted: _dip=0
  }
}


double
PndTpcRiemannTrack::calcRMS(TVector3 n1, double c1) const {
  // loop over hits and calculate RMS
  double rms = 0.;
  double norm = 0.;
  
  for(int it=0; it<_hits.size(); ++it){
    double weightFactor = _hits[it]->cluster()->amp(); // weigh with amplitude
    norm += weightFactor;
    double distance = dist(_hits[it], n1, c1, true);
    rms += weightFactor * distance*distance;
  }

  rms /= norm;
  rms = TMath::Sqrt(rms);
  return rms;
}


void
PndTpcRiemannTrack::centerR() {
  if(!_isFitted) return;

  // look at sphere from side, perpendicular to plane, so that plane becomes a line
  // line:   x=-_c*nx + a*nz;  z=-_c*nz - a*nx
  // nx = sqrt(1-nz^2)
  // circle: x^2 + (z-0.5)^2 + 0.5^2
  // then intersect line with circle -> solutions a1, a2;
  double nz = _n[2];    // z component 
  double c2 = _c*_c;
  double root1 = TMath::Sqrt(-1.*(nz-1)*(nz+1));
  double root2 = TMath::Sqrt(1.-nz*nz-4.*c2-4.*_c*nz);
  double a1 = -0.5*root1 + 0.5*root2;
  double a2 = -0.5*root1 - 0.5*root2;

  // now we get two points on the sphere (x1,z1), (x2,z2)
  double nx = TMath::Sqrt(1.-nz*nz);
  double x1 = -1.*_c*nx + a1*nz;
  double z1 = -1.*_c*nz - a1*nx;
  double x2 = -1.*_c*nx + a2*nz;
  double z2 = -1.*_c*nz - a2*nx; 

  // project them back onto the plane
  // we get two radii
  double r1, r2;

  if(z1>0.999999) r1=1.E3;
  else if(z1<0.000001) r1=1.E-3;
  else r1 = TMath::Sqrt(z1/(1.-z1));
  if(x1<0) r1 *= -1.;
  r1 *= fRiemannScale;

  if(z2>0.999999) r2=1.E3;
  else if(z2<0.000001) r2=1.E-3;
  else r2 = TMath::Sqrt(z2/(1.-z2));
  if(x2<0) r2 *= -1.;
  r2 *= fRiemannScale;

  _radius = 0.5*TMath::Abs(r2-r1);
  
  if (_radius<0.01) _radius = 0.01;

  // center
  _center=_n;
  _center.SetZ(0);
  _center.SetMag(0.5*(r1+r2));

}


double
PndTpcRiemannTrack::distHelix(PndTpcRiemannHit* hit, bool calcPos) const {

  if(!_isFitted) return 0.; // sz distance not defined

  double hit_angle=hit->getAngleOnHelix();
  TVector3 pos = hit->cluster()->pos();
  double hitZ = hit->z();

  TVector3 hitX = pos - _center;

  if(calcPos){
    double d;
    int ahit = getClosestHit(hit, d);
    if (ahit == getNumHits()) --ahit;

    TVector3 hit1 =  _hits[ahit]->cluster()->pos() - _center;
    double phi1 = _hits[ahit]->getAngleOnHelix();

    double hit_angleR = hitX.DeltaPhi(hit1) + phi1;

    // check if nearest position position lies multiples of 2Pi away
    if(_radius < 5.){
      double z = _m * hit_angleR + _t;
      double dZ =  hitZ-z; // positive when above helix, negative when below

      double twoPi = 2*TMath::Pi();
      int nn = 0;

      double zCheck;

      while (1){
        zCheck =  _m * (hit_angleR + twoPi) + _t;
        if (TMath::Abs(hitZ- zCheck) < TMath::Abs(dZ)){
          dZ = hitZ-zCheck;
          hit_angleR += twoPi;
        }
        else break;
      }

      nn=0;

      while (1){
        zCheck =  _m * (hit_angleR - twoPi) + _t;
        if (TMath::Abs(hitZ- zCheck) < TMath::Abs(dZ)){
          dZ = hitZ-zCheck;
          hit_angleR -= twoPi;
        }
        else break;
      }

    }

    double hit_angleZ = 0;
    if (TMath::Abs(_m)>1.E-3) hit_angleZ = (hitZ-_t)/_m;

    double zWeigh = 0.5*(TMath::Cos(2.*_dip)+1.);
    hit_angle = (hit_angleR*_sinDip + hit_angleZ*zWeigh) / (_sinDip+zWeigh);

  } // end recalcPos

  TVector3 Rn(1.,0.,0.);
  Rn.SetPhi(hit_angle);
  TVector3 poca = _center + _radius*Rn;
  poca.SetZ(_m * hit_angle + _t);

  return (poca - pos).Mag(); // always >= 0
}


void
PndTpcRiemannTrack::getPosDirOnHelix(unsigned int i, TVector3& pos, TVector3& dir) const {
  if (!_isFitted) return;

  double hit_angleR = _hits[i]->getAngleOnHelix();
  double hit_angleZ = 0;
  if (TMath::Abs(_m)>1.E-3) hit_angleZ = (_hits[i]->z()-_t)/_m;

  double zWeigh = 0.5*(TMath::Cos(2.*_dip)+1.);
  double hit_angle = (hit_angleR*_sinDip + hit_angleZ*zWeigh) / (_sinDip+zWeigh);

  TVector3 Rn(1.,0.,0.);
  Rn.SetPhi(hit_angle);

  pos = _center + _radius*Rn;
  pos.SetZ(_m * hit_angle + _t);

  // direction
  TVector3 z(0.,0.,-1.);
  dir = z.Cross(pos-_center);
  dir *= winding();
  dir.SetTheta(_dip);

  dir.SetMag(1.);
}


double
PndTpcRiemannTrack::getMom(double Bz) const {
  if (!_isFitted) return 0;
  if(_sinDip<1E-2) return TMath::Abs(_radius/1.E-2 * 0.0003 * Bz);
  return TMath::Abs(_radius/_sinDip * 0.0003 * Bz);
}


void
PndTpcRiemannTrack::Plot(bool standalone){
  TCanvas* cc=NULL;
  if(standalone)cc=new TCanvas("c");
  TPolyMarker3D* maker=new TPolyMarker3D(_hits.size());
  TPolyLine3D* line=new TPolyLine3D(_hits.size());

  for(int it=0; it<_hits.size(); ++it){
    TVector3 pos=_hits[it]->cluster()->pos();
    //pos.Print();
    maker->SetPoint(it,pos.X(),pos.Y(),pos.Z());
    line->SetPoint(it,pos.X(),pos.Y(),pos.Z());
  }
  maker->SetMarkerStyle(23);
  maker->Draw();
  line->Draw();
  if(standalone){
    gApplication->SetReturnFromRun(kTRUE);
    gSystem->Run();
    delete maker;
    delete line;
    delete cc;
  }
}

