
#include <iostream>
#include "PndKinVtxFitter.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoBase/TRho.h"
#include "RhoBase/TFactory.h"
#include "TDecompLU.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "PndVtxPoca.h"

using namespace std;


ClassImp(PndKinVtxFitter)

TBuffer &operator>>(TBuffer &buf, PndKinVtxFitter *&obj)
{
  obj = (PndKinVtxFitter *) buf.ReadObject(PndKinVtxFitter::Class());
  return buf;
}

//Include only those constraint which need vertex Info....
PndKinVtxFitter::PndKinVtxFitter( const TCandidate& b) : 
VAbsFitter( b )
{
  fMassConstraint =-1;
  fPointConstraint=-1;
}

PndKinVtxFitter::~PndKinVtxFitter()
{
}

void PndKinVtxFitter::AddMassConstraint(double mass)
{
  fMassConstraint = 1;
  fMass=mass;
}

void PndKinVtxFitter::AddPointingConstraint(TVector3 pVtx)
{
  fPointConstraint = 1;
  fpVtx=pVtx;
}

void PndKinVtxFitter::Fit() 
{
  FitLeaf(fHeadOfTree);
}

void PndKinVtxFitter::FitLeaf(TCandidate *head)
{
  TCandidate *tc;
  //if (head->decayVtx() == 0 ) {
  if ( head->NDaughters()>0 ) {
    TCandListIterator iter=head->DaughterIterator();
    
    while ((tc=iter.Next())) {
      if (tc->IsComposite()) FitLeaf(tc);
      //    else if( (!tc->decayVtx() && !tc->isAResonance() ) 
    }
  }
  //}
  fDaughters.Cleanup();
  //FindAndAddGenericDaughters(fHeadOfTree);
  FindAndAddGenericDaughters(head); //[ralfk:01.12.11 Try to make it a leaf-by-leaf fit]
  
  
  Compute();
  SetOutput();
  
}

void PndKinVtxFitter::FindAndAddGenericDaughters(TCandidate *head)
{
  TCandidate *tc;
  TCandListIterator iter=head->DaughterIterator();
  
  while ((tc=iter.Next()))
  {
    fDaughters.Add(*tc);
    // //[ralfk:01.12.11 Try to make it a leaf-by-leaf fit]
    // if (!tc->IsComposite()) fDaughters.Add(*tc);
    // else FindAndAddGenericDaughters(tc);
  }
}

void PndKinVtxFitter::SetMatrices(){
  int nd=fDaughters.GetLength();
  
  fNvar=7;
  fNpart=nd;
  fNpar =nd*fNvar;
  fNcon=NumCon;
  //  if(fVerbose) cout << fNcon << "Num " << endl;
  fNc=0;
  fNiter=0;
  
  
  al0.ResizeTo(7*nd,1);
  V_al0.ResizeTo(fNpar,fNpar);
  al1.ResizeTo(7*nd,1);
  V_al1.ResizeTo(fNpar,fNpar);
  vtx_ex.ResizeTo(3,1);
  vtx_st.ResizeTo(3,1);
  mPull.ResizeTo(7*nd,1);
  covC.ResizeTo(7,7);
}


void PndKinVtxFitter::ResetMatrices(){
  al0.Zero();
  V_al0.Zero();
  al1.Zero();
  V_al1.Zero();
  mPull.Zero();
  vtx_ex.Zero();
  vtx_st.Zero();
  covC.Zero();
}



void PndKinVtxFitter::Compute()
{
  // int nd=fDaughters.GetLength();
  int nd=fDaughters.GetLength();
  NumCon=0;
  fdgf =0;
  
  if(fMassConstraint >0){
    int nMass = 1;
    NumCon = NumCon + nMass;
  }
  NumCon = NumCon +2*nd;
  
  
  SetMatrices();
  ResetMatrices();
  ReadMatrix();
  
  TMatrixD cov_al_x(7*nd,3);
  
  TVector3 startVtx;
  
  GetStartVtx(startVtx);
  vtx_st[0][0]=startVtx.X();vtx_st[1][0]=startVtx.Y();vtx_st[2][0]=startVtx.Z();
  // vtx_st[0][0]=0.0;vtx_st[1][0]=0.0;vtx_st[2][0]=0.0;
  vtx_ex=vtx_st;
  if(fVerbose) cout<<"Initial vertex Position is"<<vtx_ex[0][0]<<" "<<vtx_ex[1][0]<<" "<<vtx_ex[2][0]<<endl;
  
  // al1=al0;
  // V_al1=V_al0;
  TransportToVertex(al0,V_al0,al1,V_al1,vtx_ex);
  
  al0=al1;
  V_al0=V_al1;
  TMatrixD V_vtx(3,3);
  V_vtx[0][0] = 1000.; 
  V_vtx[1][1] = 1000.;
  V_vtx[2][2] = 1000.;
  // vtx_ex=vtx_st;
  
  double ierr =0 ; // used to check inversions
  TMatrixD chi2(1,1);
  TMatrixD chi2_1(1,1);
  
  chi2[0][0]=2000000.;
  //double tmp_chiSq = 999;
  
  int j1Max=20;
  
	for(Int_t j1=0;j1<j1Max;++j1)
	{
    fNc=0; 
    if(fMassConstraint >0){ ReadMassKinMatrix();}
    //FIXME: here should be an else statement, right?
    ReadKinMatrix();
    
    TMatrixD mD_t=mD;
    mD_t.T();
    // mD_t=mD_t.Transpose(mD);
    // mD_t.Print();
    
    TMatrixD Vd_inv = mD*V_al0*mD_t;
    if(Vd_inv==0)continue;
    TMatrixD Vd = Vd_inv.Invert(&ierr);
    
    
    //   if( ierr != 0 ){
    //  if(fVerbose) cout << "Inversion of constraint-matrix failed! " << endl;
    //  return 0;}
    //  Vd.Print();
    
    TMatrixD del_al = al0 - al1;
    
    // Lagrange multiplier
    //TMatrixD lam0=Vd*md;
    TMatrixD lam0 = Vd* ( mD*del_al + md);
    //    if(fVerbose) cout << " lam0 calculated" << endl;
    
    //  Position Derivative matrix ...............
    TMatrixD mE_t=mE;
    mE_t.T();
    TMatrixD Vx_inv = mE_t*Vd*mE; 
    TMatrixD Vx=Vx_inv.Invert(&ierr);
    //  Vx.Print();
    
    // New vertex and covariance ........
    TMatrixD V_vtx_new(3,3); 	  
    TMatrixD vtx_new(vtx_ex);
    vtx_new -= Vx*mE_t*lam0;
    //     if(fVerbose) cout << " New vtx calculated" << endl;
    // vtx_new.Print();
    V_vtx_new = Vx;
    
    // Final Lagarange multiplier ........
    TMatrixD lam = lam0 + (Vd * mE) * (vtx_new - vtx_ex);
    //  if(fVerbose) cout << " New lam calculated" << endl;
    
    // New track parameters.............
    TMatrixD al_new(al0);
    al_new -= V_al0*mD_t*lam;
    //   if(fVerbose) cout << " New track param calculated" << endl;
    
    
    
    //chiSquared
    TMatrixD lam_t=lam;
    lam_t.T();
    // TMatrixD chi2_new = lam_t* md;
    //TMatrixD chi2_new = lam_t*(mD*(al0 - al_new) );
    TMatrixD chi2_new = lam_t*(mD*(al0 - al_new)  + md);
//    TMatrixD chi2_new = lam_t*(mD*(al0 - al_new) + mE*(vtx_st-vtx_ex) + md);
    
    
    // New Covariance Matrix................
    //   TMatrixD V_al_new(V_al0);
    //  V_al_new-=V_al0*mD_t*Vd*mD*V_al0_t;
    
    double deltaChi=chi2_new[0][0]-chi2[0][0];
    //  Check chi^2. If better yes update the values .............................. 
    if (deltaChi>0.1*chi2[0][0]) {continue;} 
    if( chi2_new[0][0] < chi2[0][0] ) {
      //if(true){
      vtx_ex = vtx_new;
      al1 = al_new;
      //     V_al0 = V_al_new;
    }
    
    
    //     if (j1==0)  {chi2=chi2_new;continue;}
    
    // If Chi^2 change is small then go out of iteration......................
    if( fabs(chi2[0][0] - chi2_new[0][0]) < 0.01 )   j1 = j1Max - 1;
    chi2 = chi2_new;
    
    if( (j1+1) == j1Max ){
      vtx_ex = vtx_new;
      al0 =al_new;
      //
      TMatrixD Vd_new(Vd);
      Vd_new -= Vd*(mE*Vx*mE_t)*Vd.T();
      TMatrixD V_al_new(V_al0);
      V_al_new -= V_al0*(mD_t*Vd_new*mD)*V_al0.T();
      cov_al_x -= V_al0*mD_t*Vd*mE*Vx; // Vertex-track Correlation
      
      //double covdif=(V_al0[6][6]-V_al_new[6][6]);
      //  if (covdif > 0 ) {mPull[0][0] =(al0[6][0]-al_new[6][0])/sqrt(covdif);}
      mPull[0][0] =(al0[6][0]-al_new[6][0]);
      fPull=mPull[0][0];
      
      V_al0 = V_al_new;    
      V_vtx = V_vtx_new;
    }
    if(fVerbose) cout << "iteration Number " << " " << j1 << endl;
    if(fVerbose) cout << " chi2 in iterartion" << " " << chi2[0][0] << endl;
  } // end of iteration-loop
  
  TMatrixD al_new_vtx(7*nd,1);
  TMatrixD Va_new_vtx(7*nd,7*nd);
  
  
  
  
  // Trivial way for covariance matrix of composite
// covC=
//  for(Int_t k=0;k<nd;k++) {
//        for(int j = 0; j< nd ; ++j)  {
//   {if(k<=j) CovS(k,j) = V_al_0(k,j);}}
//   CovC -= CovS;
//  covC+=V_al0.GetSub(k*7,(k+1)*7-1,k*7,(k+1)*7-1);}

  GetCovariance(V_al0,cov_al_x,V_vtx,covC);
  
  
  //   al_new_vtx=al1;
  //   Va_new_vtx=V_al1;
//  TransportToVertex(al0, V_al0, al_new_vtx, Va_new_vtx, vtx_ex);
 // al0=al_new_vtx;
//  V_al0=Va_new_vtx;
  fGlobChi2=chi2[0][0];
  fdgf=2*nd-3; //
  // Add no. of additional constraints
  // fChi2Diff=chi2_1[0][0]-chi2[0][0];
}






//Write output
void PndKinVtxFitter::SetOutput(TCandidate *head)
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
    TVector3 pos(al0[k*7+4][0],al0[k*7+5][0],al0[k*7+6][0]);
    TLorentzVector mom4(al0[k*7+0][0],al0[k*7+1][0],al0[k*7+2][0],al0[k*7+3][0]);
//better to put daugthers with mass hypothesis .......?? VJ
    TLorentzVector momM;
    double fM=fDaughters[k].Mass();
    momM.SetXYZM(al0[k*7+0][0],al0[k*7+1][0],al0[k*7+2][0],fM);
//    momM.SetP4(fM,al0[k*7+0][0],al0[k*7+1][0],al0[k*7+2][0]);
    fDaughters[k].SetP7(pos,mom4); 
//    fDaughters[k].SetP7(pos,momM);

     //Extend matrix for energy for each candidates if daughters from mass hypothesis 6x6 covariance
    TMatrixD p1Cov(7,7);
    TLorentzVector p1=fDaughters[k].P4();
    TMatrixD p2Cov(7,7);

    for(int i=0;i<7;i++){
      for (int j=0;j<7;j++){
        p1Cov[i][j]= V_al0[k*7+i][k*7+j];
      }
    }
    for (int ii=0;ii<6;ii++) {for(int jj=0;jj<6;jj++) {p2Cov[ii][jj]=p1Cov[ii][jj];}}  //test

     double invE = 1./al0[k*7+3][0];
     p2Cov[0+3][3+3] = p2Cov[3+3][0+3] = (p1.X()*p1Cov[0+3][0+3]+p1.Y()*p1Cov[0+3][1+3]+p1.Z()*p1Cov[0+3][2+3])*invE;
     p2Cov[1+3][3+3] = p2Cov[3+3][1+3] = (p1.X()*p1Cov[0+3][1+3]+p1.Y()*p1Cov[1+3][1+3]+p1.Z()*p1Cov[1+3][2+3])*invE;
     p2Cov[2+3][3+3] = p2Cov[3+3][2+3] = (p1.X()*p1Cov[0+3][2+3]+p1.Y()*p1Cov[1+3][2+3]+p1.Z()*p1Cov[2+3][2+3])*invE;
     p2Cov[3+3][3+3] = (p1.X()*p1.X()*p1Cov[0+3][0+3]+p1.Y()*p1.Y()*p1Cov[1+3][1+3]+p1.Z()*p1.Z()*p1Cov[2+3][2+3]
     +2.0*p1.X()*p1.Y()*p1Cov[0+3][1+3]
     +2.0*p1.X()*p1.Z()*p1Cov[0+3][2+3]
     +2.0*p1.Y()*p1.Z()*p1Cov[1+3][2+3])*invE*invE;

     p2Cov[3+3][4-4] = p2Cov[4-4][3+3] = (p1.X()*p1Cov[0+3][4-4]+p1.Y()*p1Cov[1+3][4-4]+p1.Z()*p1Cov[2+3][4-4])*invE;
     p2Cov[3+3][5-4] = p2Cov[5-4][3+3] = (p1.X()*p1Cov[0+3][5-4]+p1.Y()*p1Cov[1+3][5-4]+p1.Z()*p1Cov[2+3][5-4])*invE;
     p2Cov[3+3][6-4] = p2Cov[6-4][3+3] = (p1.X()*p1Cov[0+3][6-4]+p1.Y()*p1Cov[1+3][6-4]+p1.Z()*p1Cov[2+3][6-4])*invE;
//        fDaughters[k].SetCov7(p2Cov); //New covariance matrix without correlations
        fDaughters[k].SetCov7(p1Cov); //New covariance matrix without correlations

  }
  
  // For the composite particle ..............................
// Include neutrals particles not involved in vertex fit e.g. gamma and pi0
  double fpx=0,fpy=0,fpz=0,fe=0;	
  for (int k=0;k<nd;k++)
  {
    fpx+= al0[k*7+0][0]-a*(vtx_ex[1][0]*sumA/a - al0[k*7+5][0]);
    fpy+= al0[k*7+1][0]+a*(vtx_ex[0][0]*sumA/a - al0[k*7+4][0]);
    fpz+= al0[k*7+2][0];
    fe += al0[k*7+3][0];
  }
  //         double TotE=(fpx*fpx+fpy*fpy+fpz*fpz+fe*fe);
  //double fM=sqrt(fe*fe-(fpx*fpx+fpy*fpy*fpz*fpz));
  TLorentzVector sum(fpx,fpy,fpz,fe);
  //          TLorentzVector sum;
  //          sum.SetXYZM(fpx,fpy,fpz,fM);
  TVector3 vtx(vtx_ex[0][0],vtx_ex[1][0],vtx_ex[2][0]);
  //fHeadOfTree->SetP7(vtx,sum);
  head->SetP7(vtx,sum); //[ralfk:01.12.11 Try to make it a leaf-by-leaf fit]
  if(fVerbose) cout<<"Final vertex Position is"<<vtx_ex[0][0]<<" "<<vtx_ex[1][0]<<" "<<vtx_ex[2][0]<<endl;
  if(fVerbose) cout<<"Final Momenta are "<<al0[0][0]<<" "<<al1[1][0]<<" "<<al1[2][0]<<endl;
  //fHeadOfTree->SetCov7(covC); //New covariance matrix
  head->SetCov7(covC); //New covariance matrix //[ralfk:01.12.11 Try to make it a leaf-by-leaf fit]
}



//Read the input vector 
void PndKinVtxFitter::ReadMatrix()
{  
  int nd =fDaughters.GetLength();
  TMatrixD m(nd,1);
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
    TMatrixD p3Cov(6,6); //Why 6x6 here if 7x7 should be cpoied (below)
    TMatrixD p2Cov(7,7);
    TMatrixD p4Cov(7,7);
    p1Cov=fDaughters[k].Cov7(); //Cov Matrix x,y,z,px,py,pz,E
    
    for (int ii=0;ii<6;ii++) {for(int jj=0;jj<6;jj++) {p3Cov[ii][jj]=p1Cov[ii][jj];}}  //test
    
    //Extend matrix for energy for each candidates .....6x6 to 7x7
    
    TMatrixD J(7,6) ;
    J.Zero();
    TMatrixD J_t(6,7);
    for (int ii=0;ii<6;ii++) {for(int jj=0;jj<6;jj++) {J[ii][jj] = 1;}}
    for(int i=3; i<6; ++i){J[6][i] = al0[kN+i-3][0]/al0[kN+3][0];}
    //   p2Cov= J*p3Cov*(J_t.Transpose(J));
    p2Cov=p1Cov;
    //Change to px,py,pz,E,x,y,z
    for(int i=0;i<7;i++){
      for(int j=0;j<7;j++){
        if(i>=3){
          if(j>=3)
            p4Cov[i-3][j-3] = p2Cov[i][j];else p4Cov[i-3][j+3] = p2Cov[i][j];
        }else 
        {if(j>=3)
          p4Cov[i+4][j-3] = p2Cov[i][j];else p4Cov[i+4][j+4] = p2Cov[i][j];}}} 
    
    for(int i=0;i<7;i++){
      for (int j=0;j<7;j++){
        V_al0[k*7+i][k*7+j]  = p4Cov[i][j];
      }
    }
  }
} 


//Read Constraint Matrices ... D, E and d
//unsigned PndKinVtxFitter:: ReadKinMatrix( TMatrixD & mD,  TMatrixD & mE, TMatrixD & md)
void PndKinVtxFitter::ReadKinMatrix()
{  
  int  nd=fDaughters.GetLength();
  fNc=0;
  mD.ResizeTo(fNcon,fNpar);
  mE.ResizeTo(fNcon,3);
  md.ResizeTo(fNcon,1);
  for (int k=0;k<nd;k++)
  { 
    int kN=k*7;
    int k2=k*2;
    double delX = vtx_ex[0][0] - al1[kN+4][0];
    double delY = vtx_ex[1][0] - al1[kN+5][0];
    double delZ = vtx_ex[2][0] - al1[kN+6][0];
    double px = al1[kN+0][0];
    double py = al1[kN+1][0];
    double pz = al1[kN+2][0];
    double ch=fDaughters[k].GetCharge();
    //double bField = TRho::Instance()->GetMagnetField();  //unused, why?
    double a = -0.0029979246*ch*2.0; //TODO: is bfield put here manually?
    double pT_2 = px*px + py*py;
    
    double J = a*(delX*px + delY*py)/pT_2;       
    double Rx = delX - 2.*px*(delX*px + delY*py)/pT_2;
    double Ry = delY - 2.*py*(delX*px + delY*py)/pT_2;
    
    // if(fabs(J) > 1) {return 0;}
    if (J>=1.0 || J<= -1.0) { J = (J>=1.0 ? 0.99 : -0.99);}
    
    double S = 1./(pT_2*sqrt(1-(J*J)));
    // if(fVerbose) cout << ch << "ch" << S << "pt2" << J << "bField" << bField <<endl;
    double asin_J = asin(J);
    //charged particle 
    if(ch !=0)
    {
      mD[fNc+0+k2][kN+0]  = delY ;
      //if(fVerbose) cout << al0[kN+4][0] << " " << delY << endl;
      mD[fNc+0+k2][kN+1]  = -(delX) ;
      mD[fNc+0+k2][kN+2]  = 0. ;
      mD[fNc+0+k2][kN+3]  = 0. ;
      mD[fNc+0+k2][kN+4]  =  py + a*delX ;
      mD[fNc+0+k2][kN+5]  = -px + a*delY ;
      mD[fNc+0+k2][kN+6]  = 0. ;
      
      mD[fNc+1+k2][kN+0] = -pz*S*Rx ;
      mD[fNc+1+k2][kN+1] = -pz*S*Ry ;
      mD[fNc+1+k2][kN+2] = -asin_J/a ;
      mD[fNc+1+k2][kN+3] = 0.;
      mD[fNc+1+k2][kN+4] = px*pz*S;
      mD[fNc+1+k2][kN+5] = py*pz*S;
      mD[fNc+1+k2][kN+6] = -1.;
    }else{
      //neutral particle  
      mD[fNc+0+k2][kN+0]  = delY ;
      mD[fNc+0+k2][kN+1]  = -(delX) ;
      mD[fNc+0+k2][kN+2]  = 0. ;
      mD[fNc+0+k2][kN+3]  = 0. ;
      mD[fNc+0+k2][kN+4]  =  py;
      mD[fNc+0+k2][kN+5]  = -px;
      mD[fNc+0+k2][kN+6]  = 0. ;
      
      mD[fNc+1+k2][kN+0] = 2*(delX*px+delY*py)*px*pz/(pT_2*pT_2) - pz*delX/(pT_2);
      mD[fNc+1+k2][kN+1] = 2*(delX*px+delY*py)*px*pz/(pT_2*pT_2) - pz*delY/(pT_2);
      mD[fNc+1+k2][kN+2] =-(delX*px+delY*py)/(pT_2);
      mD[fNc+1+k2][kN+3] = 0.;
      mD[fNc+1+k2][kN+4] = px*pz/pT_2;
      mD[fNc+1+k2][kN+5] = py*pz/pT_2;
      mD[fNc+1+k2][kN+6] = -1.;
    } 
    //DMat_trk.push_back(DMat_tmp);
    //E jacobian matrix
    if(ch !=0 )   {
      mE[fNc+0+k2][0] = -(py + a*delX);
      mE[fNc+0+k2][1] =  (px - a*delY);
      mE[fNc+0+k2][2] = 0.;
      mE[fNc+1+k2][0] = -px*pz*S;
      mE[fNc+1+k2][1] = -py*pz*S;
      mE[fNc+1+k2][2] = 1.;
    }else{
      mE[fNc+0+k2][0] = -py ;
      mE[fNc+0+k2][1] =  px;
      mE[fNc+0+k2][2] = 0.;
      mE[fNc+1+k2][0] = -px*pz/pT_2;
      mE[fNc+1+k2][1] = -py*pz/pT_2;
      mE[fNc+1+k2][2] = 1.;
    }
    if(ch !=0 )   {
      md[fNc+0+k2][0] = delY*px - delX*py - (a/2.)*(delX*delX + delY*delY);
      md[fNc+1+k2][0] = delZ - (pz/a)*asin_J;
    }else{
      md[fNc+0+k2][0] = delY*px - delX*py;
      md[fNc+1+k2][0] = delZ - pz*(delX * px + delY * py)/(pT_2);
    }      
  }
  fNc +=2*nd;
}


void PndKinVtxFitter::ReadMassKinMatrix()
// unsigned PndKinVtxFitter:: ReadMassKinMatrix( TMatrixD & mD,  TMatrixD & mE,  TMatrixD & md)
{
  // if(m_fitIncludingVertex == 0)
  //{
  int nd=fDaughters.GetLength();
  
  mD.ResizeTo(fNcon,fNpar);
  mE.ResizeTo(fNcon,3);
  md.ResizeTo(fNcon,1);
  
  double Etot = 0.;
  double Px = 0.;
  double Py = 0.;
  double Pz = 0.;
  
  TMatrixD al1p(al1);
  double a=0; //TODO: is that right to initialize with 0?
  TMatrixD m(nd,1);
  
  /*
   // Mass Constraint without vertex Info.............................
   
   for(unsigned k=0;k<nd;++k){
   int kN=k*7;
   TLorentzVector p1=fDaughters[k].P4();
   m[k][0]=p1.M();
   double px = al1p[kN+0][0];
   double py = al1p[kN+1][0];
   double pz = al1p[kN+2][0];
   double E = TMath::Sqrt(px*px+py*py+pz*pz+m[k][0]*m[k][0]); 
   
   // Etot += E; Px   +=px; Py +=py;  Pz += pz;}
   // md[fNc+0][0] = Etot*Etot - Px*Px - Py*Py - Pz*Pz - fMass*fMass ;
   
   a = -0.00299792458*2.0*fDaughters[k].GetCharge();
   Double_t invE = 1./E;
   
   mD[fNc+0][kN+0] = 2.*(Etot*px*invE-Px);
   mD[fNc+0][kN+1] = 2.*(Etot*py*invE-Py);
   mD[fNc+0][kN+2] = 2.*(Etot*pz*invE-Pz);
   //    mD[fNc+0][kN+3] = 0.0;
   mD[fNc+0][kN+3] = 2* m[k][0]*Etot*invE;
   mD[fNc+0][kN+4] = 2.*Py*a;
   mD[fNc+0][kN+5] = -2.*Px*a;
   mD[fNc+0][kN+6] = 0.0;
   
   //................Simple....................
   
   //              mD[fNc+0][kN+0] = -2.*Px;
   //              mD[fNc+0][kN+1] = -2.*Py;;
   //              mD[fNc+0][kN+2] = -2.*Pz;
   //              mD[fNc+0][kN+3] = 2.*Etot;
   //      mD[fNc+0][kN+3] = 2* m[k][0]*Etot*invE;
   //      mD[fNc+0][kN+4] = 2.*(Etot*py*invE-Py)*a;
   //      mD[fNc+0][kN+5] = 2.*(Etot*px*invE-Px)*a;
   //              mD[fNc+0][kN+4] = 0.0;
   //              mD[fNc+0][kN+5] = 0.0;
   //              mD[fNc+0][kN+6] = 0.0;
   }
   */
  
  // With Vertex Info.............................................
  for(int k=0;k<nd;++k){
    int kN=k*7;
    TLorentzVector p1=fDaughters[k].P4();
    m[k][0]=p1.M();
    double delX = vtx_ex[0][0] - al1p[kN+4][0];
    double delY = vtx_ex[1][0] - al1p[kN+5][0];
    //    double delX=0.;
    //    double delY=0.;
    
    al1p[kN+0][0] = al1p[kN+0][0]-a*delY;
    al1p[kN+1][0] = al1p[kN+1][0]+a*delX;
  //  al1p[kN+2][0] = al1p[kN+2][0];
    double E = TMath::Sqrt(al1p[kN+0][0]* al1p[kN+0][0]+al1p[kN+1][0]*al1p[kN+1][0]+al1p[kN+2][0]*al1p[kN+2][0]+m[k][0]*m[k][0]);
    Etot += E;
    
    Px += al1p[kN+0][0] ;
    Py += al1p[kN+1][0];
    Pz += al1p[kN+2][0];
  }
  md[fNc+0][0] = Etot*Etot - Px*Px - Py*Py - Pz*Pz - fMass*fMass ;
  
  double sumA=0;
  for(int k=0;k<nd;++k){
    int kN=k*7;
    double px = al1p[kN+0][0];
    double py = al1p[kN+1][0];
    double pz = al1p[kN+2][0];
    //    double E = al1p[kN+3][0];
    double E = TMath::Sqrt(px*px+py*py+pz*pz+m[k][0]*m[k][0]); 
    a = -0.00299792458*2.0*fDaughters[k].GetCharge();
    sumA += a;
    Double_t invE = 1./E;
    
    mD[fNc+0][kN+0] = 2.*(Etot*al1p[kN+0][0]*invE-Px);
    mD[fNc+0][kN+1] = 2.*(Etot*al1p[kN+1][0]*invE-Py);
    mD[fNc+0][kN+2] = 2.*(Etot*al1p[kN+2][0]*invE-Pz);
    mD[fNc+0][kN+3] = 0.;
    mD[fNc+0][kN+4] =-2.*(Etot*al1p[kN+1][0]*invE-Py)*a;
    mD[fNc+0][kN+5] = 2.*(Etot*al1p[kN+0][0]*invE-Px)*a;
    mD[fNc+0][kN+6] = 0.;
  }
  
  mE[fNc+0][0] = 2*sumA*Px;
  mE[fNc+0][1] = -2*sumA*Py;
  mE[fNc+0][2] = 0.;
  fNc +=1;
} 

// not used yet?
void PndKinVtxFitter::ReadPointingKinMatrix(TCandidate *head)
{
  //Pass the vertex point 
  // To be applied on the composite particle
  
  //int nd=fDaughters.GetLength(); //unused
  
  mD.ResizeTo(fNcon,fNpar);
  mE.ResizeTo(fNcon,3);
  md.ResizeTo(fNcon,1);
  
  //double ch= fHeadOfTree->GetCharge(); //unused
  //TLorentzVector p1=fHeadOfTree->P4();
  //TVector3 p2=fHeadOfTree->Pos(); 
  TLorentzVector p1=head->P4(); //[ralfk:01.12.11 Try to make it a leaf-by-leaf fit]
  TVector3 p2=head->Pos();  //[ralfk:01.12.11 Try to make it a leaf-by-leaf fit]
  double delX = p2.X() - fpVtx.X();
  double delY = p2.Y() - fpVtx.Y();
  double delZ = p2.Z() - fpVtx.Z();
  
  double px = p1.X();
  double py = p1.Y();
  double pz = p1.Z();
  
  
  //double bField = TRho::Instance()->GetMagnetField(); //unused, why?  
  //double a = -0.0029979246*ch*2.0; //TODO: bfield put manually here?
  double pT = sqrt(px*px + py*py);
  double delT= sqrt(delX*delX + delY*delY);
  double p = sqrt(px*px + py*py + pz*pz);
  double T = sqrt(delX*delX + delY*delY + delZ*delZ);
  
  
  md[fNc+0][0] = (1-delX/delT)/(delY/delT) - (1-px/pT)/(py/pT);
  md[fNc+1][0]= (1-delT/T)/(delZ/T) - (1-(pT/p))/(pz/p);
  
  mD[fNc+0][0]  = -(px/(py*pT) - 1/py);
  mD[fNc+0][1] = -(1/pT - pT/(py*py)+ px/(py*py));
  mD[fNc+0][2] = 0;
  mD[fNc+0][3] = 0; 
  mD[fNc+0][4] = delX/(delY*delT) - 1/delY;
  mD[fNc+0][5] = 1/delT - delT/delY*delY+ delX/(delY*delY);
  mD[fNc+0][6] = 0; 
  
  //half angle solution
  mD[fNc+1][0] = -(px/pz)*(1/p - 1/pT);
  mD[fNc+1][1] = -(py/pz)*(1/p - 1/pT);
  mD[fNc+1][2] = -((1/(pz*pz))*(pT - p) + 1/p);
  mD[fNc+1][3] = 0;
  mD[fNc+1][4] = (delX/delZ)*(1/T - 1/delT);
  mD[fNc+1][5] = (delY/delZ)*(1/T - 1/delT);
  mD[fNc+1][6] = (1/(delZ*delZ))*(delT - T) + 1/T;
  
}

/*
 // better alternate way ......... 
 md[fNc+0][0] = py/p*delX/delT-px/pT*delY/delT;
 md[fNc+1][0]= pz/p*pT/T-pT/p*delZ/T;
 
 mD[fNc+0][0]  = -((py*(dx*px + dy*py))/(delT*pT*pT*PT));
 mD[fNc+0][1]  = -((py*(dx*px + dy*py))/(delT*pT*pT*PT));
 mD[fNc+0][2]  = 0;
 mD[fNc+0][3]  = 0; 
 mD[fNc+0][4]  = (delY*(delX*px + delY*py))/(delT*delT*delT*pT) ;
 mD[fNc+0][5]  = -((delX*(delX*px + delY*py))/(delT*delT*delT*pT)) ;
 mD[fNc+0][6]  = 0;
 
 //2nd equation
 mD[fNc+1][0] = -((px*pz*(delT*pT + dz*pz))/(T*pT*(p*p*p));  //correct 
 mD[fNc+1][1] = -((py*pz*(delT*pT + dz*pz))/(T*pT*(p*p*p));
 mD[fNc+1][2] = (delT*pT*pT + dz*pT*pz)/(delT*p*p*p) ;
 mD[fNc+1][3] = 0;
 mD[fNc+1][4] = (dx*dz*(delT*pT + dz*pz))/(delT*T*T*T*p);
 mD[fNc+1][5] = (dy*dz*(delT*pT + dz*pz))/(delT*T*T*T*p);
 mD[fNc+1][6] = (-(delT*delT*pT) - delT*dz*pz)/(delT*delT*delT*p);
 
 }
 */

void PndKinVtxFitter::GetStartVtx(TVector3 &vertex)
{
  vertex.SetXYZ(0.,0.,0.);
  int nd=fDaughters.GetLength();
  if ( nd <  2 ) vertex.SetXYZ(0.,0.,0.);
  if ( nd == 2 ) GetPocaVtx(vertex,&fDaughters[0],&fDaughters[1]);

  std::vector<Double_t> distances;
  std::vector<TVector3> results;
  // loop over daughters, take the mean value of all "best" positions
  // TODO do this smarter by using already found vertices ?
  TVector3 theVertex(0.,0.,0.);
  Double_t actualDoca=0.;
  for(Int_t daug1 =0;daug1<nd;daug1++)
  {
    TCandidate* a=&fDaughters[daug1];
    for(Int_t daug2=daug1+1;daug2<nd;daug2++)
    {
      TCandidate* b=&fDaughters[daug2];
      actualDoca = GetPocaVtx(theVertex,a,b);
      distances.push_back(actualDoca);
      results.push_back(theVertex);
    }//daug2
  }//daug1
  //TODO invent a smart procedure to get the correct 2-Track doca and something reasonable for many tracks
  // --> Mean = Sum(x/(sigmax^2)) / Sum(Sigmax^2)
  // Averaging vertex results from each track pair how to do that? "geometric" or arithmetic mean?
  std::vector<Double_t>::iterator iterDoca;
  std::vector<TVector3>::iterator iterVtx;
  Double_t docaweight=0,sumdocaweigts=0;
  TVector3 vertexK;
  for(iterVtx=results.begin(), iterDoca=distances.begin();iterVtx!=results.end()&&iterDoca!=distances.end();++iterVtx,++iterDoca)
  {
    docaweight=1/(*iterDoca);
    //docaweight *= docaweight;
    vertexK=*iterVtx;
    if (docaweight == 0) docaweight = 1; // right so?
    vertexK *= docaweight;
    vertex+=vertexK;
    sumdocaweigts+=docaweight;
  }
  if (sumdocaweigts == 0) sumdocaweigts=1;
  vertex*=1./sumdocaweigts;
  //sumdocaweigts = sqrt(sumdocaweigts);
//  return fHeadOfTree->NDaughters()/sumdocaweigts;
}





Float_t PndKinVtxFitter::GetPocaVtx(TVector3 &vertex, TCandidate *a, TCandidate *b)
{
  //Double_t d=1.0, Double_t a=3.14159265358979323846, Double_t r1=0.0, Double_t r2=1.E8
  //Taken from the TVertexSelector .
  
//  if ( fDaughters.GetLength() != 2 ) SVtx->SetXYZ(0.,0.,0.);
  
//  TCandidate a=fDaughters[0];
//  TCandidate b=fDaughters[1];
  //  SVtx->SetXYZ( 0.5, 0.5, 1.0 );
//  SVtx->SetXYZ( 0.0, 0.0, 0.0 );
  //Float_t bField = TRho::Instance()->GetMagnetField();
  Double_t bField=2.0;
  // Position vectors
  TVector3 position1 = a->GetPosition();
  TVector3 position2 = b->GetPosition();
  
  // Momentum vectors
  TVector3 ap3 = a->P3();
  Double_t pPerp1 = ap3.Perp();
  TVector3 d1 = ap3;
  d1.SetZ(0);
  d1*=1.0/pPerp1;
  
  TVector3 bp3 = b->P3();
  Double_t pPerp2 = bp3.Perp();
  TVector3 d2 = bp3;
  d2.SetZ(0);
  d2*=1.0/pPerp2;
  
  
  TVector3 dB(0,0,1.0);
  // Radius and center
  Double_t rho1 = pPerp1/(0.0029979246*bField); // Radius in cm
  TVector3 r1=d1.Cross(dB);
  r1 *= -a->Charge()*rho1;
  TVector3 center1 = position1 - r1;
  center1.SetZ(0);
  
  Double_t rho2 =  pPerp2/(0.0029979246*bField); // Radius in cm
  TVector3 r2=d2.Cross(dB);
  r2 *= -b->Charge()*rho2;
  TVector3 center2 = position2 - r2;
  center2.SetZ(0);
  
  // distance and angle of the axis between the two centers
  TVector3 ab = center2 - center1;
  Double_t dab = ab.Perp();
  Double_t cosTheAB = ab.X()/dab;
  Double_t sinTheAB = ab.Y()/dab;
  
  
  // x value of intersect at reduced system 
  Double_t x = dab/2 + ( rho1*rho1 - rho2*rho2 )/(2*dab); 
  
  // y*y value of intersect at reduced system for helix A
  Double_t y2 = (rho1+x)*(rho1-x); 
  
  // both circles do not intersect (only one solution)
  Int_t nSolMax=1;
  Double_t y=0; 
  if (y2 > 0) {
    nSolMax=2;
    y = sqrt(y2);
  }
  // now we compute the solution(s)
  TVector3 newapos[2];
  TVector3 newbpos[2];
  Int_t best=0;
  double fActualDoca=1.E8;
  //    fActualDoca=0.99999999;
  for (Int_t ns=0; ns<nSolMax; ns++){      // loop on the solutions
                                           // radius vector of intersection point
    Double_t sign = ns ? 1.0 : -1.0;
    TVector3 rs1( cosTheAB*x - sinTheAB*y * sign, sinTheAB*x + cosTheAB*y * sign, 0);  
    TVector3 rs2( rs1-ab );
    
    // are we moving forward or backward?
    Double_t adir=(rs1-r1).Dot(ap3)>0 ? 1.0 : -1.0;
    Double_t aangle=adir * r1.Angle(rs1);
    // intersection point
    Double_t newaz=position1.Z() + rho1*aangle/pPerp1 * ap3.Z();
    newapos[ns].SetX( center1.X() + rs1.X() );
    newapos[ns].SetY( center1.Y() + rs1.Y() );
    newapos[ns].SetZ( newaz );	
    
    // same for b
    Double_t bdir=(rs2-r2).Dot(bp3)>0 ? 1.0 : -1.0;
    Double_t bangle=bdir * r2.Angle(rs2);
    Double_t newbz=position2.Z() + rho2*bangle/pPerp2 * bp3.Z();
    newbpos[ns].SetX( center2.X() + rs2.X());   // ==newapos[ns].X()
    newbpos[ns].SetY( center2.Y() + rs2.Y());   // ==newapos[ns].Y()
    newbpos[ns].SetZ( newbz );
    
    Double_t delta = (newapos[ns]-newbpos[ns]).Mag();
    
    // take the solution of minimal deltaZ
    if ( delta < fActualDoca ) {
	    best=ns;
	    fActualDoca  = delta;
    }
  }
  
//  TVector3 fVertex=0.5*(newapos[best]+newbpos[best]);
//  SVtx->SetXYZ( fVertex.X(), fVertex.Y(), fVertex.Z());
  vertex=0.5*(newapos[best]+newbpos[best]);
  return fActualDoca;
}

void PndKinVtxFitter::TransportToVertex(TMatrixD &a_in, TMatrixD &a_cov_in, TMatrixD &a_out, TMatrixD &a_cov_out, TMatrixD &xref)
{
  
  int fNDau=fDaughters.GetLength();
  int nd=fNDau;
  TMatrixD U(7*nd,7*nd);
  int kN=0;
  for(int k=0; k<nd; k++){
    kN=7*k;
    
    double a = -0.00299792458*2.0*fDaughters[k].GetCharge();
    //   if(fVerbose) cout << "a" << a << endl;
    
    double px=a_in[kN+0][0]; 
    double py=a_in[kN+1][0]; 
    double pz=a_in[kN+2][0];
    double x=a_in[kN+4][0]; 
    double y=a_in[kN+5][0]; 
    double z=a_in[kN+6][0];
    
    double ptot = sqrt(px*px + py*py + pz*pz);
    double rho  = a/ptot;
    double A1 = 1 - pow(pz/ptot,2) - ( (x-xref[0][0])*py - (y-xref[1][0])*px )*rho/ptot ;
    double A2 = (x-xref[0][0])*px + (y-xref[1][0])*py;
    A2 = A2/ptot;
    
    double det  = sqrt(A1*A1+rho*rho*A2*A2);
    double cos_rho_s  =      A1/det; 
    double sin_rho_s  = -rho*A2/det;
    
    //double s = atan2(sin_rho_s,cos_rho_s);
    double s = atan2(sin_rho_s,cos_rho_s)/rho ;
    a_out[kN+0][0] = px*cos_rho_s-py*sin_rho_s;
    a_out[kN+1][0] = py*cos_rho_s+px*sin_rho_s;
    a_out[kN+2][0] = pz; 
    a_out[kN+3][0] = a_in[kN+3][0];
    a_out[kN+4][0] = x + (px*sin_rho_s - py*(1-cos_rho_s))/a;
    a_out[kN+5][0] = y + (py*sin_rho_s + px*(1-cos_rho_s))/a;
    a_out[kN+6][0] = z + (pz/ptot)*s;
    
    U[kN+0][kN+0] =  cos_rho_s;
    U[kN+0][kN+1] = -sin_rho_s;
    
    U[kN+1][kN+0] = sin_rho_s;
    U[kN+1][kN+1] = cos_rho_s;
    
    U[kN+2][kN+2] = 1.;
    U[3+kN][3+kN] = 1.;
    
    U[4+kN][0+kN] = sin_rho_s/a;
    U[4+kN][1+kN] = (1.-cos_rho_s)/a;  
    U[4+kN][4+kN] = 1.;
    
    U[5+kN][0+kN] = (1.-cos_rho_s)/a;
    U[5+kN][1+kN] = sin_rho_s/a;  
    U[5+kN][5+kN] = 1.;
    
    U[6+kN][2+kN] = s/ptot;
    U[6+kN][6+kN] = 1.;
    
    TMatrixD U_t=U;
    U_t=U_t.T();
    a_cov_out = U*a_cov_in*U_t;
  }
}


void PndKinVtxFitter::GetCovariance(TMatrixD &a_cov0, TMatrixD &cov_al_x, TMatrixD &V_vtx, TMatrixD &covS)
 //The covariance for the vitual particle ( a bit complicated)
{
 int fNDau=fDaughters.GetLength();
 int nd=fNDau;


 TMatrixD cov_al_x_temp=cov_al_x;

 TMatrixD pA(4*nd,7*nd);
 pA.Zero();
 double  sumA=0;
 double a;
 int kN, jN;
 for (int k=0;k<nd;k++){   
 kN=k*7;
 jN=k*4;
 a = -0.00299792458*2.0*fDaughters[k].GetCharge();
 sumA += a;
 pA[jN][kN]=pA[jN+1][kN+1]=pA[jN+2][kN+2]=pA[jN+3][kN+3]=1;
 pA[jN+1][kN+4]=-a;
 pA[jN][kN+5]=a;
 }

 TMatrixD pB(4,3);  
 TMatrixD pB_t(3,4);
 pB.Zero();
 pB[0][1]=-sumA;
 pB[1][0]=sumA;

TMatrixD covA_al_xi=pA*cov_al_x;
//TMatrixD covB_al_xi=pB*cov_al_x;
TMatrixD cov_al_xT(3,7*nd);
cov_al_xT = cov_al_x_temp.T();

TMatrixD pA_t(7*nd,4*nd);
//pA_t=pA.T();

//TMatrixD covP(4*nd,4*nd);
TMatrixD covP = pA*a_cov0*(pA_t.Transpose(pA));
TMatrixD covA=covA_al_xi*(pB_t.Transpose(pB));
TMatrixD covB=pB*(cov_al_xT)*(pA_t.Transpose(pA));
TMatrixD covBV=pB*(V_vtx)*(pB_t.Transpose(pB));

TMatrixD covA_al_x(4,3);
TMatrixD SumcovP(4,4);
SumcovP=covBV;

 for (int k=0;k<nd;k++){
 kN=k*7;
 jN=k*4;
 for (int i=0;i<4;i++) {
 for (int j=0;j<4;j++) {
 SumcovP[i][j] += covP[jN+i][jN+j];
 SumcovP[i][j] += covA[jN+i][j];
 SumcovP[i][j] += covB[i][jN+j];
}
 for (int jj=0;jj<3;jj++) {
 covA_al_x[i][jj]  += covA_al_xi[jN+i][jj];
}
}
}
 

TMatrixD covPX(4,3);
covPX = covA_al_x ;
covPX += (pB *V_vtx);
TMatrixD covPX_t(3,4);
covPX_t=covPX_t.Transpose(covPX);



covS.SetSub(0,0,SumcovP);
covS.SetSub(4,0,covPX_t);
covS.SetSub(0,4,covPX);
covS.SetSub(4,4,V_vtx);

}
