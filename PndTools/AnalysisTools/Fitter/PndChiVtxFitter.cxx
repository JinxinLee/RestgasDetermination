#include <iostream>
#include "PndChiVtxFitter.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoBase/TRho.h"
#include "RhoBase/TFactory.h"
#include "TDecompLU.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"

using namespace std;


ClassImp(PndChiVtxFitter)

TBuffer &operator>>(TBuffer &buf, PndChiVtxFitter *&obj)
{
  obj = (PndChiVtxFitter *) buf.ReadObject(PndChiVtxFitter::Class());
  return buf;
}

PndChiVtxFitter::PndChiVtxFitter( const TCandidate& b) : 
VAbsFitter( b )
{
}

PndChiVtxFitter::~PndChiVtxFitter()
{
  //if(fHeadOfTree) delete fHeadOfTree;
}

void PndChiVtxFitter::Fit() 
{
  
  fDaughters.Cleanup();
  FindAndAddGenericDaughters(fHeadOfTree);
  //   fHeadOfTree->RemoveAssociations();
  for (int i=0;i<fDaughters.GetLength();i++)
    fHeadOfTree->AddDaughterLinkSimple(&(fDaughters[i]));
  
  /*
   TCandidate *tc;
   TCandidate theHead=*fHeadOfTree;
   TCandListIterator iter=theHead.DaughterIterator();
   fDaughters.Cleanup();
   while (tc=iter.Next())
   {
   fDaughters.Add(*tc);
   }
   */
  
  int nd=fDaughters.GetLength();
  NumCon=0;
  fdgf =0;
  NumCon = NumCon +2*nd;
  
  SetMatrices();
  ResetMatrices();
  ReadMatrix();
  // Solve();
  // FindVertex();
  Compute();
  SetOutput();
  
}


void PndChiVtxFitter::FindAndAddGenericDaughters(TCandidate *head)
{
  TCandidate *tc;
  TCandListIterator iter=head->DaughterIterator();
  
  while ((tc=iter.Next()))
  {
    if (!tc->IsComposite()) fDaughters.Add(*tc);
    else FindAndAddGenericDaughters(tc);
  }
} 


void PndChiVtxFitter::SetMatrices(){
  int nd=fDaughters.GetLength();
  fNvar=7;
  fNpart=nd;
  fNpar =nd*fNvar;
  fNcon=NumCon;
  //  cout << fNcon << "Num " << endl;
  fNc=0;
  fNiter=0;
  
  al0.ResizeTo(7*nd,1);
  V_al0.ResizeTo(fNpar,fNpar);
  //  mPull.ResizeTo(7*nd,1);
  //  covC.ResizeTo(7,7);
  
  vtxPos.ResizeTo(3,1);
  vtxMom.ResizeTo(3,1);
  xxCov.ResizeTo(3,3);
  xpCov.ResizeTo(3,3);
  ppCov.ResizeTo(3,3);
}

void PndChiVtxFitter::ResetMatrices(){
  al0.Zero();
  V_al0.Zero();
  
  vtxMom.Zero();
  vtxPos.Zero();
  xxCov.Zero();
  xpCov.Zero();
  ppCov.Zero();
}





void PndChiVtxFitter::Compute()
{
  TVector3 startVtx;
  GetStartVtx(&startVtx); 
  TMatrixD last(3,1);
  last[0][0]=startVtx.X();last[1][0]=startVtx.Y();last[2][0]=startVtx.Z();
  cout<<"Initial vertex Position is"<<last[0][0]<<" "<<last[1][0]<<" "<<last[2][0]<<endl;
  
  // the resulting vertex
  niter=0;
  bool good(true);
  while(niter<6)
  { good=FindVertex();
    if(!good){
      //setBad();
      break;
    }
    last -=vtxPos;
    if(last.E2Norm()<=1.E-4) break;
    last  =vtxPos;
    niter++;
  }
}

void PndChiVtxFitter::SetOutput()
{
  TLorentzVector sum(vtxMom[0][0],vtxMom[1][0],vtxMom[2][0],vtxEnergy);
  TVector3 vtx(vtxPos[0][0],vtxPos[1][0],vtxPos[2][0]); 
  cout<<"vtxPos"<<endl;
  fHeadOfTree->SetP7(vtx,sum);
  //fHeadOfTree->SetP4(sum);
  cout << "Energy" << vtxEnergy<< endl;
  cout << "vtxMom" << sum.X() <<" .." <<  sum.Y() <<" .." <<  sum.Z();
  cout << "vtxPos" << vtx.X() <<" .." <<  vtx.Y() <<" .." <<  vtx.Z();
  //  fHeadOfTree->SetPos(vtx);
  TVector3 vtxP(vtxMom[0][0],vtxMom[1][0],vtxMom[2][0]);
  double mass(vtxEnergy*vtxEnergy-vtxP.Mag2());
  mass = ( mass > 0 ) ? sqrt(mass):0; 
  //        TLorentzVector sum1;
  //        sum1.SetXYZM(vtxMom[0][0],vtxMom[1][0],vtxMom[2][0],mass);
  //        fHeadOfTree->SetP4(sum1);
  cout << " mass" << mass << endl;
  
}


bool PndChiVtxFitter::FindVertex()
{
  int nd=fDaughters.GetLength();
  TMatrixD x0(3*nd,1);
  TMatrixD p0(3*nd,1);
  TMatrixD xxw(3*nd,3*nd);
  TMatrixD xpw(3*nd,3*nd);
  TMatrixD ppw(3*nd,3*nd);
  
  TMatrixD ppwnew(3,3);
  // TMatrixD xxCov(3,3);
  // TMatrixD xpCov(3,3);
  // TMatrixD ppCov(3,3);
  TMatrixD Tx(3,1);
  TMatrixD OmegaI(3,3); 
  TMatrixD tempDI(3,3);
  TMatrixD x0I(3,1);
  //Double_t problems=0;
  double fChi2=1e7;
  cout << "niter" << niter << endl;
  if (niter==0){
    for (int k=0;k<nd;k++)
    { 
      for(int i=0;i<3;i++) 
      {
        x0[3*k+i][0]=al0[7*k+i][0];
        p0[3*k+i][0]=al0[7*k+i+3][0]; 
      }
      TMatrixD p2Cov=fDaughters[k].Cov7();
      for(int i=0;i<3;i++) {
        for (int j=0;j<3;j++){
          xxw[3*k+i][3*k+j]=    p2Cov[i][j];
          ppw[3*k+i][3*k+j]=    p2Cov[i+3][j+3];
          xpw[3*k+i][3*k+j]=    p2Cov[i][j+3];
        }
      }
    }
  }
  else
  { 
    for (int k=0;k<nd;k++)
    { 
      for(int i=0;i<3;i++) 
      {
        //    x0[3*k+i][0]=vtxPos[i][0];
        //    p0[3*k+i][0]=vtxMom[i][0];
        x0[3*k+i][0]=al0[7*k+i][0];
        p0[3*k+i][0]=al0[7*k+i+3][0];
      }
      for(int i=0;i<3;i++) {
        for (int j=0;j<3;j++){
          xxw[3*k+i][3*k+j]=    xxCov[i][j];
          ppw[3*k+i][3*k+j]=    ppCov[i][j];
          xpw[3*k+i][3*k+j]-=   xpCov[i][j];
        }
      }
    }
  }
  
  for(int i=0;i<3;i++){
    Tx[i][0]=0;
    vtxMom[i][0]=0;
    vtxPos[i][0]=0;
    for(int j=i;j<3;j++){
      xpCov[i][j]=0;  
      xxCov[i][j]=0;
      ppCov[i][j]=0;
    }
  }
  vtxEnergy=0.;
  //cout<<"ppw"<<endl;
  // ppw.Print();
  TDecompLU lu(ppw);
  if(!lu.Decompose()){cout<<"::FindVertex() Cannot invert.PP matrix Fit not completed"<<endl;return false;} 
  ppw.Invert();
  //  if(problems) return false;
  TMatrixD tempD=xpw*ppw;
  TMatrixD Omega =xxw-tempD*xpw.T();
  for (int k=0;k<nd;k++)
  {  
    OmegaI.Zero();
    OmegaI = Omega.GetSub(k*3,(k+1)*3-1,k*3,(k+1)*3-1);
    xxCov += OmegaI;
    tempDI.Zero();
    tempDI = tempD.GetSub(k*3,(k+1)*3-1,k*3,(k+1)*3-1);
    xpCov += tempDI; 
    ppCov += ppw.GetSub(k*3,(k+1)*3-1,k*3,(k+1)*3-1);
    x0I.Zero();
    x0I=x0.GetSub(k*3,(k+1)*3-1,0,0);
    Tx += OmegaI*x0I;
  }
  
  TDecompLU lu1(xxCov);
  if(!lu1.Decompose()){cout<<"::FindVertex() Cannot invert. xx Matrix Fit not completed"<<endl;return false;} 
  xxCov.Invert();
  //   if(problems) return false;
  
  vtxPos=xxCov*Tx;
  cout<<"vtxPos"<<endl;
  vtxPos.Print();
  for (int k=0;k<nd;k++)
  {
    TMatrixD tempX=vtxPos-x0.GetSub(k*3,(k+1)*3-1,0,0);
    TMatrixD td=tempD.GetSub(k*3,(k+1)*3-1,k*3,(k+1)*3-1);
    TMatrixD p0I=p0.GetSub(k*3,(k+1)*3-1,0,0);
    TMatrixD tempP=p0I-((td.T())*tempX);
    //  tempP=p0I-td.T();
    TVector3 momi=(tempP[0][0],tempP[1][0],tempP[2][0]); 
    double tempM=fDaughters[k].Mass();
    double energy = sqrt(momi.Mag2()+tempM*tempM);
    vtxEnergy += energy;
    TVector3 dx;
    dx=(tempX[0][0],tempX[1][0],tempX[2][0]);
    TMatrixD tempXm=(Omega.GetSub(k*3,(k+1)*3-1,k*3,(k+1)*3-1))*tempX;
    //       cout<<" ******** "<<tempXm.GetNrows()<<" ****** "<<tempXm.GetNcols()<<endl; 
    TVector3 dxm;
    dxm=(tempXm[0][0],tempXm[1][0],tempXm[2][0]);
    fChi2+=dx.Dot(dxm);
    cout << "chi sq" << fChi2 << endl;
    //    xpCov = (-1.0)*xxCov*tempD.GetSub(k*3,(k+1)*3-1,k*3,(k+1)*3-1);
    vtxMom += tempP;
    cout<<"vtxMom"<<endl;
    vtxMom.Print();
    for (int j=0;j<nd;j++){
	    TMatrixD tmp = (tempD.GetSub(j*3,(j+1)*3-1,j*3,(j+1)*3-1).T()*xxCov*tempD.GetSub(k*3,(k+1)*3-1,k*3,(k+1)*3-1));
	    ppCov += tmp;
      fDaughters[k].SetP4(TLorentzVector(tempP[0][0],tempP[1][0],tempP[2][0],energy));
      fDaughters[k].SetPos(TVector3(vtxPos[0][0],vtxPos[1][0],vtxPos[2][0]));
    }
  }
  xpCov=-(1.0)*xxCov*xpCov;
  fGlobChi2=fChi2;
  
  return true;                                                                                                           ;
}

void PndChiVtxFitter::ReadMatrix()
{
  int nd=fDaughters.GetLength();
  for (int k=0;k<nd;k++)
  {
    TLorentzVector p1=fDaughters[k].P4();
    TVector3 p2=fDaughters[k].Pos();
    al0[7*k+0][0]=p2.X(); 
    al0[7*k+1][0]=p2.Y(); 
    al0[7*k+2][0]=p2.Z();
    al0[7*k+3][0]=p1.X(); //px
    al0[7*k+4][0]=p1.Y(); //py
    al0[7*k+5][0]=p1.Z(); //pz
    al0[7*k+6][0]=p1.T(); //e
    //  al0[7*k+6][0]=fDaughters[k].P4().E();
    //  al0[7*k+6][0]=p1.E();
    
    TMatrixD p1Cov=fDaughters[k].Cov7();
    
    for(int i=0;i<7;i++)
    {
      for (int j=0;j<7;j++)
      {
        V_al0[k*7+i][k*7+j]  = p1Cov[i][j];
      }
    }
    
  }
  
}

void PndChiVtxFitter::GetStartVtx(TVector3 * SVtx)
{
  //Double_t d=1.0, Double_t a=3.14159265358979323846, Double_t r1=0.0, Double_t r2=1.E8
  //Taken from the TVertexSelector .
  
  if ( fDaughters.GetLength() != 2 ) SVtx->SetXYZ(0.,0.,0.); 
  
  TCandidate a=fDaughters[0];
  TCandidate b=fDaughters[1];
  //  SVtx->SetXYZ( 0.5, 0.5, 1.0 );
  SVtx->SetXYZ( 0.0, 0.0, 0.0 );
  //Float_t bField = TRho::Instance()->GetMagnetField();
  Double_t bField=2.0;
  // Position vectors
  TVector3 position1 = a.GetPosition();
  TVector3 position2 = b.GetPosition();
  
  // Momentum vectors
  TVector3 ap3 = a.P3();
  Double_t pPerp1 = ap3.Perp();
  TVector3 d1 = ap3;
  d1.SetZ(0);
  d1*=1.0/pPerp1;
  
  TVector3 bp3 = b.P3();
  Double_t pPerp2 = bp3.Perp();
  TVector3 d2 = bp3;
  d2.SetZ(0);
  d2*=1.0/pPerp2;
  
  
  TVector3 dB(0,0,1.0);
  // Radius and center
  Double_t rho1 = pPerp1/(0.0029979246*bField); // Radius in cm
  TVector3 r1=d1.Cross(dB);
  r1 *= -a.Charge()*rho1;
  TVector3 center1 = position1 - r1;
  center1.SetZ(0);
  
  Double_t rho2 =  pPerp2/(0.0029979246*bField); // Radius in cm
  TVector3 r2=d2.Cross(dB);
  r2 *= -b.Charge()*rho2;
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
  
  TVector3 fVertex=0.5*(newapos[best]+newbpos[best]);
  SVtx->SetXYZ( fVertex.X(), fVertex.Y(), fVertex.Z());
}

