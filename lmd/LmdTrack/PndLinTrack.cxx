// -------------------------------------------------------------------------
// -----                    PndLinTrack source file                -----
// -----                                 -----
// -------------------------------------------------------------------------

#include "stdlib.h"
#include <iostream>
#include "PndLinTrack.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TMath.h"
#include <cmath>

// -----   Default constructor   -------------------------------------------
PndLinTrack::PndLinTrack() {
  // std::cout<<"/lmd/LmdTrack/PndLinTrack()"<<std::endl;
  //fCovMatrix = new TMatrixDSym(6);
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
PndLinTrack::PndLinTrack(TString detName, Double_t p0, Double_t p1, Double_t p2, Double_t p3, Double_t p4, Double_t p5, 
			 Double_t chi, Int_t first, Int_t last, Int_t cand){
  fDetName = detName;
  fP0 = p0; fP1 = p1; fP2 = p2; fP3 = p3; fP4 = p4; fP5 = p5;
  fChiSq = chi;
  fFirst = first; fLast = last;
  fCandId = cand;
  // fP0err = p0err; fP1err = p1err; fP2err = p2err; fP3err = p3err;
  //fz0 = z0;
  // fCovMatrix = NULL;
  fCovMatrix.ResizeTo(6,6);
  // fCovMatrix = TMatrixDSym(6);
}

TVector3 PndLinTrack::GetDirectionVec(){
  double l = 1./sqrt(1+fP1*fP1+fP3*fP3);
  return TVector3(fP1*l,fP3*l,l);
}

TVector3 PndLinTrack::GetStartErrVec(){
  return TVector3(TMath::Sqrt((fCovMatrix)(0,0)),TMath::Sqrt((fCovMatrix)(2,2)),TMath::Sqrt((fCovMatrix)(4,4)));
}

TVector3 PndLinTrack::GetDirectionErrVec(){
 double l = 1./sqrt(1+fP1*fP1+fP3*fP3);
 // std::cout<<"l = "<<l<<" pow(l,3) = "<<pow(l,3)<<" p1="<<fP1<<" p3="<<fP3<<std::endl;
 double ddx_dp1=(1+fP3*fP3);
 double ddx_dp3=fP1*fP3;
 double err_dx = pow(l,3)*sqrt(ddx_dp1*ddx_dp1*((fCovMatrix)(1,1))+ddx_dp3*ddx_dp3*((fCovMatrix)(3,3))
		      +fabs(2*ddx_dp1*ddx_dp3*((fCovMatrix)(1,3))));
 // std::cout<<"err_dx = "<<ddx_dp1*ddx_dp1*((fCovMatrix)(1,1))<<" + "<<ddx_dp3*ddx_dp3*((fCovMatrix)(3,3))
 // 	  <<" + "<<2*ddx_dp1*ddx_dp3*((fCovMatrix)(1,3))<<" = "<<err_dx<<std::endl;

 double ddy_dp1=fP1*fP3;
 double ddy_dp3=(1+fP1*fP1);
 double err_dy =  pow(l,3)*sqrt(ddy_dp1*ddy_dp1*((fCovMatrix)(1,1))+ddy_dp3*ddy_dp3*((fCovMatrix)(3,3))
		      +fabs(2*ddy_dp1*ddy_dp3*fabs((fCovMatrix)(1,3))));

 double ddz_dp1=fP1;
 double ddz_dp3=fP3;
 double err_dz =pow(l,3)*sqrt(ddz_dp1*ddz_dp1*((fCovMatrix)(1,1))+ddz_dp3*ddz_dp3*((fCovMatrix)(3,3))
		      +fabs(2*ddz_dp1*ddz_dp3*fabs((fCovMatrix)(1,3))));
 return TVector3(err_dx,err_dy,err_dz);
}

void PndLinTrack::GetParErr(Double_t* errpar){ 
  errpar[0] = TMath::Sqrt((fCovMatrix)(0,0));
  errpar[1] = TMath::Sqrt((fCovMatrix)(1,1));
  errpar[2] = TMath::Sqrt((fCovMatrix)(2,2));
  errpar[3] = TMath::Sqrt((fCovMatrix)(3,3));
  errpar[4]=  TMath::Sqrt((fCovMatrix)(4,4));
  errpar[5]=  TMath::Sqrt((fCovMatrix)(5,5));
}

// -----   Destructor   ----------------------------------------------------
PndLinTrack::~PndLinTrack() {}

// -------------------------------------------------------------------------
ClassImp(PndLinTrack);
