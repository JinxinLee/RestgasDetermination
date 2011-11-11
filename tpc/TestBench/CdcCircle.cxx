//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Class containing CDC Cirle 
//      
//
// Environment:
//      Software developed for the Prototype Detector at FOPI
//
// Author List:
//      Robert Münzer          TUM
//
//-----------------------------------------------------------

// C++ headers
#include <iostream>
#include <algorithm>
#include <vector>
#include <Rtypes.h>

# include "CdcCircle.h"

CdcCircle::CdcCircle(){
  fradius=0;
  fmidx=0;
  fmidy=0;
  fmidxtpc=0.;
  fmidytpc=0.;
  fradiustpcinner=5.;
  fradiustpcouter=15.;
    
}

void CdcCircle::SetRXY(Float_t rad,Float_t midx,Float_t midy){
  fradius=fabs(rad);
  fmidx=midx;
  fmidy=midy;
}

void CdcCircle::SetRXYP(Float_t rad,Float_t midx,Float_t midy,Float_t phi){
  fradius=fabs(rad);
  fmidx=midx;
  fmidy=midy;
  fphi=phi;
}
double CdcCircle::GetTpcLength(){
  Float_t centredistance = pow(pow(fmidxtpc-fmidx,2)+pow(fmidytpc-fmidy,2),0.5);
  
  if(centredistance>fradius+fradiustpcouter||centredistance<fradius-fradiustpcouter){
    ftpctracklength = 0;
  }
  else{
    if(centredistance>fradius+fradiustpcinner||centredistance<fradius-fradiustpcinner){
      ftpctracklength = fradius*asin(pow(pow(fradiustpcouter,2)-pow(fradius-centredistance,2),0.5)/(2*fradius));
    }
    else{
      ftpctracklength = fradius*(asin(pow(pow(fradiustpcouter,2)-pow(fradius-centredistance,2),0.5)/(2*fradius))-asin(pow(pow(fradiustpcinner,2)-pow(fradius-centredistance,2),0.5)/(2*fradius)));  
    }
  }
  return ftpctracklength;
}

double CdcCircle::MatPoint(TVector3 point,Float_t maxdist,Float_t maxphi){
  Float_t distr  = pow(pow((point.X()-fmidx),2)+pow((point.Y()-fmidy),2),0.5)-fradius;
  Float_t phitpcpoint;
  if(point.Y()<0)
    {
      phitpcpoint = atan2(point.Y(),point.X())+4*acos(0.);
    }
  else
    {
      phitpcpoint = atan2(point.Y(),point.X());
    }
  if(fabs(phitpcpoint-fphi)>maxphi) return(0.);
  if(fabs(distr)>maxdist) return(0.);
  
  return(fabs(phitpcpoint-fphi));

}
