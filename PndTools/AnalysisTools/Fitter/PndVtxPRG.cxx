
#include <iostream>
#include "PndVtxPRG.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoBase/TRho.h"
#include "RhoBase/TFactory.h"
#include "TMatrixT.h"

using namespace std;

ClassImp(PndVtxPRG)

TBuffer &operator>>(TBuffer &buf, PndVtxPRG *&obj)
{
  obj = (PndVtxPRG *) buf.ReadObject(PndVtxPRG::Class());
  return buf;
}

//Include only those constraint which need vertex Info....
PndVtxPRG::PndVtxPRG( const TCandidate& b) : 
VAbsFitter( b )
{
  // Assume Zero origin as default
  fPerigee.SetXYZ(0.,0.,0.);
  fTrackArray=0;
  std::cout<<" PndVtxPRG created."<<std::endl;
}

PndVtxPRG::~PndVtxPRG()
{
}


void PndVtxPRG::Fit()
{ // We'll do a vertex finding and Fitting in empty space at a constant magnetic field along z-direction
  // We don't dive into the particle tree, it's just the daughters of this candidate
  
  //TODO: do helix parameters on pocaz in pndanalysis
  
  // - loop on tracks
  TCandidate *tc;
  //TODO: return the poca to the z axis here?
  if ( fHeadOfTree->NDaughters()<2 ) return;
  
  TCandListIterator iter=fHeadOfTree->DaughterIterator();
  while (tc=iter.Next()) {
    // - transform track parameters
    //VAbsMicroCandidate* mic = tc->GetMicroCandidate();
    
    
    // - propagate to prigee point (choose from helix pocaz, prg pocaz, geane pocaz)
    // - don't forget to propagate cov matrix, mind adding the pipe scattering error contribution?
    // - calculate helpful variables
    // - prepare the D' matrix and weights
    // - add values to the fit      
  }
  // - divide by weights
  
  // - Update Tracks
  //SetOutput();
  
}



double PndVtxPRG::CalculateVertexFast(TVector3 &vtx, TMatrixD &cov)
{
  int nTrk = fHeadOfTree->NDaughters();
  
  std::vector<TMatrixD> w;
  std::vector<TMatrixD> xp;
  TMatrixD sumw(3,3);
  TMatrixD sumwx(3,1);
  
  for(int i=0;i<nTrk;i++)
  { 
    TCandidate* tcand = fHeadOfTree->Daughter(i);
    // we assume here that track parameters are delivered at 
    // distance of closest approach to the z axis.
    
    TVector3 pocai = tcand->GetPosition();
    TVector3 momi = tcand->GetMomentum();
    double phi_v = tcand->GetHelixPhi0(); // or momi.Phi();
    double tTheta = tcand->GetHelixTanDip(); // this is ok and will not change
    
    TMatrixD xpi(1,3);
    xpi[0][0]=pocai.X();
    xpi[0][1]=pocai.Y();
    xpi[0][2]=pocai.Z();
    xp.push_back(xpi); // save for later use
    if(fVerbose) std::cout<<" #$#$#$# xpi "; xpi.Print();
    
    Float_t* helixCOV = tcand->GetHelixCov();
    TMatrixD COVi(2,2); // track parameter cov for (epsilon,z_p)
    // TCandidate/TFitParams Helixparams: (D0,Phi0,Omega,Z0,TanDip)
    COVi[0][0]=helixCOV[0];// D0-D0
    COVi[0][1]=helixCOV[3];// D0-z0
    COVi[1][0]=helixCOV[3];// D0-z0
    COVi[1][1]=helixCOV[12];// z0-z0 
    if(fVerbose) std::cout<<" #$#$#$# COVi  "; COVi.Print();
    
    TMatrixD Wi(TMatrixD::kInverted,COVi);
    if(fVerbose) {std::cout<<" #$#$#$# Wi"<<std::endl; Wi.Print();}
    TMatrixD Di(2,3); // Derivative in V, 2x3
    // variables for D Matrix
    double s = sin(phi_v);
    double c = cos(phi_v);
    double t = tTheta;  //tan(theta);
    if(t!=0) t = 1/t; else return -333;
    
    Di[0][0]=s;
    Di[0][1]=-c;
    Di[0][2]=0;
    Di[1][0]=-t*c;
    Di[1][1]=-t*s;
    Di[1][2]=1;
    if(fVerbose) {std::cout<<" #$#$#$# Di  "; Di.Print();}
    
    TMatrixD wi(TMatrixD(Di,TMatrixD::kTransposeMult,Wi),TMatrixD::kMult,Di);
    w.push_back(wi); // save for later use
    if(fVerbose) {std::cout<<" #$#$#$# wi  "; wi.Print();}
    TMatrixD wixpi(wi,TMatrixD::kMultTranspose,xpi);
    if(fVerbose) {std::cout<<" #$#$#$# wixpi  "; wixpi.Print();}
    
    sumw+=wi; // sum up weights
    if(fVerbose) {std::cout<<" #$#$#$# sumw  "; sumw.Print();}
    sumwx+=wixpi; // sum up weighted positions
    if(fVerbose) {std::cout<<" #$#$#$# sumwx  "; sumwx.Print();}
  }
  
  TMatrixD cV(TMatrixD::kInverted,sumw);
  if(fVerbose) std::cout<<" #$#$#$# cV  "; cV.Print();
  TMatrixD V(cV,TMatrixD::kMult,sumwx);// result vertex
  V.T(); // make it a row vector
  if(fVerbose) std::cout<<" #$#$#$# V  "; V.Print();
  
  double chisq=0; // calculate chisquare
  for(int i=0;i<nTrk;i++)
  {
    if(fVerbose) std::cout<<" #$#$#$# V  "; V.Print();
    TMatrixD resid(xp[i],TMatrixD::kMinus,V);
    if(fVerbose) {std::cout<<" #$#$#$# resid  "; resid.Print();}
    if(fVerbose) {std::cout<<" #$#$#$# w["<<i<<"]  "; w[i].Print();}
    TMatrixD chisqi(TMatrixD(resid,TMatrixD::kMult,w[i]),TMatrixD::kMultTranspose,resid);
    if(fVerbose) {std::cout<<" #$#$#$# chisqi  "; chisqi.Print();}
    chisq+=chisqi[0][0];
    if(fVerbose) {std::cout<<" #$#$#$# chisq = "<<chisq<<std::endl;}
  }
  
  // set output. TODO: do that more intelligently?
  vtx.SetXYZ(V[0][0],V[0][1],V[0][2]); // right order of indice?
  cov=cV;
  return chisq;
  
}

double PndVtxPRG::CalculateVertexFull(TVector3 &vtx, TMatrixD &cov)
{
  return -1;
  /*
   
   TMatrixD A += DiT*Wi*Di;
   
   // loop tracks: i
   for(int i=0;i<nTrk;i++){ //TODO
   double rho;
   double theta;
   double epsilon;
   double phi_p;
   double z_p;
   
   // do all the matrix preparations
   TMatrixD COVi(3,3); // track parameter cov for (epsilon,z_p,fi_p)
   TMatrixD Wi=COVi; Wi.T(); // Weight Matrix
   
   TMatrixD Di(3,3); // Derivative in V
   double s = sin(phi_v);
   double c = cos(phi_v);
   double t = cot(theta);
   
   Di[0][0]=s;
   Di[0][1]=-c;
   Di[0][2]=0;
   Di[1][0]=-t*c;
   Di[1][1]=-t*s;
   Di[1][2]=1;
   Di[2][0]=-rho*c;
   Di[2][1]=-rho*s;
   Di[2][2]=0;
   
   TMatrixD DiT=Di; DiT.T();
   
   TMatrixD Ei(3,3); // Derivative in p
   double x_v, y_v;
   double R=y_v*c-x_v*s;
   double Q=(x_v*c+y_v*s);
   Ei[0][0]=0;
   Ei[0][1]=Q;
   Ei[0][2]=-0.5*Q*Q;
   Ei[1][0]=-Q*(1+t*t);
   Ei[1][1]=-R*t;
   Ei[1][2]=Q*R*t;
   Ei[2][0]=0;
   Ei[2][1]=1;
   Ei[2][2]=-Q;
   
   TMatrixD EiT=Ei; EiT.T();
   
   A += DiT*Wi*Di;
   
   TMatrixD Bi = DiT*Wi*Ei;
   
   TMatrixD Ci = EiT*Wi*Ei;
   
   
   
   
   
   }
   */
}


//
//
//Bool_t PndVtxPRG::Propagator(int mode, TCandidate* cand, TVector3* mypoint)
//{
//  //Propagate from the tracks first parameter set to the POCA from mypoint
//  //The candidate is updated but the track not touched 
//  //Only the uncorrelated errors are propagated, 
//  //TODO: implement a real cov matrix
//  
//  Bool_t rc = kFALSE;
//  if(!cand) {
//    Error("Propagator","Candidate not found: %p",cand);
//    return kFALSE;
//  }
//  PndPidCandidate* pidCand = static_cast<PndPidCandidate*>(&cand->GetMicroCandidate());
//  PndTrack* track = (PndTrack*)fTracks->At(pidCand->GetTrackIndex());
//  if (!track) {Warning("Propagator","Could not find track object of index %d",pidCand->GetTrackIndex()); return kFALSE;}
//  FairGeanePro* geaneProp = new FairGeanePro();
//  FairTrackParP tStart = track->GetParamFirst();
//  FairTrackParH* myStart = new FairTrackParH(tStart);
//  FairTrackParH* myResult = new FairTrackParH();
//  Int_t pdgcode = cand->PdgCode();
//  if(fVerbose>0)std::cout<<"Try mode "<<mode<<" with pdgCode "<<pdgcode<<std::endl;
//  if(1==mode && NULL!=mypoint){
//    geaneProp->BackTrackToVertex(); //set where to propagate
//    geaneProp->SetPoint(*mypoint);
//  } else if(2==mode){
//    geaneProp->PropagateToPCA(2, -1);// track back to z axis
//    TVector3 ex1(0.,0.,-10.);
//    TVector3 ex2(0.,0.,10.);
//    geaneProp->SetWire(ex1,ex2);
//  } else return kFALSE;
//  
//  // now we propagate
//  rc = geaneProp->Propagate(myStart, myResult,pdgcode);
//  
//  if (!rc) return kFALSE;
//  TVector3 pos(myResult->GetX(),myResult->GetY(),myResult->GetZ()); // I want to be sure... 
//  //printout for checks
//  TVector3 vecdiff=myStart->GetPosition() - myResult->GetPosition();
//  if(fVerbose>1){
//    std::cout<<"position start     :";  myStart->GetPosition().Print();
//    std::cout<<"position ip        :";  myResult->GetPosition().Print();
//    std::cout<<"position difference:";  vecdiff.Print();
//    vecdiff=myStart->GetMomentum()-myResult->GetMomentum();
//    std::cout<<"momentum start     :";  myStart->GetMomentum().Print();
//    std::cout<<"momentum ip        :";  myResult->GetMomentum().Print();
//    std::coutstd::endl<<"momentum difference:";  vecdiff.Print();
//  }
//  cand->SetPosition(pos);
//  cand->SetP3(myResult->GetMomentum()); // implicitly uses the candidates mass to set P4
//  
//  TMatrixD covPosMom(7,7);
//  for(Int_t ii=0;ii<7;ii++) for(Int_t jj=0;jj<7;jj++) covPosMom[ii][jj]=0.;
//  Double_t A=0;
//  A=myResult->GetDX();
//  covPosMom[0][0]=A*A; // x x
//  A=myResult->GetDY();
//  covPosMom[1][1]=A*A; // y y
//  A=myResult->GetDZ();
//  covPosMom[2][2]=A*A; // z z
//  A=myResult->GetDPx();
//  covPosMom[3][3]=A*A; // px px
//  A=myResult->GetDPy();
//  covPosMom[4][4]=A*A; // py py
//  A=myResult->GetDPz();
//  covPosMom[5][5]=A*A; // pz pz
//  
//  Double_t M=cand->M();
//  Double_t Q=myResult->GetQ();
//  if(0==Q)return kFALSE;
//  A=myResult->GetQp()/Q;
//  A=A*A;
//  A=A*A*(1+M*M*A);
//  if(0==A) return kFALSE;
//  Double_t dA=myResult->GetDQp()/Q;
//  covPosMom[6][6]=dA*dA/A; // e e
//  
//  cand->SetCov7(covPosMom);
//  
//  if(fVerbose>1)Info("Propagator  ","Succsess=%b",rc);
//  return kTRUE;
//}
//
//


