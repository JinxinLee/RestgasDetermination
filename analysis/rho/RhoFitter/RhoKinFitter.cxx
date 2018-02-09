#include <iostream>
#include "RhoKinFitter.h"
#include "RhoBase/RhoCandListIterator.h"
#include "RhoBase/RhoFactory.h"
#include "TDecompLU.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "RhoTools/RhoCalculationTools.h"
using namespace std;

ClassImp(RhoKinFitter)

TBuffer& operator>>(TBuffer& buf, RhoKinFitter *&obj)
{
  obj = (RhoKinFitter*) buf.ReadObject(RhoKinFitter::Class());
  return buf;
}

RhoKinFitter::RhoKinFitter( RhoCandidate* b) :
  RhoFitterBase( b )
{
  f4MomConstraint=-1;
  fMomConstraint=-1;
  fMassConstraint =-1;
  fTotMomConstraint=-1;
  fTotEConstraint=-1;
}

RhoKinFitter::~RhoKinFitter()
{
}


void RhoKinFitter::Add4MomConstraint(TLorentzVector lv)
{
  f4MomConstraint = 1;
  flmm=lv;
}

void RhoKinFitter::AddMomConstraint(TVector3 v)
{
  fMomConstraint = 1;
  fmm=v;
}

void RhoKinFitter::AddTotEConstraint(double energy)
{
  fTotEConstraint = 1;
  fEc=energy;
}
void RhoKinFitter::AddTotMomConstraint(double momentum)
{
  fTotMomConstraint = 1;
  fMom=momentum;
}
void RhoKinFitter::AddMassConstraint(double mass)
{
  fMassConstraint = 1;
  fMass=mass;
}

Bool_t RhoKinFitter::Fit()
{
  fDaughters.clear();
  FindAndAddFinalStateDaughters(fHeadOfTree);
  //int nd=fDaughters.size(); //unused?

  fNumCon=0;
  if(f4MomConstraint >0) {
    int n4Mom = 4;
    fNumCon = fNumCon + n4Mom;
  }
  if(fMomConstraint >0) {
    int nMom = 3;
    fNumCon = fNumCon + nMom;
  }
  if(fTotEConstraint >0) {
    int nE = 1;
    fNumCon = fNumCon + nE;
  }
  if(fMassConstraint >0) {
    int nMass = 1;
    fNumCon = fNumCon + nMass;
  }
  if(fTotMomConstraint >0) {
    int nTotMom = 1;
    fNumCon = fNumCon + nTotMom;
  }
  SetMatrices();
  ZeroMatrices();
  ReadMatrix();
  Bool_t check = Solve();
  SetOutput();
  return check;
}

void RhoKinFitter::SetMatrices()
{
  int nd=fDaughters.size();

  fNvar=7;
  fNpar = nd*fNvar;
  fNcon= fNumCon;
  fNc=0;
  fNiter=0;


  fAl0.ResizeTo(7*nd,1);
  fV_al0.ResizeTo(fNpar,fNpar);
  fAl1.ResizeTo(7*nd,1);
  fV_al1.ResizeTo(fNpar,fNpar);
  fmD.ResizeTo(fNcon,fNpar);
  fmE.ResizeTo(fNcon,3);
  fmd.ResizeTo(fNcon,1);
  fmPull.ResizeTo(7*nd,1);
}


void RhoKinFitter::ZeroMatrices()
{
  fAl0.Zero();
  fV_al0.Zero();
  fAl1.Zero();
  fV_al1.Zero();
  fmPull.Zero();
  fmD.Zero();
  fmd.Zero();
  fmE.Zero();
}


Bool_t RhoKinFitter::Solve()
{
  //int nd=fDaughters.size(); //unused?
  double ierr; // used to check inversions
  fAl1=fAl0;
//   int j1Max=50;
//  for(Int_t j1=0;j1<j1Max;++j1)
//  {
  if(f4MomConstraint >0) { Read4MomKinMatrix();}
  if(fMomConstraint >0) { ReadMomKinMatrix();}
  if(fTotEConstraint >0) { ReadTotEKinMatrix();}
  if(fMassConstraint >0) { ReadMassKinMatrix();}
  if(fTotMomConstraint >0) { ReadTotMomKinMatrix();}

  TMatrixD mD_t=fmD;
  mD_t.T();
//  mD_t=mD_t.Transpose(mD);

// mD_t.Print();
  TMatrixD Vd_inv = fmD*fV_al0*mD_t;

  TMatrixD Vd = Vd_inv.Invert(&ierr);
  //Vd.Print();
// TMatrixD lam=Vd*md;
  TMatrixD lam = Vd* ( fmD*(fAl1 - fAl0) + fmd);
  TMatrixD al_new=fAl0-fV_al0*mD_t*lam;
//  al_new.Print();
  TMatrixD V_al_new=fV_al0-fV_al0*mD_t*Vd*fmD*fV_al0;
  double chi2=0.;
  for (int i=0; i<fNcon; i++) { chi2+=lam[i][0]*fmd[i][0]; }
  fChiSquare=chi2;
  double covdif=(fV_al0[0][0]-V_al_new[0][0]);
  if (covdif > 0 ) {        fmPull[0][0] =(fAl0[0][0]-al_new[0][0])/sqrt(covdif);}
  fAl0=al_new;
  fV_al0=V_al_new;
  fPull=fmPull[0][0];
  return kTRUE;
}

//Write output
//Write output
void RhoKinFitter::SetOutput()
{
  int nd=fDaughters.size();
  TMatrixD m(nd,1);
  fNDegreesOfFreedom = 0;
  if(f4MomConstraint >0) 
    fNDegreesOfFreedom += 4;
  else {
    if(fMomConstraint >0) 
      fNDegreesOfFreedom += 3;
    if(fTotEConstraint >0 || fMassConstraint >0 || fTotMomConstraint >0) 
      fNDegreesOfFreedom += 1;
  }
  double  sumA=0;
  double a;

  for (int k=0; k<nd; k++) {
    Double_t bField = 0.1*RhoCalculationTools::GetBz(fDaughters[k]->Pos()); // T, assume field in z only
    a = -0.00299792458*bField*fDaughters[k]->GetCharge();
    sumA += a;
    m[k][0]=fDaughters[k]->P4().M();
    TVector3 pos(fAl0[k*7+4][0],fAl0[k*7+5][0],fAl0[k*7+6][0]);
    TLorentzVector mom4(fAl0[k*7+0][0],fAl0[k*7+1][0],fAl0[k*7+2][0],fAl0[k*7+3][0]);
    TLorentzVector p1;
    p1.SetXYZM(fAl0[k*7+0][0],fAl0[k*7+1][0],fAl0[k*7+2][0],m[k][0]);
    // fDaughters[k]->SetP4(p1);
    //fDaughters[k]->SetP7(pos,mom4);
    fDaughters[k]->SetP7(pos,p1);
      

    TMatrixD p1Cov(7,7);
    for(int i=0; i<7; i++) {
      for (int j=0; j<7; j++) {
        
        p1Cov[i][j]= fV_al0[k*7+i][k*7+j];
        fDaughters[k]->SetCov7(p1Cov); //New covariance matrix without correlations
      }
    }
  }

// For the composite particle ..............................
//  double fpx=0,fpy=0,fpz=0,fe=0;
//  for (int k=0; k<nd; k++) {
//    fpx+= fAl0[k*7+0][0];
//    fpy+= fAl0[k*7+1][0];
//    fpz+= fAl0[k*7+2][0];
//    fe += fAl0[k*7+3][0];
//    //   fe +=fDaughters[k]->P4().E(); // Energy from the initial daughter particles
//  }
//  TLorentzVector sum(fpx,fpy,fpz,fe);
//  fHeadOfTree->SetP4(sum);

  //iteratively set the foumomenta in the tree
  SetFourMomentumByDaughters(fHeadOfTree);
  return;
}



//Read the input vector
void RhoKinFitter::ReadMatrix()
{
  int nd =fDaughters.size();
  for (int k=0; k<nd; k++) {
    int kN=k*7;
//px,py,pz,E,x,y,z
    TLorentzVector p1=fDaughters[k]->P4();
    TVector3 p2=fDaughters[k]->Pos();
    fAl0[kN+0][0]=p1.X();
    fAl0[kN+1][0]=p1.Y();
    fAl0[kN+2][0]=p1.Z();
//    al0[kN+3][0]=p1.E();
    fAl0[kN+4][0]=p2.X();
    fAl0[kN+5][0]=p2.Y();
    fAl0[kN+6][0]=p2.Z();

    double fm=fDaughters[k]->Mass();
    fAl0[kN+3][0]=sqrt(fAl0[kN+0][0]*fAl0[kN+0][0]+ fAl0[kN+1][0]*fAl0[kN+1][0]+fAl0[kN+2][0]*fAl0[kN+2][0]+fm*fm);

// Read Covariance Matrix .... Can read 6x6 matrices..................
    TMatrixD p1Cov(7,7);
    TMatrixD p2Cov(6,6);
    TMatrixD p3Cov(7,7);
    TMatrixD p4Cov(7,7);
    p1Cov=fDaughters[k]->Cov7(); //Cov Matrix x,y,z,px,py,pz,E

    for (int ii=0; ii<6; ii++) {for(int jj=0; jj<6; jj++) {p2Cov[ii][jj]=p1Cov[ii][jj];}} //test

    //Extend matrix for energy for each candidates .....6x6 to 7x7

    TMatrixD J(7,6) ;
    J.Zero();
    TMatrixD J_t(6,7);
    for (int ii=0; ii<6; ii++) {for(int jj=0; jj<6; jj++) {J[ii][jj] = 1;}}
    for(int i=3; i<6; ++i) {J[6][i] = fAl0[kN+i-3][0]/fAl0[kN+3][0];}
//  for (int i=3; i <6; i++) { J[6][i] = -al0[kN+i-3][0]/fm;}
//        J[6][6] =al0[kN+6-3][0] /fm;
//   p3Cov= J*p2Cov*(J_t.Transpose(J));
    p3Cov=p1Cov;

//Change to px,py,pz,E,x,y,z
    for(int i=0; i<7; i++) {
      for(int j=0; j<7; j++) {
        if(i>=3) {
          if(j>=3) {
            p4Cov[i-3][j-3] = p3Cov[i][j];
          } else { p4Cov[i-3][j+4] = p3Cov[i][j]; }
        } else {
          if(j>=3) {
            p4Cov[i+4][j-3] = p3Cov[i][j];
          } else { p4Cov[i+4][j+4] = p3Cov[i][j]; }
        }
      }
    }

// cout<<"p2Cov"<<endl;
    // p2Cov.Print();
    for(int i=0; i<7; i++) {
      for (int j=0; j<7; j++) {
        fV_al0[k*7+i][k*7+j]  = p4Cov[i][j];
      }
    }
  }
}


void RhoKinFitter::ReadMassKinMatrix()
{
  int nd=fDaughters.size();

  double Etot = 0.;
  double Px = 0.;
  double Py = 0.;
  double Pz = 0.;

  TMatrixD al1p(fAl1);
  double a;
  TMatrixD m(nd,1);
  for(int k=0; k<nd; ++k) {
    int kN=k*7;
    TLorentzVector p1=fDaughters[k]->P4();
    m[k][0]=p1.M();

    double px = al1p[kN+0][0];
    double py = al1p[kN+1][0];
    double pz = al1p[kN+2][0];
    double E = TMath::Sqrt(px*px+py*py+pz*pz+m[k][0]*m[k][0]);
    Etot += E;
    Px   +=px;
    Py +=py;
//   Px += (px - a*delY);
    //  Py += (py + a*delX);
    Pz += pz;
  }

  fmd[fNc+0][0] = Etot*Etot - Px*Px - Py*Py - Pz*Pz - fMass*fMass ;

  for(int k=0; k<nd; ++k) {
    int kN=k*7;
    TLorentzVector p1=fDaughters[k]->P4();
    m[k][0]=p1.M();
    //double px = al1p[kN+0][0]; //[R.K. 01/2017] unused variable?
    //double py = al1p[kN+1][0]; //[R.K. 01/2017] unused variable?
    //double pz = al1p[kN+2][0]; //[R.K. 01/2017] unused variable?
    //double E = TMath::Sqrt(px*px+py*py+pz*pz+m[k][0]*m[k][0]); //[R.K. 01/2017] unused variable?
    Double_t bField = 0.1*RhoCalculationTools::GetBz(fDaughters[k]->Pos()); // T, assume field in z only
    a = -0.00299792458*bField*fDaughters[k]->GetCharge();
    //Double_t invE = 1./E;

//....................................................
// V.J. - force head mass to constraint mass
fmD[fNc+0][kN+0] = -2.*Px;
fmD[fNc+0][kN+1] = -2.*Py;;
fmD[fNc+0][kN+2] = -2.*Pz;
fmD[fNc+0][kN+3] = 2.*Etot;
fmD[fNc+0][kN+4] = 2.*a*Py;
fmD[fNc+0][kN+5] = -2.*a*Px;
fmD[fNc+0][kN+6] = 0.0;
//....................................................
//    fmD[fNc+0][kN+0] = 2.*(Etot*px*invE-Px);
//    fmD[fNc+0][kN+1] = 2.*(Etot*py*invE-Py);
//    fmD[fNc+0][kN+2] = 2.*(Etot*pz*invE-Pz);
//    fmD[fNc+0][kN+3] = 0.0;
//    fmD[fNc+0][kN+3] = 2* m[k][0]*Etot*invE;
//    //  mD[0][kN+4] = 2.*(Etot*py*invE-Py)*a;
//    // mD[0][kN+5] = 2.*(Etot*px*invE-Px)*a;
//    fmD[fNc+0][kN+4] = 2.*Py*a;
//    fmD[fNc+0][kN+5] = -2.*Px*a;
//    fmD[fNc+0][kN+6] = 0.0;
//................Simple....................
    /*
                  mD[fNc+0][kN+0] = -2.*Px;
                  mD[fNc+0][kN+1] = -2.*Py;;
                  mD[fNc+0][kN+2] = -2.*Pz;
                  mD[fNc+0][kN+3] = 2.*Etot;
          //      mD[fNc+0][kN+3] = 2* m[k][0]*Etot*invE;
          //      mD[fNc+0][kN+4] = 2.*(Etot*py*invE-Py)*a;
          //      mD[fNc+0][kN+5] = 2.*(Etot*px*invE-Px)*a;
                  mD[fNc+0][kN+4] = 0.0;
                  mD[fNc+0][kN+5] = 0.0;
                  mD[fNc+0][kN+6] = 0.0;
    */
//.....................................

  }
  fNc += 1;
}

void RhoKinFitter::Read4MomKinMatrix()
{
  int nd=fDaughters.size();
  TMatrixD alp(fAl1);
  TMatrixD m(nd,1);
  int k,i;
  for (k=0; k<nd; k++) {
    TLorentzVector p1=fDaughters[k]->P4();
    m[k][0]=p1.M();
    double E =sqrt(alp[k*7+0][0]*alp[k*7+0][0]+
                   alp[k*7+1][0]*alp[k*7+1][0]+
                   alp[k*7+2][0]*alp[k*7+2][0]+
                   m[k][0]*m[k][0]);
    for (i=0; i<3; i++) {
      fmD[fNc+i][k*7+i]   = 1;
      fmD[fNc+3][k*7+i]   = alp[k*7+i][0]/E;
    }
//   cout << "value of D" <<  mD[3][k*7+i] << endl;

    for (i=0; i<3; i++) {
      fmd[fNc+i][0] += alp[k*7+i][0];
    }
    fmd[fNc+3][0] += E;
  }
  fmd[fNc+0][0] -= flmm.X();
  fmd[fNc+1][0] -= flmm.Y();
  fmd[fNc+2][0] -= flmm.Z();
  fmd[fNc+3][0] -= flmm.T();
  fNc += 4;
}


void RhoKinFitter::ReadMomKinMatrix()
{
  int nd=fDaughters.size();
  TMatrixD alp(fAl1);
  TMatrixD m(nd,1);
  int k,i;
  for (k=0; k<nd; k++) {
    for (i=0; i<3; i++) {
      fmD[fNc+i][k*7+i]   = 1;
    }
    for (i=0; i<3; i++) {
      fmd[fNc+i][0] += alp[k*7+i][0];
    }
  }
  fmd[fNc+0][0]-= fmm.X();
  fmd[fNc+1][0]-= fmm.Y();
  fmd[fNc+2][0]-= fmm.Z();

  fNc += 3;
}


void RhoKinFitter::ReadTotEKinMatrix()
{
  int nd=fDaughters.size();
  TMatrixD alp(fAl1);
  TMatrixD m(nd,1);
  int k,i;
  for (k=0; k<nd; k++) {
    TLorentzVector p1=fDaughters[k]->P4();
    m[k][0]=p1.M();
    double E =sqrt(alp[k*7+0][0]*alp[k*7+0][0]+
                   alp[k*7+1][0]*alp[k*7+1][0]+
                   alp[k*7+2][0]*alp[k*7+2][0]+
                   m[k][0]*m[k][0]);
    for (i=0; i<3; i++) {
      fmD[fNc+0][k*7+i]   = 0.;
      // mD[0][k*7+i]  = alp[k*7+i][0]/E;
    }
    fmD[fNc+0][k*7+3]   = 1.;
    fmd[fNc+0][0] += E;
  }
  fmd[fNc+0][0] -= fEc;
  fNc +=1;
}

void RhoKinFitter::ReadTotMomKinMatrix()
{
  int nd=fDaughters.size();
  TMatrixD alp(fAl1);
  TMatrixD m(nd,1);
  int k,i;
  for (k=0; k<nd; k++) {
    double Ptot =sqrt(alp[k*7+0][0]*alp[k*7+0][0]+
                      alp[k*7+1][0]*alp[k*7+1][0]+
                      alp[k*7+2][0]*alp[k*7+2][0]);
    for (i=0; i<3; i++) {
      // mD[0][k*7+i]   = 1.;
      fmD[fNc+0][k*7+i]  = alp[k*7+i][0]/Ptot;
    }
    fmd[fNc+0][0] += Ptot;
  }
  fmd[fNc+0][0] -= fMom;
  fNc +=1;
}

/*
void RhoKinFitter::ReadEqMassKinMatrix()
{int nd=fDaughters.size();
 TMatrixD alp(al1);
 TMatrixD m(nd,1);
 int k,i,j;
 for (k=0;k<nd;k++)
{
   TLorentzVector p1=fDaughters[k]->P4();
   double Mtot =sqrt(alp[k*7+3][0]*alp[k*7+3][0]
                -(alp[k*7+0][0]*alp[k*7+0][0]+
                 alp[k*7+1][0]*alp[k*7+1][0]+
                 alp[k*7+2][0]*alp[k*7+2][0])
  for (i=0;i<3;i++)
     {
     // mD[0][k*7+i]   = 1.;
     mD[fNc+0][k*7+i]  -= 2.*alp[k*7+i][0];
     mD[fNc+0][k*7+3]  -= -2.*alp[k*7+3][0];
    }
    md[fNc+0][0] -= Mtot;
   }
   md[fNc+0][0] -= 0.0;
  fNc +=1;
}
*/

