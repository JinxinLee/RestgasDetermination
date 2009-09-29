#include "PndTpcMCTracklet.h"
#include <TMath.h>
#include <cmath>

ClassImp(PndTpcMCTracklet)


Int_t 
PndTpcMCTracklet::nmvdAcc(Double_t cut){
  unsigned int n=fmvdhits.size();
  Int_t found=0;
  for(unsigned int i=0;i<n;++i){
    if(fabs(fmvdhits[i]<cut))++found;
  }
  return found;
}



 bool 
 PndTpcMCTrackletSort::operator() (PndTpcMCTracklet* t1, PndTpcMCTracklet* t2){
   TVector3 pos1=t1->pos();
   TVector3 pos2=t2->pos();

   // sort according to (if z1-z2>5cm -> t1>t2)
   // (theta1-theta2>5° -> t1>t2)
   // (r1-r2>5cm ->t1>t2);

   bool result=false;
   if(pos1.Z()-pos2.Z()>5)return true;
   else if(pos2.Z()-pos1.Z()>5)return false;
   else { // we are in same z bin
     if(pos1.DeltaPhi(pos2)>0.0873)return true; // 5degrees
     else if(pos2.DeltaPhi(pos1)>0.0873)return false;
     else { // we are also in same angle bin
       if(pos1.Perp()-pos2.Perp()>5)return true;
       else return false;
     }
   }

 }
