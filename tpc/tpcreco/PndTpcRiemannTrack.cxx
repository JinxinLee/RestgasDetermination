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
#include "PndTpcRiemannHit.h"
#include "PndTpcCluster.h"

#include "TMatrixD.h"
#include "TVectorD.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TF1.h"
#include "TMath.h"
#include "TPolyMarker3D.h"
// Class Member definitions -----------


ClassImp(PndTpcRiemannTrack)

PndTpcRiemannTrack::PndTpcRiemannTrack()
  : _n(3),_av(3), _c(0),_m(0), _t(0), _isFitted(false), _nit(0), _doSort(true)
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
  std::list<PndTpcRiemannHit*>::const_iterator it=_hits.begin();
  std::advance(it, i);
  return *it;
}

PndTpcRiemannHit*
PndTpcRiemannTrack::getLastHit() const {
  return _hits.back();
}

//##########################################################################

hitIt
PndTpcRiemannTrack::getClosestHit(PndTpcRiemannHit* hit, 
				  double& Dist) {
  // check if we start at end or at beginning:
  TVector3 posX=hit->cluster()->pos();
  TVector3 posend=_hits.back()->cluster()->pos();
  TVector3 posstart=_hits.front()->cluster()->pos();
  double s1x=(posX-posstart).Mag();
  double s2x=(posX-posend).Mag();
  int dir=0;
  if(s1x>=s2x){// hit closer to end of list
     dir=-1;// direction -1 --> start backwards search
  }
  else { // hit closer to start of list
    dir=+1; // direction 1 --> search from front to end
  }

  // prepare iterators through list
  hitIt it2;
  if(dir>0){
    it2=_hits.begin();
  }
  else {
    it2=_hits.end(); 
    --it2;
  }

  TVector3 pos2=(*it2)->cluster()->pos(); //next point
  // find closest point to my hit
  int found=0;
  double dis=(pos2-posX).Mag();
  double mindis=dis;
  //std::cout<<"dir="<<dir<<std::endl;
  int it=0; // count iterations
  while(it<_hits.size() && it2!=_hits.end()){ // allow for some overstepping
    ++it;
    pos2=(*it2)->cluster()->pos(); //next point
    dis=(pos2-posX).Mag();
    //std::cout << dis << "   ";
    if(dis<mindis){
      found=1;
      mindis=dis;
    }
    else ++found; // record how far the last minimum is away
    if(it2==_hits.begin()&& dir<0) {
      //std::cout<< "dropping out at begin";
      found-=1;
      
      break;
    }
    dir>0 ? ++it2 : --it2;
  }
  //if(it2==_hits.end()&& dir>0){std::cout<< "dropping out at end";}
  //std::cout << std::endl << "mindis="<<mindis<< "   found="<<found<<std::endl;
  // step back to the minimum
  for(int step=0;step<found;++step) dir>0 ? --it2 : ++it2;
  pos2=(*it2)->cluster()->pos(); //next point
  Dist=(pos2-posX).Mag();
  return it2;
}

hitIt
PndTpcRiemannTrack::getClosestHit(PndTpcRiemannHit* hit, 
				  double& Dist, 
				  TVector3& outdir){
  hitIt it2=getClosestHit(hit,Dist);
  if(_hits.size()>1){
    // catch the case where we are at boundary
    hitIt it1=it2;
    if(it2!=_hits.begin())--it1;
    //else std::cout<<"at beginning"<<std::endl;
    hitIt it3=it2;
    if(it2!=--_hits.end())++it3;
    //else std::cout<<"at end"<<std::endl;
    
    TVector3 pos1=(*it1)->cluster()->pos(); //next point
    TVector3 pos3=(*it3)->cluster()->pos();
    // construct general direction of track from these three
    
    outdir=(pos3-pos1);
    outdir.SetMag(1);
  }
  else {
    TVector3 pos=hit->cluster()->pos(); //next point
    TVector3 pos2=(*it2)->cluster()->pos();
    outdir=(pos2-pos);
    outdir.SetMag(1);
  }
  return it2;
}

//##########################################################################

void
PndTpcRiemannTrack::addHit(PndTpcRiemannHit* hit){
  int nbefore=_hits.size();
  _mcid.AddIDCollection(hit->cluster()->mcId(),1.);
  if(nbefore<2 || !_doSort){// first two hits hit
    _hits.push_back(hit);
    return;
  }
  // check if we start at end or at beginning:
  TVector3 posX=hit->cluster()->pos();
  std::list<PndTpcRiemannHit*>::iterator it2=_hits.end();
  --it2;
  TVector3 posend=(*it2)->cluster()->pos();
  std::list<PndTpcRiemannHit*>::iterator it1=_hits.begin();
  TVector3 posstart=(*it1)->cluster()->pos();
  double s1x=(posX-posstart).Mag();
  double s2x=(posX-posend).Mag();
  _nit=0; // reset iteration counter
  if(s1x>=s2x){// hit closer to end of list
    _hits.insert(this->sortHit(hit,_hits,-1),hit); // direction -1 --> start backwards search
  }
  else { // hit closer to start of list
    _hits.insert(this->sortHit(hit,_hits,1),hit); // direction 1 --> search from front to end
  }
  
  // update average
  _av*=(double)nbefore;
  _av[0]+=hit->x().X();
  _av[1]+=hit->x().Y();
  _av[2]+=hit->x().Z();
  _av*=1./(double)(nbefore+1);
  
}


#include "TCanvas.h"
#include "TApplication.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"
#include "TSystem.h"

// returns iterator BEFORE which to insert hitX!!!
// dir = 1 means start from beginning
// dir = -1 mean strart from end
hitIt
PndTpcRiemannTrack::sortHit(PndTpcRiemannHit* hitX, 
			    hitList& hL, int dir){
  // prepare iterators through list
  hitIt it2;
  if(dir>0){
    it2=hL.begin();
  }
  else {
    it2=hL.end(); 
    --it2;
  }

  TVector3 posX=hitX->cluster()->pos();
  TVector3 pos2=(*it2)->cluster()->pos(); //next point
  // find closest point to my hit
  int found=0;
  double dis=(pos2-posX).Mag();
  double mindis=dis;
  //std::cout<<"dir="<<dir<<std::endl;
  int it=0; // count iterations
  while(it<hL.size() && it2!=hL.end()){ // allow for some overstepping
    ++it;
    pos2=(*it2)->cluster()->pos(); //next point
    dis=(pos2-posX).Mag();
    //std::cout << dis << "   ";
    if(dis<mindis){
      found=1;
      mindis=dis;
    }
    else ++found; // record how far the last minimum is away
    if(it2==hL.begin()&& dir<0) {
      //std::cout<< "dropping out at begin";
      found-=1;
      
      break;
    }
    dir>0 ? ++it2 : --it2;
  }
  //if(it2==hL.end()&& dir>0){std::cout<< "dropping out at end";}
  //std::cout << std::endl << "mindis="<<mindis<< "   found="<<found<<std::endl;
  // step back to the minimum
  for(int step=0;step<found;++step) dir>0 ? --it2 : ++it2;
  pos2=(*it2)->cluster()->pos(); //next point
  dis=(pos2-posX).Mag();
  //std::cout << "   mini="<<dis<<std::endl;
  
  // now it2 is the hit with minimum distance
  // decide if to insert in front or behind:

  // catch the case where we are at boundary
  hitIt it1=it2;
  if(it2!=hL.begin())--it1;
  //else std::cout<<"at beginning"<<std::endl;
  hitIt it3=it2;
  if(it2!=--hL.end())++it3;
  //else std::cout<<"at end"<<std::endl;

  
  TVector3 pos1=(*it1)->cluster()->pos(); //next point
  TVector3 pos3=(*it3)->cluster()->pos();
// construct general direction of track from these three
  TVector3 d1=(pos3-pos1);
  TVector3 d2=(pos2-pos1);
  TVector3 dx=(posX-pos1);
  //d1.SetMag(1);
  //d2.SetMag(1);
  //dx.SetMag(1);
  TVector3 d=d1+d2+dx;
  if(d.Mag()==0){
    std::cout<<"d=0"<<std::endl;
    pos1.Print();
    pos2.Print();
    pos3.Print();
    posX.Print();
  }
  else d.SetMag(1);

  double dx1=(posX-pos1)*d;//.Mag();
  double dx2=(posX-pos2)*d;//.Mag();
  double dx3=(posX-pos3)*d;//.Mag();
  double d31=(pos3-pos1)*d;//.Mag();
  double d21=(pos2-pos1)*d;
 
  //-------------------------------------------------------------------
  /*
  TPolyMarker3D* hit1=new TPolyMarker3D(1);
  hit1->SetPoint(0,pos1.X(),pos1.Y(),pos1.Z());
  TPolyMarker3D* hit2=new TPolyMarker3D(1);
  hit2->SetPoint(0,pos2.X(),pos2.Y(),pos2.Z());
  TPolyMarker3D* hit3=new TPolyMarker3D(1);
  hit3->SetPoint(0,pos3.X(),pos3.Y(),pos3.Z());
  TCanvas* c=new TCanvas("c");
  TPolyMarker3D* maker=new TPolyMarker3D(hL.size());
  TPolyLine3D* line=new TPolyLine3D(hL.size());
  TPolyMarker3D* point=new TPolyMarker3D(1);
  point->SetPoint(0,posX.X(),posX.Y(),posX.Z());
  point->SetMarkerColor(kRed);
  point->SetMarkerStyle(23);
  TPolyLine3D* arrow=new TPolyLine3D(2);
  arrow->SetLineColor(kBlue);
  arrow->SetPoint(0,pos1.X(),pos1.Y(),pos1.Z());
  arrow->SetPoint(1,pos1.X()+d.X(),pos1.Y()+d.Y(),pos1.Z()+d.Z());
  
  std::list<PndTpcRiemannHit*>::iterator ait=hL.begin();
  int count=0;
  while(ait!=hL.end()){
    TVector3 pos=(*ait)->cluster()->pos();
    //pos.Print();
    maker->SetPoint(count,pos.X(),pos.Y(),pos.Z());
    line->SetPoint(count,pos.X(),pos.Y(),pos.Z());
    ++ait;
    ++count;
  }
  maker->SetMarkerStyle(23);
  maker->Draw();
  line->Draw();
  point->Draw();
  hit1->SetMarkerStyle(23);
  hit1->SetMarkerColor(kBlue);
  hit1->Draw();
  hit2->SetMarkerStyle(23);
  hit2->SetMarkerColor(kGreen);
  hit2->Draw();
  hit3->SetMarkerStyle(23);
  hit3->SetMarkerColor(kMagenta);
  hit3->Draw();
  arrow->Draw();
  gApplication->SetReturnFromRun(kTRUE);
  gSystem->Run();
  delete maker;
  delete line;
  delete point;
  delete hit1;
  delete arrow;
  delete c;

  //------------------------------------------------------
  */


  //check if inside 13
  if(dx1>0 && d31>dx1){ // different signs -> inside!
    //std::cout<<"X is inside 13!"<<std::endl;
    if(dx1*dx2<0){ // inside 12
      return it2;
    }
    else if(dx2*dx3<0){
      return it3; // inside 23
    }
  }
    if(dx1<0){// it is before 1
      //std::cout<<"x before x1"<<std::endl;
      return it1;
    }
    else {
      //std::cout<<"x after x3"<<std::endl;
      return ++it3; // insert before
    }
}

//double
//PndTpcRiemannTrack::s_hit(PndTpcRiemannHit* hit){
//  hitIt it=_hits.begin();
//  double s=0;
//  while()
//
//
//}


void
PndTpcRiemannTrack::trackpos(){
  // loop over hits in track and calculate their position
  hitIt it=_hits.begin();
  (*it)->setPosOnTrk(0);
  hitIt itlast=--_hits.end();
  while(it!=itlast){
    double s1=(*it)->s();
    TVector3 pos1=(*it)->cluster()->pos();
    ++it; // next hit
    TVector3 pos2=(*it)->cluster()->pos();
    (*it)->setPosOnTrk(s1+(pos2-pos1).Mag());
  }
}



// returns winding sense along z-axis
int
PndTpcRiemannTrack::winding(){
  hitIt it=_hits.begin();
  TVector3 pos1=(*it)->cluster()->pos();
  ++it;++it;
  TVector3 pos2=(*it)->cluster()->pos();
  it=--_hits.end();
  TVector3 pos3=(*it)->cluster()->pos();
  int dir= pos1.Mag()<pos3.Mag() ? 1 : -1; // correct for forward and backward going tracks pos=(0,0,0) corresponds to IP
  pos1.SetZ(0);
  pos2.SetZ(0);
  pos3.SetZ(0);

  TVector3 d12=pos2-pos1;
  TVector3 d21=pos3-pos1;
  double a=d12.DeltaPhi(d21);
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
PndTpcRiemannTrack::refit()
{
  
  TMatrixT<double> Av(3,1);
  Av[0][0]=_av[0];
  Av[1][0]=_av[1];
  Av[2][0]=_av[2];

  TMatrixD sampleCov(3,3);
  
  std::list<PndTpcRiemannHit*>::iterator it=_hits.begin();
  while(it!=_hits.end()){
    TMatrixD h(3,1);
    h[0][0]=(*it)->x().X();
    h[1][0]=(*it)->x().Y();
    h[2][0]=(*it)->x().Z();
    TMatrixD d(3,1);
    d=h-Av;
    TMatrixD dt(TMatrixD::kTransposed,d);
    TMatrixD ddt(d,TMatrixD::kMult,dt);
    sampleCov+=ddt;  
    ++it;
  }
  double nh=_hits.size();
  sampleCov*=1./nh;
  
  TVectorD eigenValues(3);
  TMatrixD eigenVec=sampleCov.EigenVectors(eigenValues);
    
  // eigenvalues are sorted according to their value
  // in descendign order -> last one is smallest
  _n=TMatrixDColumn(eigenVec,2);
 
  double norm=1./TMath::Sqrt(_n.Norm2Sqr());
  _n*=norm;
  _c=-1.*_n*_av;

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
  if(_c==0)return 0;
  if(fabs(_c)>1000)return 0;
  double a=2.*(_c+_n[2]);
  //if(a==0){
  //  std::cout<<"PndTpcRiemannTrack:: a==0 cannot calc r! set r=1E4"<<std::endl;
  //  return 1.E-4;
  //}
  //std::cout<<_n[0]<<"   "<<_n[1]<<"   "<<_c<<"   "<<a<<std::endl;
  double nom=_n[0]*_n[0]+_n[1]*_n[1]-2.0*_c*a;
  if(nom<0.0){
    nom=1.E-4;
    std::cout<<"PndTpcRiemannTrack::nom<0! set r=1E-4!"<<std::endl;
  }
  return sqrt(nom)/TMath::Abs(a);
}

void
PndTpcRiemannTrack::szFit(bool print){
  trackpos(); // calculate positions on track
  std::list<PndTpcRiemannHit*>::iterator it=_hits.begin();
  std::list<PndTpcRiemannHit*>::iterator lastit=it;
  if(!_isFitted){
    TGraph g(4);
    for(unsigned int i=0;i<4;++i){
      //(*it)->calcPosOnTrk((*lastit),false);
    g.SetPoint(i,(*it)->s(),(*it)->z());
    //std::cout<<_hits[i]->z()<<"|"<<_hits[i]->s()<<std::endl;
    lastit=it;
    ++it;
    }
    int errorcode;
    g.LeastSquareLinearFit(4,_t,_m,errorcode,-999,999);
    _isFitted=true;
  }
  
  TGraph g(getNumHits());
  // get s'es and zs
  it=_hits.begin();
  lastit=it;
  unsigned int nn=getNumHits();
  for(unsigned int i=0;i<nn;++i){
    //(*it)->calcPosOnTrk((*lastit),false);
    g.SetPoint(i,(*it)->s(),(*it)->z());
    lastit=it;
    ++it;
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
  return;
}

double
PndTpcRiemannTrack::szDist(PndTpcRiemannHit* hit, bool calcPos){
 if(!_isFitted)szFit();
 double hits=hit->s();
 //std::cout<<"raw s="<<hits<<std::endl;
 if(calcPos){
   // where does this hit belong?:
   // check if we start at end or at beginning:
   TVector3 posX=hit->cluster()->pos();
   std::list<PndTpcRiemannHit*>::iterator it2=_hits.end();
   --it2;
   TVector3 posend=(*it2)->cluster()->pos();
   std::list<PndTpcRiemannHit*>::iterator it1=_hits.begin();
   TVector3 posstart=(*it1)->cluster()->pos();
   double s1x=(posX-posstart).Mag();
   double s2x=(posX-posend).Mag();
   _nit=0; // reset iteration counter
   hitIt ahit;
   if(s1x>=s2x){// hit closer to end of list
     ahit=this->sortHit(hit,_hits,-1); // direction -1 --> start backwards search
   }
   else { // hit closer to start of list
     ahit=this->sortHit(hit,_hits,1); // direction 1 --> search from front to end
   }
   // ahit is the hit AFTER our hit ...
   bool first=(ahit==_hits.begin());
   for(int i=0;i<2;++i){ // TODO: Why do I have to increment twice????
     if(!first)--ahit;
     first=(ahit==_hits.begin());
   }

   TVector3 pos1=(*ahit)->cluster()->pos();
   if(first){
     hits=-((posX-pos1).Mag());  
   }
   else {
     double s1=(*ahit)->s();
     hits=s1+(posX-pos1).Mag();  
   }
   //std::cout<<"recalc s="<<hits<<std::endl;
 } // end recalcPos
 double predz=hits*_m+_t;
 return predz-hit->z();
}

// only after szFit!
double
PndTpcRiemannTrack::dip() const {
  return TMath::PiOver2()-(asin(_m));
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
  std::list<PndTpcRiemannHit*>::iterator it=_hits.begin();
  int count=0;
  while(it!=_hits.end()){
    TVector3 pos=(*it)->cluster()->pos();
    //pos.Print();
    maker->SetPoint(count,pos.X(),pos.Y(),pos.Z());
    line->SetPoint(count,pos.X(),pos.Y(),pos.Z());
    ++it;
    ++count;
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
