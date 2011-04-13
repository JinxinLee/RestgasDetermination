//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRiemannHit
//      see PndTpcRiemannHit.hh for details
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
#include "PndTpcRiemannHit.h"
#include "TMath.h"
// C/C++ Headers ----------------------
#include "PndTpcCluster.h"
#include <assert.h>
#include <iostream>
#include <cmath>

// Collaborating Class Headers --------
#include "PndTpcRiemannTrack.h"

// Class Member definitions -----------


// scale factor! r=1 => z'=0.5 !!!
// to normalize cm-range 
// divide radius by RIEMANNSCALE // also needed in RiemannTrack->r()
// 8.66025 is optimal for R=5..15cm
// 24.6 for R=15..42cm

ClassImp(PndTpcRiemannHit)


PndTpcRiemannHit::PndTpcRiemannHit(double r, double phi, double scale)
: _cluster(NULL),_s(0.),_alpha(0.),fRiemannScale(scale)
{
  double r2=r*r;
  double d=1+r2;
  _x.SetX(r*TMath::Cos(phi)/d);
  _x.SetY(r*TMath::Sin(phi)/d);
  _x.SetZ(r2/d);
}

PndTpcRiemannHit::PndTpcRiemannHit(PndTpcCluster* cl,double scale)
  : _cluster(cl),_s(0.),_alpha(0.),fRiemannScale(scale)
{
  TVector2 a(cl->pos().X(),cl->pos().Y());
  double r = a.Mod()/fRiemannScale;
  double phi=a.Phi();
  double r2=r*r;
  double d=1+r2;
  _x.SetX(r*TMath::Cos(phi)/d);
  _x.SetY(r*TMath::Sin(phi)/d);
  _x.SetZ(r2/d);
}


PndTpcRiemannHit::~PndTpcRiemannHit(){}

double
PndTpcRiemannHit::z()const {
  if(_cluster==NULL){
    std::cout<<"PndTpcRiemannHit::z() No cluster available!!!"<<std::endl;
    return 0;
  }
  else return _cluster->pos().Z();
}

double
PndTpcRiemannHit::calcPosOnTrk(PndTpcRiemannHit* hit, bool refine){
  assert(hit!=NULL);
  this->_s=0; // reset
  _s=hit->s()+(hit->cluster()->pos()-this->cluster()->pos()).Mag();
  /*
  TVectorD o=trk->orig(); 
  o*=fRiemannScale;// convert back to cm;
  double r=trk->r();
  r*=fRiemannScale;
  const PndTpcCluster* firstCl=trk->getHit(0)->cluster();
  assert(firstCl!=NULL);
  TVector2 k(firstCl->pos().X()-o[0],firstCl->pos().Y()-o[1]);
  TVector2 l(_cluster->pos().X()-o[0],_cluster->pos().Y()-o[1]);
  _alpha=l.DeltaPhi(k);
  _s=_alpha*r; 
  if(refine){
    // minimize sz-dist
    double olddist=trk->szDist(this,false);
    // try one step:
    _s+=2.0*TMath::Pi();
    double newdist1=trk->szDist(this,false);
    _s-=4.0*TMath::Pi();
    double newdist2=trk->szDist(this,false);
    // first hypothesis: back step was correct
    double newdist=newdist2;
    double sign=-1;
    if(newdist1<newdist2){ // is first hypo wrong?
      sign=1;
      _s+=4.0*TMath::Pi();
      newdist=newdist1;
    }
    int count=0;
    while (fabs(olddist)>=fabs(newdist) && count<5){
      ++count;
      olddist=newdist;
      _s+=sign*2.0*TMath::Pi();
      newdist=trk->szDist(this,false);
    }
    // here we went one step too far. So we have to go back 360 degrees:
    _s-=sign*2.0*TMath::Pi();
  }
  */
  return _s;
}
