#include "ExtrapolateToPCA.h"

#include "TVector3.h"

#include "Track.h"


double trkDist(Track* trk1, Track* trk2){
  TVector3 d=trk1->getPos()-trk2->getPos();
  return d.Mag();
}



void ExtrapolateToPCA(Track* trk1, Track* trk2){
  // TODO: make accuracy configurable!
  // a simple newtonian search.
  double h=0.01;
  double m1=999999;
  double s1=0;
  int steps=0;
  double oldd=999999;
  double d=99999;
  while(oldd>d && steps<100){
    trk1->stepalong(s1);
    trk2->gotoPoint(trk1->getPos());
    oldd=d;
    ++steps;

    d=trkDist(trk1,trk2);
    trk1->stepalong(h);
    trk2->gotoPoint(trk1->getPos());
    double d1=trkDist(trk1,trk2);
    
    m1=(d1-d)/h;
    if(TMath::Abs(m1)<1E-4)break;
    s1=-d/m1;
    // limit stepsize to max 180cm
    if(TMath::Abs(s1)>200)s1= s1>0 ? 180 : -180;
    //std::cout<<"d="<<d<<"  d1="<<d1<<"  m1="<<m1<<"  s1="<<s1<<std::endl;
    
  }
  trk1->stepalong(s1);
  trk2->gotoPoint(trk1->getPos());

  // three points -> fit a parabola -> minimum
  h=1.;
  double d0=trkDist(trk1,trk2);
  trk1->stepalong(h);
  trk2->gotoPoint(trk1->getPos());
  double d1=trkDist(trk1,trk2);
  trk1->stepalong(-2.*h);
  trk2->gotoPoint(trk1->getPos());
  double d2=trkDist(trk1,trk2);

  double s=(d2-d1)*h;
  double denom=2.*(d1+d2-2*d0);
  if(denom==0)return;
  s/=denom;

  //std::cout<<"d0="<<d0<<"  d1="<<d1<<"  d2="<<d2<<"   s="<<s<<std::endl;

  trk1->stepalong(s+h);
  trk2->gotoPoint(trk1->getPos());

  return;
}


