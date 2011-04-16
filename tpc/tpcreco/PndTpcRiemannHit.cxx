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

