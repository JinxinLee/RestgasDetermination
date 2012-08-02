#include <iostream>
#include "PndKinFitter.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoBase/TFactory.h"
#include "TDecompLU.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "RhoBase/TRho.h"
using namespace std;

ClassImp(PndKinFitter)

TBuffer &operator>>(TBuffer &buf, PndKinFitter *&obj)
{
   obj = (PndKinFitter *) buf.ReadObject(PndKinFitter::Class());
   return buf;
}

PndKinFitter::PndKinFitter( const TCandidate& b) : 
  VAbsFitter( b )
{
f4MomConstraint=-1;
fMomConstraint=-1;
fMassConstraint =-1;
fTotMomConstraint=-1;
fTotEConstraint=-1;
}

PndKinFitter::~PndKinFitter()
{
}


void PndKinFitter::Add4MomConstraint(TLorentzVector lv)
{
f4MomConstraint = 1;
flmm=lv;
}

void PndKinFitter::AddMomConstraint(TVector3 v)
{
fMomConstraint = 1;
fmm=v;
}

void PndKinFitter::AddTotEConstraint(double energy)
{
fTotEConstraint = 1;
fEc=energy;
}
void PndKinFitter::AddTotMomConstraint(double momentum)
{
fTotMomConstraint = 1;
fMom=momentum;
}
void PndKinFitter::AddMassConstraint(double mass)
{
fMassConstraint = 1;
fMass=mass;
}

void PndKinFitter::Fit() 
{
   fDaughters.Cleanup();
   FindAndAddGenericDaughters(fHeadOfTree);
   fHeadOfTree->RemoveAssociations();
   for (int i=0;i<fDaughters.GetLength();i++)
   fHeadOfTree->AddDaughterLinkSimple(&(fDaughters[i]));
   //int nd=fDaughters.GetLength(); //unused?

   NumCon=0;
   if(f4MomConstraint >0){
         int n4Mom = 4;
         NumCon = NumCon + n4Mom;
         }
   if(fMomConstraint >0){
         int nMom = 3;
         NumCon = NumCon + nMom;
         }
   if(fTotEConstraint >0){
         int nE = 1;
         NumCon = NumCon + nE;
         }
   if(fMassConstraint >0){
         int nMass = 1;
         NumCon = NumCon + nMass;
         }
   if(fTotMomConstraint >0){
         int nTotMom = 1;
         NumCon = NumCon + nTotMom;
         }
   SetMatrices();
   ZeroMatrices();
   ReadMatrix();
   Solve();
   if (true) {SetOutput();}
}


void PndKinFitter::FindAndAddGenericDaughters(TCandidate *head)
{
  TCandidate *tc;
  TCandListIterator iter=head->DaughterIterator();
  
  while ((tc=iter.Next()))
  {
    if (!tc->IsComposite()) fDaughters.Add(*tc);
    else FindAndAddGenericDaughters(tc);
  }
}

void PndKinFitter::SetMatrices(){
  int nd=fDaughters.GetLength();

  fNvar=7;
  fNpar = nd*fNvar;
  fNcon= NumCon;
  fNc=0;
  fNiter=0;


  al0.ResizeTo(7*nd,1);
  V_al0.ResizeTo(fNpar,fNpar);
  al1.ResizeTo(7*nd,1);
  V_al1.ResizeTo(fNpar,fNpar);
  mD.ResizeTo(fNcon,fNpar);
  mE.ResizeTo(fNcon,3);
  md.ResizeTo(fNcon,1);
  mPull.ResizeTo(7*nd,1);
}


void PndKinFitter::ZeroMatrices(){
   al0.Zero();
   V_al0.Zero();
   al1.Zero();
   V_al1.Zero();
   mPull.Zero();
   mD.Zero();
   md.Zero();
   mE.Zero();
}


void PndKinFitter::Solve()
{
  //int nd=fDaughters.GetLength(); //unused?
  double ierr; // used to check inversions
   al1=al0;
//   int j1Max=50;
//	for(Int_t j1=0;j1<j1Max;++j1)
//	{
   if(f4MomConstraint >0){ Read4MomKinMatrix();}
   if(fMomConstraint >0){ ReadMomKinMatrix();}
   if(fTotEConstraint >0){ ReadTotEKinMatrix();}
   if(fMassConstraint >0){ ReadMassKinMatrix();}
   if(fTotMomConstraint >0){ ReadTotMomKinMatrix();}

   TMatrixD mD_t=mD;
   mD_t.T();
 //  mD_t=mD_t.Transpose(mD);
 
 // mD_t.Print();
  TMatrixD Vd_inv = mD*V_al0*mD_t;

  TMatrixD Vd = Vd_inv.Invert(&ierr);
  //Vd.Print(); 
 // TMatrixD lam=Vd*md;
  TMatrixD lam = Vd* ( mD*(al1 - al0) + md);
  TMatrixD al_new=al0-V_al0*mD_t*lam;
//  al_new.Print();
  TMatrixD V_al_new=V_al0-V_al0*mD_t*Vd*mD*V_al0;
  double chi2=0.;
  for (int i=0;i<fNcon;i++) chi2+=lam[i][0]*md[i][0];
  fGlobChi2=chi2;
  double covdif=(V_al0[0][0]-V_al_new[0][0]);
  if (covdif > 0 ) {mPull[0][0] =(al0[0][0]-al_new[0][0])/sqrt(covdif);}
  al0=al_new;
  V_al0=V_al_new;
  fPull=mPull[0][0];
}

//Write output
//Write output
void PndKinFitter::SetOutput()
{ 
int nd=fDaughters.GetLength();
 TMatrixD m(nd,1);
 fdgf = NumCon;

 double  sumA=0;
 double a;

    for (int k=0;k<nd;k++)
    {
       a = -0.00299792458*2.0*fDaughters[k].GetCharge();
       sumA += a;
       m[k][0]=fDaughters[k].P4().M();
     TVector3 pos(al0[k*7+4][0],al0[k*7+5][0],al0[k*7+6][0]);
     TLorentzVector mom4(al0[k*7+0][0],al0[k*7+1][0],al0[k*7+2][0],al0[k*7+3][0]);
     TLorentzVector p1;
     p1.SetXYZM(al0[k*7+0][0],al0[k*7+1][0],al0[k*7+2][0],m[k][0]);
   // fDaughters[k].SetP4(p1);
    fDaughters[k].SetP7(pos,mom4); 

    for(int i=0;i<7;i++){
      for (int j=0;j<7;j++){
        TMatrixD p1Cov(7,7);
        p1Cov[i][j]= V_al0[k*7+i][k*7+j];
        fDaughters[k].SetCov7(p1Cov); //New covariance matrix without correlations 
      }
    }
 	  }

// For the composite particle ..............................
    double fpx=0,fpy=0,fpz=0,fe=0;	
    for (int k=0;k<nd;k++)
    {
     fpx+= al0[k*7+0][0];
     fpy+= al0[k*7+1][0];
     fpz+= al0[k*7+2][0];
     fe += al0[k*7+3][0];
  //   fe +=fDaughters[k].P4().E(); // Energy from the initial daughter particles
 	  }
          TLorentzVector sum(fpx,fpy,fpz,fe);
	  fHeadOfTree->SetP4(sum);
}



//Read the input vector 
void PndKinFitter::ReadMatrix()
{  
  int nd =fDaughters.GetLength();
  for (int k=0;k<nd;k++)
  { 
   int kN=k*7;
//px,py,pz,E,x,y,z
    TLorentzVector p1=fDaughters[k].P4();
    TVector3 p2=fDaughters[k].Pos(); 
    al0[kN+0][0]=p1.X(); 
    al0[kN+1][0]=p1.Y(); 
    al0[kN+2][0]=p1.Z(); 
//    al0[kN+3][0]=p1.E(); 
    al0[kN+4][0]=p2.X(); 
    al0[kN+5][0]=p2.Y(); 
    al0[kN+6][0]=p2.Z();

    double fm=fDaughters[k].Mass();
    al0[kN+3][0]=sqrt(al0[kN+0][0]*al0[kN+0][0]+ al0[kN+1][0]*al0[kN+1][0]+al0[kN+2][0]*al0[kN+2][0]+fm*fm); 

// Read Covariance Matrix .... Can read 6x6 matrices..................
    TMatrixD p1Cov(7,7);
    TMatrixD p2Cov(6,6);
    TMatrixD p3Cov(7,7);
    TMatrixD p4Cov(7,7);
    p1Cov=fDaughters[k].Cov7(); //Cov Matrix x,y,z,px,py,pz,E

    for (int ii=0;ii<6;ii++) {for(int jj=0;jj<6;jj++) {p2Cov[ii][jj]=p1Cov[ii][jj];}}  //test

   //Extend matrix for energy for each candidates .....6x6 to 7x7

    TMatrixD J(7,6) ;
    J.Zero();
    TMatrixD J_t(6,7);
    for (int ii=0;ii<6;ii++) {for(int jj=0;jj<6;jj++) {J[ii][jj] = 1;}}
    for(int i=3; i<6; ++i){J[6][i] = al0[kN+i-3][0]/al0[kN+3][0];}
 //  for (int i=3; i <6; i++) { J[6][i] = -al0[kN+i-3][0]/fm;}
//        J[6][6] =al0[kN+6-3][0] /fm;
 //   p3Cov= J*p2Cov*(J_t.Transpose(J));
    p3Cov=p1Cov;
 
//Change to px,py,pz,E,x,y,z
        for(int i=0;i<7;i++){
           for(int j=0;j<7;j++){
              if(i>=3){
                if(j>=3)
                 p4Cov[i-3][j-3] = p3Cov[i][j];else p4Cov[i-3][j+3] = p3Cov[i][j];
                  }else 
                 {if(j>=3)
                 p4Cov[i+4][j-3] = p3Cov[i][j];else p4Cov[i+4][j+4] = p3Cov[i][j];}}}

 // cout<<"p2Cov"<<endl;
  // p2Cov.Print();
    for(int i=0;i<7;i++){
      for (int j=0;j<7;j++){
        V_al0[k*7+i][k*7+j]  = p4Cov[i][j];
      }
    }
}
} 


void PndKinFitter::ReadMassKinMatrix()
{
  int nd=fDaughters.GetLength();
 
   double Etot = 0.;
   double Px = 0.;
   double Py = 0.;
   double Pz = 0.;

   TMatrixD al1p(al1);
   double a;
   TMatrixD m(nd,1);
   for(int k=0;k<nd;++k){
      int kN=k*7;
      TLorentzVector p1=fDaughters[k].P4();
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

      md[fNc+0][0] = Etot*Etot - Px*Px - Py*Py - Pz*Pz - fMass*fMass ;


     for(int k=0;k<nd;++k){
        int kN=k*7;
        TLorentzVector p1=fDaughters[k].P4();
        m[k][0]=p1.M();
       double px = al1p[kN+0][0];
       double py = al1p[kN+1][0];
       double pz = al1p[kN+2][0];
       double E = TMath::Sqrt(px*px+py*py+pz*pz+m[k][0]*m[k][0]); 
	  a = -0.00299792458*2.0*fDaughters[k].GetCharge();
                Double_t invE = 1./E;
//....................................................
              mD[fNc+0][kN+0] = 2.*(Etot*px*invE-Px);
              mD[fNc+0][kN+1] = 2.*(Etot*py*invE-Py);
              mD[fNc+0][kN+2] = 2.*(Etot*pz*invE-Pz);
              mD[fNc+0][kN+3] = 0.0;
              mD[fNc+0][kN+3] = 2* m[k][0]*Etot*invE;
          //  mD[0][kN+4] = 2.*(Etot*py*invE-Py)*a;
           // mD[0][kN+5] = 2.*(Etot*px*invE-Px)*a;
              mD[fNc+0][kN+4] = 2.*Py*a;
              mD[fNc+0][kN+5] = -2.*Px*a;
              mD[fNc+0][kN+6] = 0.0;
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

void PndKinFitter::Read4MomKinMatrix()
{
 int nd=fDaughters.GetLength();
 TMatrixD alp(al1);
 TMatrixD m(nd,1);
 int k,i;
 for (k=0;k<nd;k++)
  {  
  TLorentzVector p1=fDaughters[k].P4();
   m[k][0]=p1.M();
  double E =sqrt(alp[k*7+0][0]*alp[k*7+0][0]+
                 alp[k*7+1][0]*alp[k*7+1][0]+
                 alp[k*7+2][0]*alp[k*7+2][0]+
                 m[k][0]*m[k][0]);
  for (i=0;i<3;i++) 
    {
      mD[fNc+i][k*7+i]   = 1;
      mD[fNc+3][k*7+i]   = alp[k*7+i][0]/E;
    }
//   cout << "value of D" <<  mD[3][k*7+i] << endl;

for (i=0;i<3;i++) 
    {
     md[fNc+i][0] += alp[k*7+i][0];
    }
    md[fNc+3][0] += E;
  }
  md[fNc+0][0] -= flmm.X();
  md[fNc+1][0] -= flmm.Y();
  md[fNc+2][0] -= flmm.Z();
  md[fNc+3][0] -= flmm.T();
 fNc += 4;
}


void PndKinFitter::ReadMomKinMatrix()
{
 int nd=fDaughters.GetLength();
 TMatrixD alp(al1);
 TMatrixD m(nd,1);
 int k,i;
 for (k=0;k<nd;k++)
  {
  for (i=0;i<3;i++) 
    {
      mD[fNc+i][k*7+i]   = 1;
    }
for (i=0;i<3;i++) 
    {
     md[fNc+i][0] += alp[k*7+i][0];
    }
  }
  md[fNc+0][0]-= fmm.X();
  md[fNc+1][0]-= fmm.Y();
  md[fNc+2][0]-= fmm.Z();

  fNc += 3;
}


void PndKinFitter::ReadTotEKinMatrix()
{int nd=fDaughters.GetLength();
 TMatrixD alp(al1);
 TMatrixD m(nd,1);
 int k,i;
 for (k=0;k<nd;k++)
{
 TLorentzVector p1=fDaughters[k].P4();
   m[k][0]=p1.M();
  double E =sqrt(alp[k*7+0][0]*alp[k*7+0][0]+
                 alp[k*7+1][0]*alp[k*7+1][0]+
                 alp[k*7+2][0]*alp[k*7+2][0]+
                 m[k][0]*m[k][0]);
 for (i=0;i<3;i++) 
    {
     mD[fNc+0][k*7+i]   = 0.;
    // mD[0][k*7+i]  = alp[k*7+i][0]/E;
    }
    mD[fNc+0][k*7+3]   = 1.;
    md[fNc+0][0] += E;
}
 md[fNc+0][0] -= fEc;
 fNc +=1;
}

void PndKinFitter::ReadTotMomKinMatrix()
{int nd=fDaughters.GetLength();
 TMatrixD alp(al1);
 TMatrixD m(nd,1);
 int k,i;
 for (k=0;k<nd;k++)
{
  double Ptot =sqrt(alp[k*7+0][0]*alp[k*7+0][0]+
                   alp[k*7+1][0]*alp[k*7+1][0]+
                   alp[k*7+2][0]*alp[k*7+2][0]);
 for (i=0;i<3;i++) 
    {
    // mD[0][k*7+i]   = 1.;
     mD[fNc+0][k*7+i]  = alp[k*7+i][0]/Ptot;
    }
    md[fNc+0][0] += Ptot;
}
 md[fNc+0][0] -= fMom;
 fNc +=1;
}

/*
void PndKinFitter::ReadEqMassKinMatrix()
{int nd=fDaughters.GetLength();
 TMatrixD alp(al1);
 TMatrixD m(nd,1);
 int k,i,j;
 for (k=0;k<nd;k++)
{
   TLorentzVector p1=fDaughters[k].P4();
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












