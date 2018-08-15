#include <iostream>
#include "RhoKinHyperonFitter.h"
#include "RhoBase/RhoCandListIterator.h"
#include "RhoBase/RhoFactory.h"
#include "TDecompLU.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "RhoTools/RhoCalculationTools.h"
using namespace std;

ClassImp(RhoKinHyperonFitter)

TBuffer& operator>>(TBuffer& buf, RhoKinHyperonFitter *&obj)
{
  obj = (RhoKinHyperonFitter*) buf.ReadObject(RhoKinHyperonFitter::Class());
  return buf;
}

RhoKinHyperonFitter::RhoKinHyperonFitter( RhoCandidate* b) :
  RhoFitterBase( b )
{
  f4MomConstraint=-1;
  fMomConstraint=-1;
  fMassConstraint =-1;
  fTotMomConstraint=-1;
  fTotEConstraint=-1;
}

RhoKinHyperonFitter::~RhoKinHyperonFitter()
{
}


void RhoKinHyperonFitter::Add4MomConstraint(TLorentzVector lv)
{
  f4MomConstraint = 1;
  flmm=lv;
}

void RhoKinHyperonFitter::AddMomConstraint(TVector3 v)
{
  fMomConstraint = 1;
  fmm=v;
}

void RhoKinHyperonFitter::AddTotEConstraint(double energy)
{
  fTotEConstraint = 1;
  fEc=energy;
}
void RhoKinHyperonFitter::AddTotMomConstraint(double momentum)
{
  fTotMomConstraint = 1;
  fMom=momentum;
}
void RhoKinHyperonFitter::AddMassConstraint(double mass)
{
  fMassConstraint = 1;
  fMass=mass;
}

Bool_t RhoKinHyperonFitter::Fit()
{
  fDaughters.clear();
  //FindAndAddFinalStateDaughters(fHeadOfTree);
  SetDaugthersFromComposite(fHeadOfTree);
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

if(fMassConstraint >0|| fMomConstraint>0)
{
  SetMatrices();
  ZeroMatrices();
  ReadMatrix();
}
  Bool_t check = Solve();
  SetOutput();
  return check;
}

void RhoKinHyperonFitter::SetMatrices()
{
  int nd;
 
  if(fMassConstraint>0 || fMomConstraint>0) {nd=1;}
  else{
  fDaughters.clear();
  //FindAndAddFinalStateDaughters(fHeadOfTree);
  SetDaugthersFromComposite(fHeadOfTree);
  nd=fDaughters.size(); 
  //cout<<"SetMatrix nd="<<nd<<endl;
  }
 
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


void RhoKinHyperonFitter::ZeroMatrices()
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


Bool_t RhoKinHyperonFitter::Solve()
{
  double ierr; // used to check inversions
  fAl1=fAl0;
  TMatrixD chi2_0(1,1);
  chi2_0[0][0]=10000000.;// Xinying Song added a very large initial chi2
  int j1Max=40;

  for(Int_t j1=0;j1<j1Max;++j1)
  {
  if(f4MomConstraint >0) { Read4MomKinMatrix();}
  if(fMomConstraint >0) { ReadMomKinMatrix();}
  if(fTotEConstraint >0) { ReadTotEKinMatrix();}
  if(fMassConstraint >0) { ReadMassKinMatrix();}
  if(fTotMomConstraint >0) { ReadTotMomKinMatrix();}

  
  TMatrixD mD_t=fmD;
  mD_t.T();

  TMatrixD Vd_inv = fmD*fV_al0*mD_t;
  TMatrixD Vd = Vd_inv.Invert(&ierr);
  TMatrixD lam = Vd* ( fmD*(fAl0 - fAl1) + fmd);
  TMatrixD lam_t=lam;
  lam_t.T();
  TMatrixD al_new=fAl0-fV_al0*mD_t*lam;
  TMatrixD V_al_new=fV_al0-fV_al0*mD_t*Vd*fmD*fV_al0;
//  TMatrixD chi2_new = lam_t*(fmD*(fAl0-fAl1)+fmd );
  TMatrixD chi2_new = lam_t*(fmD*(fAl0-al_new)+fmd );

  if(TMath::IsNaN(chi2_new[0][0])) continue;
  double delchi2 = chi2_0[0][0]- chi2_new[0][0];


 if( chi2_new[0][0] < chi2_0[0][0]|| fabs(delchi2)<1.){
   fAl1= al_new;
 //  TMatrixD V_al_new=fV_al0-fV_al0*mD_t*Vd*fmD*fV_al0;
 // fV_al1= V_al_new;
   chi2_0[0][0]= chi2_new[0][0];
  fChiSquare=chi2_new[0][0];
  if(fabs(delchi2)<0.01||j1==j1Max-1){ fV_al1= V_al_new; break;}
   }
}

fAl0=fAl1;
fV_al0=fV_al1;
return kTRUE;


}

//Write output
void RhoKinHyperonFitter::SetOutput()
{


  fNDegreesOfFreedom = 0;
  if(f4MomConstraint >0) 
    //fNDegreesOfFreedom += 4+fDaughters.size();
    fNDegreesOfFreedom += 4;
  else {
    if(fMomConstraint >0) 
      fNDegreesOfFreedom += 3;
    if(fTotEConstraint >0 || fMassConstraint >0 || fTotMomConstraint >0) 
      fNDegreesOfFreedom += 1;
  }



if(fMassConstraint>0 || fMomConstraint>0){
  int nd=1;
  TMatrixD m(nd,1);
    TVector3 pos(fAl0[4][0],fAl0[5][0],fAl0[6][0]);
    TLorentzVector mom4(fAl0[0][0],fAl0[1][0],fAl0[2][0],fAl0[3][0]);
    fHeadOfTree->SetP7(pos,mom4);
      

    TMatrixD p1Cov(7,7);
    TMatrixD HeadCov(7,7);
    for(int i=0; i<7; i++) {
      for (int j=0; j<7; j++) {
        
        p1Cov[i][j]= fV_al0[i][j];
        //fHeadOfTree->SetCov7(p1Cov); //New covariance matrix without correlations
      }
    }
  
//Change from px,py,pz,E,x,y,z
    //         to x,y,z,px,py,pz,E
    for(int i=0; i<7; i++) {
      for(int j=0; j<7; j++) {
        if(i>=4) {
          if(j>=4) {
            HeadCov[i-4][j-4] = p1Cov[i][j];
          } else { HeadCov[i-4][j+3] = p1Cov[i][j]; }
        } else {
          if(j>=4) {
            HeadCov[i+3][j-4] = p1Cov[i][j];
          } else { HeadCov[i+3][j+3] = p1Cov[i][j]; }
        }
      }
    }


fHeadOfTree->SetCov7(HeadCov);
  return;

}else{//in the 4C case, we need to output the p7 and cov7 of the daughters
    int nd=fDaughters.size();
    TMatrixD m(nd,1);
    TLorentzVector tot(0,0.0,0.0,0.0);
    for (int k=0; k<nd; k++) {
    TLorentzVector p1=fDaughters[k]->P4();
    m[k][0]=p1.M();
    TVector3 pos(fAl0[k*7+4][0],fAl0[k*7+5][0],fAl0[k*7+6][0]);
    double E =sqrt(fAl0[k*7+0][0]*fAl0[k*7+0][0]+
                   fAl0[k*7+1][0]*fAl0[k*7+1][0]+
                   fAl0[k*7+2][0]*fAl0[k*7+2][0]+
                   m[k][0]*m[k][0]);

    TLorentzVector mom4(fAl0[k*7+0][0],fAl0[k*7+1][0],fAl0[k*7+2][0],E);
    fDaughters[k]->SetP7(pos,mom4);


    TMatrixD p1Cov(7,7);
    TMatrixD p2Cov(7,7);
    for(int i=0; i<7; i++) {
      for (int j=0; j<7; j++) {

        p1Cov[i][j]= fV_al0[k*7+i][k*7+j];
  //      fDaughters[k]->SetCov7(p1Cov); //New covariance matrix without correlations
      }
    }

//Change from px,py,pz,E,x,y,z
//         to x,y,z,px,py,pz,E

    for(int i=0; i<7; i++) {
      for(int j=0; j<7; j++) {
        if(i>=4) {
          if(j>=4) {
            p2Cov[i-4][j-4] = p1Cov[i][j];
          } else { p2Cov[i-4][j+3] = p1Cov[i][j]; }
        } else {
          if(j>=4) {
            p2Cov[i+3][j-4] = p1Cov[i][j];
          } else { p2Cov[i+3][j+3] = p1Cov[i][j]; }
        }
      }
    }
tot+=mom4;
fDaughters[k]->SetCov7(p2Cov);
  }
fHeadOfTree->SetP4(tot);
return;
 }


}



//Read the input vector
void RhoKinHyperonFitter::ReadMatrix()
{

//Here the m.c.f is treated differently with other fit, e.g. 4c

if(fMassConstraint>0||fMomConstraint>0){

//px,py,pz,E,x,y,z
    TLorentzVector p1=fHeadOfTree->P4();
    TVector3 p2=fHeadOfTree->Pos();
    fAl0[0][0]=p1.X();
    fAl0[1][0]=p1.Y();
    fAl0[2][0]=p1.Z();
    fAl0[3][0]=p1.E();
    fAl0[4][0]=p2.X();
    fAl0[5][0]=p2.Y();
    fAl0[6][0]=p2.Z();


// Read Covariance Matrix .... Can read 6x6 matrices..................
    TMatrixD p1Cov(7,7);
    TMatrixD HeadCov(7,7);
    TMatrixD p3Cov(7,7);
    TMatrixD p4Cov(7,7);
    p1Cov=fHeadOfTree->Cov7(); //Cov Matrix x,y,z,px,py,pz,E

    for (int ii=0; ii<6; ii++) {for(int jj=0; jj<6; jj++) {HeadCov[ii][jj]=p1Cov[ii][jj];}} //test

    //Extend matrix for energy for each candidates .....6x6 to 7x7

    p3Cov=p1Cov;
//cout<<"in readMatrix before trans"<<endl;
//   p3Cov.Print();
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

 //cout<<"after trans"<<endl;
 //    p4Cov.Print();
    for(int i=0; i<7; i++) {
      for (int j=0; j<7; j++) {
        fV_al0[i][j]  = p4Cov[i][j];
      }
    }

 }

 }



void RhoKinHyperonFitter::SetInputMatrix(TMatrixD M1, TMatrixD V1)
{

// Here these Matrix must be read in from the vertex fit
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
fAl0=M1;
fV_al0=V1;

}






void RhoKinHyperonFitter::ReadMassKinMatrix()
{

  fmD.Zero();
  //double Etot = 0.;
  //double Px = 0.;
  //double Py = 0.;
  //double Pz = 0.;
  TMatrixD al1p(fAl1);
  //double a;


    double px = al1p[0][0];
    double py = al1p[1][0];
    double pz = al1p[2][0];
    double E =  al1p[3][0];

  fmd[0][0] = E*E - px*px - py*py - pz*pz - fMass*fMass ;

// V.J. - force head mass to constraint mass
//....................................................
                  fmD[0][0] = -2.*px;
                  fmD[0][1] = -2.*py;;
                  fmD[0][2] = -2.*pz;
                  fmD[0][3] = 2.*E;
                  fmD[0][4] = 0.0;
                  fmD[0][5] = 0.0;
                  fmD[0][6] = 0.0;
//.....................................

}

void RhoKinHyperonFitter::Read4MomKinMatrix()
{
  int nd=fDaughters.size();
  TMatrixD alp(fAl1);
  TMatrixD m(nd,1);
  fmD.Zero();
  fmd.Zero();
  int k,i;
  for (k=0; k<nd; k++) {

   TLorentzVector p1=fDaughters[k]->P4();
   m[k][0]=p1.M();

  // double E =alp[k*7+3][0];
   double E =sqrt(alp[k*7+0][0]*alp[k*7+0][0]+
                   alp[k*7+1][0]*alp[k*7+1][0]+
                   alp[k*7+2][0]*alp[k*7+2][0]+
                   m[k][0]*m[k][0]);

    for(i=0; i<3; i++) {
      fmD[fNc+i][k*7+i]   = 1;
      fmD[fNc+3][k*7+i]   = alp[k*7+i][0]/E;
     //if(i==3){
     //fmD[fNc][k*7+i]=E/alp[k*7][0];
     //fmD[fNc+1][k*7+i]=E/alp[k*7+1][0];
     //fmD[fNc+2][k*7+i]=E/alp[k*7+2][0];
     //fmD[fNc+3][k*7+i]=1;
     //}

    }

//  cout << "value of D" <<  mD[3][k*7+i] << endl;

    for (i=0; i<3; i++) {
      fmd[fNc+i][0] += alp[k*7+i][0];
    }
    fmd[fNc+3][0] += E;
  }

  fmd[fNc+0][0] -= flmm.X();
  fmd[fNc+1][0] -= flmm.Y();
  fmd[fNc+2][0] -= flmm.Z();
  fmd[fNc+3][0] -= flmm.T();

//  fNc += 4;
}


void RhoKinHyperonFitter::ReadMomKinMatrix()
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

 // fNc += 3;
}


void RhoKinHyperonFitter::ReadTotEKinMatrix()
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
  //fNc +=1;
}

void RhoKinHyperonFitter::ReadTotMomKinMatrix()
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
 // fNc +=1;
}

/*
void RhoKinHyperonFitter::ReadEqMassKinMatrix()
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
  //fNc +=1;
}
*/

