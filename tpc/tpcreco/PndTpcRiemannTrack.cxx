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

#define DEBUG 0

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
  _center(0.,0.,0.), _radius(0),
  _av(0.,0.,0.),  _sumOfWeights(0),
  _m(0), _t(0),
  _dip(0), _sinDip(0), _rms(0),
  fRiemannScale(24.6), _isFitted(false), _isInitialized(false),
  _isFinished(false), _isGood(false), _doSort(true)
{}

PndTpcRiemannTrack::PndTpcRiemannTrack(double scale)
: _n(0.,0.,0.), _c(0),
  _center(0.,0.,0.), _radius(0),
  _av(0.,0.,0.),  _sumOfWeights(0),
  _m(0), _t(0),
  _dip(0), _sinDip(0), _rms(0),
  fRiemannScale(scale), _isFitted(false), _isInitialized(false),
  _isFinished(false), _isGood(false), _doSort(true)
{}


void PndTpcRiemannTrack::deleteHits(){
  for (unsigned int i=0; i<getNumHits(); ++i){
    delete _hits[i];
  }
  _hits.clear();
}

void
PndTpcRiemannTrack::initTargetTrack(double Dip, double curvature){
  if (getNumHits()!=1) return;

  _isInitialized=true;
  _isFitted=true;

  TVector3 hit0 = _hits[0]->cluster()->pos();

  // hit1 is the "origin" or vertex of the track to be initialized
  double z1 = hit0.Z() - hit0.Perp() / TMath::Tan(TMath::PiOver2()-Dip);
  TVector3 hit1(0,0,z1);

  // init plane parameters
  _n.SetXYZ(hit0.X(), hit0.Y(), curvature);
  _n.RotateZ(TMath::PiOver2()); // rotate 90 deg
  _n.SetMag(1.);
  _c=0; // track coming from origin

  // calc _center and _radius
  centerR();

  _radius = (hit0-_center).Perp();

  // get angle of hit0
  hit0 -= _center;
  double angle0 = hit0.Phi(); // [-pi, pi]
  _hits[0]->setAngleOnHelix(angle0); // set angle of first hit relative to x axis

  // get angle of hit1
  hit1 -= _center;
  double angle1 = hit1.Phi(); // [-pi, pi]

  _m = (z1 - hit0.Z()) / (angle1 - angle0) * -1.;
  _t = hit0.Z() - _m*angle0;

  _dip = TMath::ATan(-1.* _m/_radius) + TMath::PiOver2();
  _sinDip=sin(_dip);

  //std::cout << "Dip " << _dip << " angle 0 " << angle0 << "  angle1 " << angle1 << "\n";

}


void
PndTpcRiemannTrack::initCircle(double phi) {
  if (getNumHits()!=1) return;

  _isInitialized=true;
  _isFitted=true;

  TVector3 hit0 = _hits[0]->cluster()->pos();

  _center.SetXYZ(0,0,0);
  _radius = hit0.Perp();

  _m = 0;
  _t = hit0.Z();

  _dip = TMath::PiOver2();
  _sinDip=sin(_dip);

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

  Dist=9.E99;

  int found;
  double dis;

  if(from<0) from = 0;
  if (to<from) to = from+1;
  if(to>_hits.size()) to = _hits.size();

  for(int it=from; it<to; ++it){
    pos2=_hits[it]->cluster()->pos(); 
    dis=(pos2-posX).Mag();
    if(dis<Dist){
      found=it;
      Dist=dis;
    }
  }
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
  if(!_isFitted && !_isInitialized) return 1;
  double angle0 = _hits.front()->getAngleOnHelix();
  double angle1;
  if(_isInitialized){
    TVector3 hit1(0,0,0);
    hit1 -= _center;
    angle1 = hit1.Phi(); // [-pi, pi]
  }
  else{
    angle1 = _hits.back()->getAngleOnHelix();
  }
  if (angle1 > angle0) return 1;
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
  bool hasBeenFitted(_isFitted);
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

  unsigned int iVec(1);

  // no plane switching for long tracks needed! If track long enough (on riemann sphere) -> calc rms only for smallest eigenvec!
  if (nhits>10 && (getFirstHit()->x() - getLastHit()->x()).Mag() > 0.2) {
    iVec=2;
  }

  for(; iVec<3; ++iVec){
    planeN=TMatrixDColumn(eigenVec,iVec);
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

  double firstangle = hit0.Phi(); // [-pi, pi]
  _hits[0]->setAngleOnHelix(firstangle); // set angle of first hit relative to x axis

  double lastangle(firstangle);

  TVector3 hiti;

  // phi goes counterclockwise and can be > 2Pi for curlers
  double meanAngle, twoPi(TMath::TwoPi()), nTurns(1);
  bool twoPiCheck(hasBeenFitted && _radius < 50. && nhits > 10 && _m*twoPi > 2. && _sinDip > 0.03 && _sinDip < 0.99);

  for(int i=1; i<nhits; ++i){
    hiti = _hits[i]->cluster()->pos() - _center;

    double angle = hiti.DeltaPhi(hit0);

    // check if we have to go +-2Pi further
    if (twoPiCheck && i > 5){
      // todo: make it working properly!!!!
      meanAngle = (lastangle-firstangle)/i;
      if (angle/meanAngle < -4){
        if (angle < 0) {
          angle += twoPi;
        }
        else {
          angle -= twoPi;
        }
        firstangle += angle;
        ++nTurns;
      }
    }

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
  
  // calc dip
  _dip = TMath::ATan(_m/_radius) + TMath::PiOver2();
  _sinDip=sin(_dip);
}


void
PndTpcRiemannTrack::fitAndSort(){

  _isInitialized=false;

  // sort by z so that angle calculation in refit() is possible
  if(_doSort) {
    // keep rough sorting!
    if((_hits.front())->z() < (_hits.back())->z())
      std::sort(_hits.begin(), _hits.end(), sortByZ);
    else
      std::sort(_hits.begin(), _hits.end(), sortByZInv);
  }

  refit(); // fit plane and dip

  // if dip [90°+-50°] or R<0.5, resort by angle
  if(_doSort && _isFitted &&
     ((_dip>0.698131701 && _dip<2.44346095) ||
      _radius < 0.5)) {
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
  rms = sqrt(rms);
  return rms;
}


double
PndTpcRiemannTrack::distRMS() const{
  if(!_isFitted && !_isInitialized) return 0;

  double d2s(0), dis;

  unsigned int nHits(getNumHits());

  for(int it=0; it<nHits; ++it){
    dis = distHelix(_hits[it], false);
    dis *= dis;
    d2s += dis;
  }

  d2s /= double(nHits);

  if (d2s < 1E-50) return 1E-25;
  return sqrt(d2s);
}


void
PndTpcRiemannTrack::centerR() {
  if(!_isFitted && !_isInitialized) return;

  // look at sphere from side, perpendicular to plane, so that plane becomes a line
  // line:   x=-_c*nx + a*nz;  z=-_c*nz - a*nx
  // nx = sqrt(1-nz^2)
  // circle: x^2 + (z-0.5)^2 + 0.5^2
  // then intersect line with circle -> solutions a1, a2;
  double nz = _n[2];    // z component 
  double c2 = _c*_c;
  double arg1 = -1.*(nz-1)*(nz+1);
  double root1(1E-5);
  if(arg1>1E-10) root1 = sqrt(arg1);

  double arg2 = 1.-nz*nz-4.*c2-4.*_c*nz;
  double root2(1E-5);
  if(arg2>1E-10) root2 = sqrt(arg2);
  double a1 = -0.5*root1 + 0.5*root2;
  double a2 = -0.5*root1 - 0.5*root2;

  // now we get two points on the sphere (x1,z1), (x2,z2)
  double argnx = 1.-nz*nz;
  double nx(1E-5);
  if(argnx>1E-10) nx = sqrt(argnx);

  double x1 = -1.*_c*nx + a1*nz;
  double z1 = -1.*_c*nz - a1*nx;
  double x2 = -1.*_c*nx + a2*nz;
  double z2 = -1.*_c*nz - a2*nx; 

  // project them back onto the plane
  // we get two radii
  double r1, r2;

  if(z1>0.99999999) r1=1.E4;
  else if(z1<0.0000000001) r1=1.E-5;
  else r1 = sqrt(z1/(1.-z1));
  if(x1<0) r1 *= -1.;
  r1 *= fRiemannScale;

  if(z2>0.99999999) r2=1.E4;
  else if(z2<0.0000000001) r2=1.E-5;
  else r2 = sqrt(z2/(1.-z2));
  if(x2<0) r2 *= -1.;
  r2 *= fRiemannScale;

  _radius = 0.5*fabs(r2-r1);
  
  //std::cout<< "r1 = " << r1 << "   r2 = " << r2 << "   radius = " << _radius;

  // limit
  if (_radius<0.1) _radius = 0.1;   // 1 mm
  //if (_radius>1.E5) _radius = 1.E5; // 1 km

  // center
  _center=_n;
  _center.SetZ(0);
  _center.SetMag(0.5*(r1+r2));

  //_center.Print();

}


double
PndTpcRiemannTrack::distHelix(PndTpcRiemannHit* hit, bool calcPos, bool TwoPiCheck, TVector3* POCA) const {
  if(!_isFitted && !_isInitialized) return 0.;

  double hit_angle=hit->getAngleOnHelix();
  TVector3 pos = hit->cluster()->pos();
  double hitZ = hit->z();

  TVector3 hitX = pos - _center;

  double hit_angleR, hit_angleZ;

  if(calcPos){
    double d;
    int ahit(0);
    if(!_isInitialized){ // when track is initialized, only hit0 has a meaningful angle!
      ahit = getClosestHit(hit, d);
      if (ahit == getNumHits()) --ahit;
    }

    TVector3 hit1 =  _hits[ahit]->cluster()->pos() - _center;
    double phi1 = _hits[ahit]->getAngleOnHelix();

    hit_angleR = hitX.DeltaPhi(hit1) + phi1;

    // check if nearest position position lies multiples of 2Pi away
    if(TwoPiCheck && _radius < 50.){
      double z = _m * hit_angleR + _t;
      double dZ =  hitZ-z; // positive when above helix, negative when below

      double twoPi = 2*TMath::Pi();
      int nn = 0;

      double zCheck;

      const unsigned int maxIt = 4;
      unsigned int it = 0;

      while (it<maxIt){
        zCheck =  _m * (hit_angleR + twoPi) + _t;
        if (fabs(hitZ- zCheck) < fabs(dZ)){
          dZ = hitZ-zCheck;
          hit_angleR += twoPi;
        }
        else break;
        ++it;
      }

      nn=0;
      it=0;

      while (it<maxIt){
        zCheck =  _m * (hit_angleR - twoPi) + _t;
        if (fabs(hitZ- zCheck) < fabs(dZ)){
          dZ = hitZ-zCheck;
          hit_angleR -= twoPi;
        }
        else break;
        ++it;
      }

    }

    hit_angleZ = 0;
    if (fabs(_m)>1.E-3) hit_angleZ = (hitZ-_t)/_m;

    double zWeigh = 0.5*(cos(2.*_dip)+1.);
    hit_angle = hit_angleR*(1-zWeigh) + hit_angleZ*zWeigh;

    hit_angle = 0;
    if (fabs(_m)>1.E-22) hit_angle = (hitZ-_t)/_m;

  } // end recalcPos


  double sinphi, cosphi, xHelix, yHelix, zHelix, dist2, distance, deltadist, delta, mindist(999999), accuracy(1E-4);
  unsigned int i(0), maxIt(4);

  // newtons method for finding POCA
  while (true){
    sinphi = sin(hit_angle);
    cosphi = cos(hit_angle);

    xHelix = _center.X() + cosphi * _radius - pos.X();
    yHelix = _center.Y() + sinphi * _radius - pos.Y();
    zHelix = _m * hit_angle + _t - hitZ;

    dist2 = (xHelix*xHelix + yHelix*yHelix + zHelix*zHelix);
    if (dist2 > 1E-20) distance = sqrt(dist2);
    else distance = 1E-10;

    deltadist = mindist - distance;
    if (distance < mindist) mindist = distance;

    if (deltadist < accuracy || i>maxIt) break;

    // f  = (-1.* xHelix*sinphi*_radius + yHelix*cosphi*_radius + zHelix*_m) / distance; // first derivative of  distance  wrt  phi
    // f1 = f/(distance*distance) + ( _radius*_radius - xHelix*cosphi*_radius - yHelix*sinphi*_radius + 2.*_m*_m )/distance; // second derivative of  distance  wrt  phi
    // hit_angle -= f/f1;

    // simplified:
    delta = (-1.* xHelix*sinphi*_radius + yHelix*cosphi*_radius + zHelix*_m);
    delta = delta / (delta/(distance*distance) + _radius*_radius - xHelix*cosphi*_radius - yHelix*sinphi*_radius + 2.*_m*_m);
    hit_angle -= delta;

    ++i;
  }

  if(POCA!=NULL) POCA->SetXYZ(xHelix, yHelix, zHelix);

  return mindist;
}


void
PndTpcRiemannTrack::getPosDirOnHelix(unsigned int i, TVector3& pos, TVector3& dir) const {
  if (!_isFitted && !_isInitialized) return;

  double hit_angle;

  if(_isInitialized) {
    hit_angle = _hits[0]->getAngleOnHelix();
  }
  else {
    double hit_angleR = _hits[i]->getAngleOnHelix();
    double hit_angleZ = 0;
    if (fabs(_m)>1.E-3) hit_angleZ = (_hits[i]->z()-_t)/_m;

    double zWeigh = 0.5*(cos(2.*_dip)+1.);
    hit_angle = hit_angleR*(1-zWeigh) + hit_angleZ*zWeigh;
  }

  TVector3 Rn(_radius,0.,0.);
  Rn.SetPhi(hit_angle);

  pos = _center + Rn;
  pos.SetZ(_m * hit_angle + _t);

  // direction
  TVector3 z(0.,0.,-1.);
  dir = z.Cross(pos-_center);
  if (winding()>0) dir *= -1;
  dir.SetTheta(TMath::Pi()-_dip);

  dir.SetMag(1.);
}


double
PndTpcRiemannTrack::getMom(double Bz) const {
  if (!_isFitted && !_isInitialized) return 0;
  if(_sinDip<1E-2) return fabs(_radius/1.E-2 * 0.0003 * Bz);
  return fabs(_radius/_sinDip * 0.0003 * Bz);
}


TVector3
PndTpcRiemannTrack::pocaToZ() const {
  TVector3 POCA(0,0,0);
  if (!_isFitted) return POCA;

  double closestAngle; // angle on track of hit with smallest distance to z axis
  TVector3 ctrToClHit; // vector from center to hit with smallest distance to z axis
  if (_hits.front()->cluster()->pos().Perp() < _hits.back()->cluster()->pos().Perp()){
    closestAngle = _hits.front()->getAngleOnHelix();
    ctrToClHit = _hits.front()->cluster()->pos() - _center;
  }
  else {
    closestAngle = _hits.back()->getAngleOnHelix();
    ctrToClHit = _hits.back()->cluster()->pos() - _center;
  }

  double angle = closestAngle - ctrToClHit.DeltaPhi(-1.*_center); // angle on helix of POCA
  //double angle = (-1.*_center).Phi();

  POCA.SetX(1.);
  POCA.SetPhi(angle);
  POCA.SetMag(_radius);
  POCA += _center;
  POCA.SetZ(_m*angle + _t);

  //std::cout<<"POCA "; POCA.Print();

  return POCA;
}


TVector3
PndTpcRiemannTrack::pocaToIP(double z) const {
  TVector3 POCA(0,0,z);
  if (!_isFitted) return POCA;

  PndTpcCluster* TestCluster = new PndTpcCluster(POCA,1,0);
  PndTpcRiemannHit* TestHit = new PndTpcRiemannHit(TestCluster);
  double hDist(distHelix(TestHit,true, true, &POCA));

  delete TestHit;
  delete TestCluster;

  return POCA;
}


double
PndTpcRiemannTrack::resolution() const {
  if (_isInitialized || !_isFitted) return 1.E3;

  static const double minrms(1E-4);
  double rms(minrms);
  if (_rms > minrms) rms = _rms;

  double projLength(fabs((getFirstHit()->getAngleOnHelix() - getLastHit()->getAngleOnHelix()) * _radius));
  if (projLength<0.1) projLength = 0.1;

  // estimate the resolution
  return rms*fRiemannScale/(projLength*projLength) * sqrt(720/(getNumHits()+4)); // from pdg book, rms instead of epsilon (spacial resolution)
}


double
PndTpcRiemannTrack::quality() const {
  if (_isInitialized || !_isFitted) return 0;
  double res = sqrt(1./resolution())/50.; // invert and scale [0..1]
  if (res>1) return 1.;
  return res;
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

