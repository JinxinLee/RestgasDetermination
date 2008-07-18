//-----------------------------------------------------------
// File and Version Information:
// $Id: LinearInterpolPolicy.cc,v 1.4 2006/02/24 13:42:22 sneubert Exp $
//
// Description:
//      Implementation of class LinearIP
//      see LinearInterpolationPolicy.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "LinearInterpolPolicy.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include <iostream>

// Collaborating Class Headers --------
#include "PndTpcPoint.h"
#include "PndTpcPrimaryCluster.h"
#include "TRandom.h"
#include "TMath.h"
// Class Member definitions -----------

void
LinearInterpolPolicy::Interpolate(const PndTpcPoint* hit1, const PndTpcPoint* hit2, TClonesArray* ClusterList, unsigned int from, unsigned int n){
  if(n==0)return;
  //calculate direction and distance between hits
  double x=hit1->GetX();
  double y=hit1->GetY();
  double z=hit1->GetZ();
  double dx= hit2->GetX()-x;
  double dy= hit2->GetY()-y;
  double dz= hit2->GetZ()-z;
  double l=TMath::Sqrt(dx*dx+dy*dy+dz*dz);
  if(l<1E-12)l=1E-12;
  dx/=l;
  dy/=l;
  dz/=l;
  //average distance between clusters:
  double s=l/n;
  assert(from+n<=ClusterList->GetEntriesFast());
  double way=0.;
  for(unsigned int i=0;i<n;++i){
    double step=gRandom->Exp(s);
    	((PndTpcPrimaryCluster*)ClusterList->At(from+i))->setpos(x+=dx*step,y+=dy*step,z+=dz*step);
  }
};

