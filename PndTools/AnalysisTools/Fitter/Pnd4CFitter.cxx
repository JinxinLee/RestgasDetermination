//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Pnd4CFitter                                                          //
//                                                                      //
// Author: K. Goetzen, GSI, 2008                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "Pnd4CFitter.h"
#include "RhoBase/RhoCandListIterator.h"

#include "RhoBase/RhoFactory.h"

using namespace std;

ClassImp(Pnd4CFitter)

TBuffer& operator>>(TBuffer& buf, Pnd4CFitter *&obj)
{
  obj = (Pnd4CFitter*) buf.ReadObject(Pnd4CFitter::Class());
  return buf;
}

Pnd4CFitter::Pnd4CFitter( RhoCandidate* b , TLorentzVector lv) :
  RhoFitterBase( b ),
  fLv4C(lv),
  fNDau(0),
  fConserveDaughterMasses(false)
{
  fChiSquare=0.;
}


Pnd4CFitter::~Pnd4CFitter()
{
}

void Pnd4CFitter::PrintTree(RhoCandidate* c, int l)
{
  for (int i=0; i<l; i++) { std::cout <<"          "; }
  if (c) {
    printf("[%d, %1.5f]",c->Uid(),c->M());
    if (c->NDaughters()>0) {
      cout<<" ->"<<endl;
      for (int j=0; j<c->NDaughters(); j++) {
        PrintTree(c->Daughter(j),l+1);
      }
    } else { cout<<endl; }
  }
}

Bool_t Pnd4CFitter::Fit()
{
  fDaughters.clear();
  FindAndAddFinalStateDaughters(fHeadOfTree); //add all leaves as deep as they are unlocked in fit status!
  fNDau=fDaughters.size();
  Bool_t check = Do4CFit();
  SetFourMomentumByDaughters(fHeadOfTree);
  return check;
}


Bool_t Pnd4CFitter::FitConserveMasses()
{
  fDaughters.clear();
  FindAndAddFinalStateDaughters(fHeadOfTree);
  fNDau=fDaughters.size();
  Bool_t check = Do4CFitWithMassConservation();
  SetFourMomentumByDaughters(fHeadOfTree);
  return check;
}




Bool_t Pnd4CFitter::Do4CFit()
{
  int nd=fNDau;

  TMatrixD al(4*nd,1);
  TMatrixD V_al0(4*nd,4*nd);
  TMatrixD V_D(4,4);
  TMatrixD d(4,1);
  TMatrixD D(4,4*nd);

  int k,i,j;

  for (k=0; k<nd; k++) {
    TLorentzVector p1=fDaughters[k]->P4();
    al[k*4+0][0]=p1.X();
    al[k*4+1][0]=p1.Y();
    al[k*4+2][0]=p1.Z();
    al[k*4+3][0]=p1.T();

    TMatrixD p1Cov=fDaughters[k]->Cov7();
    for(i=0; i<4; i++) {
      for (j=0; j<4; j++) {
        V_al0[k*4+i][k*4+j]  = p1Cov[i+3][j+3];
        V_D[i][j]           += p1Cov[i+3][j+3];
      }
    }

    for (i=0; i<4; i++) {
      D[i][i+k*4]   = 1;
      d[i][0] += al[k*4+i][0];
    }
  }

  TMatrixD D_t(4*nd,4);
  D_t=D_t.Transpose(D);

  //cout <<"D_t:"<<endl;
  //D_t.Print();

  d[0][0] -= fLv4C.X();
  d[1][0] -= fLv4C.Y();
  d[2][0] -= fLv4C.Z();
  d[3][0] -= fLv4C.T();

  //cout <<"d:"<<endl;
  //d.Print();


  //TMatrixD V_D = D*V_al0*D_t;
  //cout <<"V_D:"<<endl;
  //V_D.Print();

  V_D=V_D.Invert(0);

  //cout <<"V_D inv:"<<endl;
  //V_D.Print();


  TMatrixD lam(4,1);

  lam=V_D*d;
  /*
    for (i=0;i<4;i++)
    {
      lam2[i][0]=0;
      double sum=0;
      for (j=0;j<4;j++){

         // cout <<"v:"<<V_D[j][i]<<"  d:"<<d[i][0]<<endl;
          sum+=V_D[i][j]*d[j][0];
      }
      lam2[i][0]=sum;
    }
    */
  TMatrixD alnew=al-V_al0*D_t*lam;

  // Writing results to final state

  for (k=0; k<nd; k++) {
    TLorentzVector p1;//=fDaughters[k]->P4();
    p1.SetX(alnew[k*4+0][0]);
    p1.SetY(alnew[k*4+1][0]);
    p1.SetZ(alnew[k*4+2][0]);
    p1.SetT(alnew[k*4+3][0]);
    fDaughters[k]->SetP4(p1);
  }
  //TMatrixD chi2(1,1);

  double chi2=0;//lam2[0][0]*d[0][0]+lam2[1][0]*d[1][0]+lam2[2][0]*d[2][0]+lam2[3][0]*d[3][0];

  for (i=0; i<4; i++) { chi2+=lam[i][0]*d[i][0]; }

  fChiSquare=chi2;
  //fNDegreesOfFreedom=4*nd+4-4*nd; //(measurement+constraints-adjusted)
  fNDegreesOfFreedom=4;
  return kTRUE;
}



//Fit conserves the daughter masses (only 3 params per track...)

Bool_t Pnd4CFitter::Do4CFitWithMassConservation()
{
  int nd=fDaughters.size();

  TMatrixD al(3*nd,1);
  TMatrixD V_al0(3*nd,3*nd);
  TMatrixD V_D(4,4);
  TMatrixD d(4,1);
  TMatrixD D(4,3*nd);

  TMatrixD m(nd,1);


  int k,i,j;

  for (k=0; k<nd; k++) {
    TLorentzVector p1=fDaughters[k]->P4();
    al[k*3+0][0]=p1.X();
    al[k*3+1][0]=p1.Y();
    al[k*3+2][0]=p1.Z();
    //al[k*4+3][0]=p1.T();
    m[k][0]=p1.M();

    TMatrixD p1Cov=fDaughters[k]->Cov7();
    for(i=0; i<3; i++) {
      for (j=0; j<3; j++) {
        V_al0[k*3+i][k*3+j]  = p1Cov[i+3][j+3];
        //V_D[i][j]           += p1Cov[i+3][j+3];
      }
    }

    for (i=0; i<3; i++) {
      D[i][k*3+i]   = 1;
      D[3][k*3+i]   = al[k*3+i][0]/p1.T();
      d[i][0] += al[k*3+i][0];
    }
    d[3][0] += p1.T();
  }

  TMatrixD D_t(3*nd,4);
  D_t=D_t.Transpose(D);

  //cout <<"D_t:"<<endl;
  //D_t.Print();

  d[0][0] -= fLv4C.X();
  d[1][0] -= fLv4C.Y();
  d[2][0] -= fLv4C.Z();
  d[3][0] -= fLv4C.T();

  //cout <<"d:"<<endl;
  //d.Print();


  V_D = D*V_al0*D_t;
  //cout <<"V_D:"<<endl;
  //V_D.Print();

  V_D=V_D.Invert(0);

  //cout <<"V_D inv:"<<endl;
  //V_D.Print();


  TMatrixD lam(4,1);

  lam=V_D*d;
  /*
    for (i=0;i<4;i++)
    {
      lam2[i][0]=0;
      double sum=0;
      for (j=0;j<4;j++){

         // cout <<"v:"<<V_D[j][i]<<"  d:"<<d[i][0]<<endl;
          sum+=V_D[i][j]*d[j][0];
      }
      lam2[i][0]=sum;
    }
    */
  TMatrixD alnew=al-V_al0*D_t*lam;

  // Write to final state/daughters
  for (k=0; k<nd; k++) {
    TLorentzVector p1;//=fDaughters[k]->P4();
    p1.SetXYZM(alnew[k*3+0][0],alnew[k*3+1][0],alnew[k*3+2][0],m[k][0]);
    fDaughters[k]->SetP4(p1);
  }
  //TMatrixD chi2(1,1);

  double chi2=0;//lam2[0][0]*d[0][0]+lam2[1][0]*d[1][0]+lam2[2][0]*d[2][0]+lam2[3][0]*d[3][0];

  for (i=0; i<4; i++) { chi2+=lam[i][0]*d[i][0]; }

  fChiSquare=chi2;
  //fNDegreesOfFreedom=nd+4;
  fNDegreesOfFreedom=4;
  return kTRUE;
}


