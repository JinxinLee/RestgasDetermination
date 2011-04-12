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

bool sortByTempPosOnTrack(PndTpcRiemannHit* hit1, PndTpcRiemannHit* hit2){
  return (hit1->tempPosOnTrack()   < hit2->tempPosOnTrack());
}
bool sortByPosOnTrack(PndTpcRiemannHit* hit1, PndTpcRiemannHit* hit2){
  return (hit1->s()   < hit2->s());
}


ClassImp(PndTpcRiemannTrack)

PndTpcRiemannTrack::PndTpcRiemannTrack()
  : _n(3),_av(3), _sumOfWeights(0), _c(0),_m(0), _t(0), _isFitted(false), _isFittedPlane(false), _nit(0), _doSort(true)
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
  
  _n[0]=A;
  _n[1]=B;
  _n[2]=C;
  _c=D;
  _m=Dip;
  _t=z0;
}


PndTpcRiemannHit*
PndTpcRiemannTrack::getHit(unsigned int i) const {
  return _hits[i];
}


PndTpcRiemannHit*
PndTpcRiemannTrack::getLastHit() const {
  return _hits.back();
}


int
PndTpcRiemannTrack::getClosestHit(PndTpcRiemannHit* hit, 
				                          double& Dist, 
				                          TVector3& outdir){
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
PndTpcRiemannTrack::getClosestHit(PndTpcRiemannHit* hit, double& Dist) {
  TVector3 posX=hit->cluster()->pos();
  TVector3 pos2;

  int found;
  double mindis=9.E99;
  double dis;

  for(int it=0; it<_hits.size(); ++it){
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
PndTpcRiemannTrack::getClosestRiemannHit(PndTpcRiemannHit* hit, double& Dist) {
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
  int nbefore=_hits.size();
  _mcid.AddIDCollection(hit->cluster()->mcId(),1.);

  // update average
  double weightFactor = 1./(hit->cluster()->sig().Perp());

  _av*=_sumOfWeights;
  _av[0]+=hit->x().X() * weightFactor;
  _av[1]+=hit->x().Y() * weightFactor;
  _av[2]+=hit->x().Z() * weightFactor;
  _sumOfWeights += weightFactor;
  _av*=1./_sumOfWeights;

  if(nbefore<2 || !_doSort){// first two hits hit
    _hits.push_back(hit);
    return;
  }

  _hits.insert(_hits.begin()+this->sortHit(hit), hit);
}


int
PndTpcRiemannTrack::sortHit(PndTpcRiemannHit* hitX){ // returns index BEFORE which to insert hitX!!!
  
  bool debug = false;
  int nhits = _hits.size();

  if(nhits<2){
    return _hits.size();
  }

  TVector3 posX=hitX->cluster()->pos();

  if(debug){
    std::cout<<"\n\n\n PndTpcRiemannTrack::sortHit   -   positions of hitX: "<<std::endl;
    posX.Print();
  }

  if(nhits==2){ // minimize tracklength
    TVector3 pos1=_hits[0]->cluster()->pos();
    TVector3 pos2=_hits[1]->cluster()->pos();

    // X-1-2
    double dx12 = (posX-pos1).Mag() + (pos1-pos2).Mag();
    // 1-X-2
    double d1x2 = (pos1-posX).Mag() + (posX-pos2).Mag();
    // 1-2-X
    double d12x = (pos1-pos2).Mag() + (pos2-posX).Mag();
    if(dx12 < d1x2 && dx12 < d12x) return 0;
    if(d1x2 < d12x) return 1;
    return 2;
  }

  if(nhits==3){ // minimize tracklength
    TVector3 pos1=_hits[0]->cluster()->pos();
    TVector3 pos2=_hits[1]->cluster()->pos();
    TVector3 pos3=_hits[2]->cluster()->pos();

    // X-1-2-3
    double dx123 = (posX-pos1).Mag() + (pos1-pos2).Mag() + (pos2-pos3).Mag();
    // 1-X-2-3
    double d1x23 = (pos1-posX).Mag() + (posX-pos2).Mag() + (pos2-pos3).Mag();
    // 1-2-X-3
    double d12x3 = (pos1-pos2).Mag() + (pos2-posX).Mag() + (posX-pos3).Mag();
    // 1-2-3-X
    double d123x = (pos1-pos2).Mag() + (pos2-pos3).Mag() + (pos3-posX).Mag();
    if(dx123 < d1x23 && dx123 < d12x3 && dx123 < d123x) return 0;
    if(d1x23 < d12x3 && d1x23 < d123x) return 1;
    if(d12x3 < d123x) return 2;
    return 3;
  }
  
  // now we have a track with 4 or more hits
  // get hit with minimum distance
  double Dist;
  int itCl = this->getClosestHit(hitX, Dist);

  if(debug){
    std::cout<<"closest Hit:"<<std::endl;
    _hits[itCl]->cluster()->pos().Print();
  }

  // build average track direction with sliding average of 4 to 5 hits
  // get up to 5 points, from it1 to it2
  int it1=itCl-2;
  if (it1 < 0) it1 = 0; 
  int it2=itCl+2;
  if (it2 > _hits.size()-1) it2 = _hits.size()-1;

  // hitIts now contains 4 to 7 hits -> reduce
  int nHits=it2-it1+1;
  if(nHits==7){
    ++it1;
    --it2;
  }
  else if(nHits==6){
    if(itCl==it1+2) --it2;
    else ++it1;
  }
  /*else if(nHits==5){
    if(itCl==it1+1) --it2;
    else if(itCl==it1+3) ++it1;
  }*/
  ++it2; // 
 
  if(debug) std::cout<<"positions of hits: "<<std::endl;

  // get positions
  std::vector<TVector3> slidingAvrg;
  for(int i=it1; i<it2; ++i) {
    slidingAvrg.push_back( _hits[i]->cluster()->pos() );
    if(debug) _hits[i]->cluster()->pos().Print();
  }  
  // calculate sliding average until 2 points left
  while(slidingAvrg.size()>2){      
    for(int i=0; i<slidingAvrg.size()-1; ++i) {
      slidingAvrg[i] = 0.5*(slidingAvrg[i]) + 0.5*(slidingAvrg[i+1]);
    } 
    slidingAvrg.pop_back();
  }
  // construct direction of track
  TVector3 direction=(slidingAvrg[1]-slidingAvrg[0]);
  direction.SetMag(1.);

  if(debug){
    std::cout<<"direction: "<<std::endl;
    direction.Print();
    std::cout<<"\n positions on track: "<<std::endl;
  }

  // calculate positions on the track
  hitX->tempPosOnTrack( direction * (posX-slidingAvrg[0]) );
  if(debug) std::cout<<"xPosOnTrack "<<hitX->tempPosOnTrack()<<std::endl;
  for(int i=it1; i<it2; ++i) {
    _hits[i]->tempPosOnTrack( direction * (_hits[i]->cluster()->pos() - slidingAvrg[0]) );
    if(debug) std::cout<<"  "<<_hits[i]->tempPosOnTrack()<<std::endl;
  }
  if(debug) std::cout<<std::endl;

  // sort by positions on track
  if(_doSort) sort(_hits.begin()+it1, _hits.begin()+it2, sortByTempPosOnTrack);

  for(int i=it1; i<it2; ++i) {
    if(hitX->tempPosOnTrack() < _hits[i]->tempPosOnTrack()) {
      if(debug) std::cout<<"inserting before "<<i<<std::endl;      
      return i; 
    }
    ++i;
  }
  if(debug) std::cout<<"inserting before "<<it2<<std::endl;  
  return it2;

}


void
PndTpcRiemannTrack::trackpos(){
  // loop over hits in track and calculate their position
  _hits[0]->setPosOnTrk(0);
  TVector3 posX;
  TVector3 avrg0; // average position of (hit-1) and hit
  TVector3 avrg1; // average position of hit and (hit+1)
  TVector3 dir;   // direction, vector from avrg0 to avrg1
  double s=0.;     // tracklength
  double sDir; // length of dir
  double sOnDir;  
  
  // calculate sliding average to be more outlier tolerant
  posX = _hits[0]->cluster()->pos();
  avrg0 = 0.5*posX + 0.5*_hits[1]->cluster()->pos();
  s += (avrg0 - posX).Mag();

  for(int it=1; it<_hits.size()-1; ++it){
    posX = _hits[it]->cluster()->pos();
    avrg1 = 0.5*posX + 0.5*_hits[it+1]->cluster()->pos();
    dir =avrg1-avrg0;
    sDir = dir.Mag();
    dir.SetMag(1.);
    sOnDir = dir * (posX-avrg0);
    _hits[it]->setPosOnTrk(s+sOnDir);
    // update values for next iteration
    s += sDir;
    avrg0 = avrg1;
  }

  // last hit
  posX = _hits[_hits.size()-1]->cluster()->pos(); 
  sOnDir = (posX-avrg0).Mag();
  _hits[_hits.size()-1]->setPosOnTrk(s+sOnDir);

  if(_doSort) sort(_hits.begin(), _hits.begin()+_hits.size(), sortByPosOnTrack);
}


// TODO: improve this!!!
int
PndTpcRiemannTrack::winding(){ // returns winding sense along z-axis
  int end=_hits.size()-1;
  if(end<2) return 0;
  int halfway = 0.5*end;
  TVector3 pos1=_hits[0]->cluster()->pos();
  TVector3 pos2=_hits[halfway]->cluster()->pos();
  TVector3 pos3=_hits[end]->cluster()->pos();
  int dir= pos1.Perp()<pos3.Perp() ? 1 : -1; // correct for forward and backward going tracks pos=(0,0,0) corresponds to IP
  pos1.SetZ(0);
  pos2.SetZ(0);
  pos3.SetZ(0);

  TVector3 d12=pos2-pos1;
  TVector3 d13=pos3-pos1;
  double a=d12.DeltaPhi(d13);
  std::cout << "dPhi="<<a<<std::endl;
  return a>0 ? dir : -dir;
}


double
PndTpcRiemannTrack::dist(PndTpcRiemannHit* hit){
  double d2=_c;
  d2+=hit->x().X()*_n[0];
  d2+=hit->x().Y()*_n[1];
  d2+=hit->x().Z()*_n[2];

  return d2;
}


void
PndTpcRiemannTrack::refit(){
  _isFittedPlane = false;
  TMatrixT<double> Av(3,1);
  Av[0][0]=_av[0];
  Av[1][0]=_av[1];
  Av[2][0]=_av[2];

  TMatrixD sampleCov(3,3);
  
  double nh=0;
  for(int it=0; it<_hits.size(); ++it){
    TMatrixD h(3,1);
    // weigh hits with 1/cluster error
    double weightFactor = 1./(_hits[it]->cluster()->sig().Perp());
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
    ++it;
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
  _n=TMatrixDColumn(eigenVec,2);

  double norm=1./TMath::Sqrt(_n.Norm2Sqr());
  _n*=norm;
  _c=-1.*_n*_av;

  _isFittedPlane = true;
}


double
PndTpcRiemannTrack::planeRMS(){
  if(!_isFittedPlane) return 0.;

  // get plane parameters
  TVector3 n3;
  n3.SetXYZ(_n[0], _n[1], _n[2]);

  // loop over hits and calculate RMS
  double rms = 0.;

  for(int it=0; it<_hits.size(); ++it){
    TVector3 pos = _hits[it]->x();
    double distance = pos*n3 + _c;
    rms += distance*distance;
    ++it;
  }

  rms /= _hits.size();
  rms = TMath::Sqrt(rms);
  return rms;
}


TVectorD
PndTpcRiemannTrack::orig() const {
  TVectorD o(2);
  double den=0.5/(_c+_n[2]);
  o[0]=-_n[0]*den;
  o[1]=-_n[1]*den;
  return o;
}


double 
PndTpcRiemannTrack::r() const {
  if(!_isFittedPlane) return 0.;
  if(_c>=0.999) return 0.01;

  // look at sphere from side, perpendicular to plane, so that plane becomes a line
  // line:   x=_c*nx - a*nz;  z=_c*nz + a*nx
  // circle: x^2 + (z-0.5)^2 + 0.5^2
  // then intersect line with circle -> solutions a1, a2;
  double nx = -1.*TMath::Sqrt(_n[0]*_n[0] + _n[1]*_n[1]); // radial direction -> x direction in 3D projection
  double nz = -1.*_n[2];    // z direction
  double c2 = _c*_c;
  double root = TMath::Sqrt(nx*nx - 8.*c2*nx*nx*nz*nz - 4.*c2*pow(nz, 4.) + 4.*_c*pow(nz, 3.) - 4.*c2*pow(nx, 4.) + 4*_c*nx*nx*nz);
  double denom = nx*nx + nz*nz;
  double a1 =  0.5*(nx+root)/denom;
  double a2 = -0.5*(-1.*nx+root)/denom;

  // now we get two points on the sphere (x1,z1), (x2,z2)
  double x1 = _c*nx - a1*nz;
  double z1 = _c*nz + a1*nx;
  double x2 = _c*nx - a2*nz;
  double z2 = _c*nz + a2*nx; 

  /*std::cout<<"PndTpcRiemannTrack::r() "<<std::endl;
  std::cout<<"_n"<<std::endl;
  _n.Print();
  std::cout<<"nx "<<nx<<"  nz "<<nz<<std::endl;
  std::cout<<"_c "<<_c<<std::endl;
  std::cout<<"root "<<root<<std::endl;
  std::cout<<"a1 "<<a1<<"  a2 "<<a2<<std::endl;
  std::cout<<"x1 "<<x1<<"  z1 "<<z1<<std::endl;
  std::cout<<"x2 "<<x2<<"  z2 "<<z2<<std::endl;
*/

  // project them back onto the plane
  // we get two radii
  double r1, r2;

  if(z1>0.9999) r1=1.E4;
  else r1 = TMath::Sqrt(z1/(1.-z1));
  if(x1<0) r1 *= -1.;

  if(z2>0.9999) r2=1.E4;
  else r2 = TMath::Sqrt(z2/(1.-z2));
  if(x2<0) r2 *= -1.;
  
  //std::cout<<"r1 "<<r1<<"  r2 "<<r2<<std::endl;

  return TMath::Abs(r2-r1) * 8.66025; // RIEMANNSCALE TODO still hardcoded*/
}


void
PndTpcRiemannTrack::szFit(bool print){
  _isFitted=false;
  trackpos(); // calculate positions on track

  // get s'es and zs
  TGraph g(getNumHits());
  unsigned int nn=getNumHits();
  for(unsigned int it=0; it<nn; ++it){
    g.SetPoint(it,_hits[it]->s(),_hits[it]->z());
  }
  int errorcode;
  g.LeastSquareLinearFit(nn,_t,_m,errorcode,-999,999);
  //std::cout<<"szFit Error Code:"<<errorcode<<std::endl;
  if(print){
    g.SetMarkerStyle(21);
    g.Draw("AP");
    TF1 f1("f1","[0]*x+[1]",0,100);
    f1.SetParameter(0,_m);
    f1.SetParameter(1,_t);
    f1.SetLineColor(kRed);
    f1.Draw("same");
    
    gApplication->SetReturnFromRun(kTRUE);
    gSystem->Run();
  }
  if(errorcode==0) _isFitted=true;
  return;
}


double
PndTpcRiemannTrack::szDist(PndTpcRiemannHit* hit, bool calcPos){
  if(!_isFitted) szFit();
  double hit_s=hit->s();

  if(calcPos){
    TVector3 posX=hit->cluster()->pos();

    // ahit is the hit AFTER our hit ...
    int ahit = this->sortHit(hit); 
    TVector3 pos1;

    if(ahit==0){ // @ begining
      pos1 = _hits[ahit]->cluster()->pos();
      hit_s=-((posX-pos1).Mag());  
    }
    else {
      --ahit; // hit before our hit
      pos1 = _hits[ahit]->cluster()->pos();
      double s1=_hits[ahit]->s();
      hit_s=s1+(posX-pos1).Mag();  
    }
  } // end recalcPos

  // calc distance to line
  TVector3 line, X;
  line.SetXYZ(1., _m, 0.);
  line.SetMag(1.); // unit vector of fitted s-z-line
  X.SetXYZ(hit_s, hit->z()-_t, 0.);
  return ( line*(line*X) - X ).Mag();
}


// only after szFit!
double
PndTpcRiemannTrack::dip() const {
  std::cout << _m << std::endl;
  if(_m>=1. || _m<=-1.)return 0;
  return TMath::PiOver2()-(asin(_m)); // 0 .. Pi for m = -1 .. 1
}


// Todo: check for backward going tracks!!!
double
PndTpcRiemannTrack::sign() const {
  double s=getHit(3)->s(); 
  return s<0 ? -1. : 1;
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


