
#include <iostream>
#include "PndVtxPRG.h"
#include "RhoBase/TCandListIterator.h"
#include "RhoBase/TRho.h"
#include "RhoBase/TFactory.h"
#include "TMatrixT.h"
#include "PndAnalysisCalcTools.h"

ClassImp(PndVtxPRG)

TBuffer &operator>>(TBuffer &buf, PndVtxPRG *&obj)
{
  obj = (PndVtxPRG *) buf.ReadObject(PndVtxPRG::Class());
  return buf;
}

PndVtxPRG::PndVtxPRG( const TCandidate& b) : 
VAbsFitter(b),
fDebug(false),
fExpansionPoint(0.,0.,0.),  // Assume Zero origin as default
fPrgCov(5,5),
fJacobian(5,7)
{
  printf("Creating PndVtxPRG. This class is preliminary.");
}

PndVtxPRG::~PndVtxPRG()
{
}

void PndVtxPRG::Fit()
{ 
  Calculate();
}

void PndVtxPRG::Calculate(int niterations)
{ // We'll do a vertex finding and Fitting in empty space at a constant magnetic field along z-direction
  // We don't dive into the particle tree, it's just the daughters of this candidate
  
  // The theta angle and the curvature will not change from the fit, so these dimensions are omitted.
  
  int nTrk = fHeadOfTree->NDaughters();
  
  TVector3 vtx(0.,0.,0.); 
  TVector3 bkExpPoint = fExpansionPoint; // backup vertex seed
  TMatrixD tmpcov(3,3);

  for (int afast=0;afast<3;afast++)
  {
    FitVertexFast(vtx,tmpcov,true);//fast vertex seed (with expansion point used)
    fExpansionPoint=vtx; // then rerun with new expansion point.
  }
  vtx -= fExpansionPoint; // move to relative system
  fExpansionPoint = bkExpPoint; // get back the seed.
  
  Double_t determinant = 0.;
  for (int iteration = 0 ; iteration < niterations ; iteration++)
  {
    std::vector<TMatrixD> B;
    std::vector<TMatrixD> CI; // only Ci^-1 is used
    std::vector<TMatrixD> BCI; // helper to save on multiplications (more memory needed, though)
    std::vector<TMatrixD> D; // helper
    std::vector<TMatrixD> E; //helper
    TMatrixD A(3,3);
    TMatrixD T(3,1);
    std::vector<TMatrixD> U;
    std::vector<TMatrixD> W; 
    std::vector<TMatrixD> dq; // helper
    for(int i=0;i<nTrk;i++)
    { 
      if(fVerbose) {std::cout<<" #$#$#$# Begin #$#$#$# "<<std::endl;}
      if(fVerbose) {std::cout<<" #$#$#$# Expansion point        "; fExpansionPoint.Print();}
      if(fVerbose) {std::cout<<" #$#$#$# Vertex before          "; vtx.Print();}
      if(fVerbose) {std::cout<<" #$#$#$# Vertex before (global) "; (vtx+fExpansionPoint).Print();}
      TCandidate* tcand = fHeadOfTree->Daughter(i);
      // we assume here that track parameters are delivered at 
      // distance of closest approach to the z axis of the extraction point.
      
      const double charge = tcand->GetCharge();
      if (fVerbose) {
        printf("#$#$#$# Helix params:\n\t epsilon\t = %.4g cm\n\t Z0\t = %.4g cm\n\t theta\t = %.4g\n\t phi0\t = %.4g\n\t rho\t = %.4g 1/cm\n\t charge\t = %g e\n",fPrgParams[0],fPrgParams[1],fPrgParams[2],fPrgParams[3],fPrgParams[4],charge);
        std::cout<<" #$#$#$# Helix cov: "; fPrgCov.Print();
      }
      
      
      TVector3 momi = tcand->GetMomentum();
      if(fDebug) {printf(" ##$#$#$# momi: ");momi.Print();}
      TVector3 posi = tcand->GetPosition(); // (printout only!)
      posi -= fExpansionPoint;
      if(fDebug) {printf(" ##$#$#$# pocai:");posi.Print();}
      
      
      
      // all positions relative to fExpansionPoint  !!!!
      
      // get parameters closest to vertex estimate
      Bool_t testvtx = CalcPrgParams(tcand,vtx+fExpansionPoint); 
      if(!testvtx) return;
      
      double s = sin(fPrgParams[3]);
      double c = cos(fPrgParams[3]);
      double t = tan(fPrgParams[2]);
      if(t!=0) t = 1/t; else return;
      // make an iterative loop to update it
      TMatrixD xvi(1,3); // point closest to estimated vertex
      xvi[0][0]=s*fPrgParams[0];
      xvi[0][1]=-c*fPrgParams[0];
      xvi[0][2]=fPrgParams[1];
      if(fVerbose) {std::cout<<" #$#$#$# xvi from seed "; xvi.Print();}
      xvi[0][0]+=vtx.X();
      xvi[0][1]+=vtx.Y();
      xvi[0][2]+=vtx.Z();
      if(fVerbose) {std::cout<<" #$#$#$# xvi "; xvi.Print();}
      double Q=xvi[0][0]*c+xvi[0][1]*s; // at estimated vertex perigee
      double R=xvi[0][1]*c-xvi[0][0]*s;
      if(fDebug) printf(" #$#$#$# Helper variables: Q = %g , R = %g\n",Q,R);
      
      TMatrixD pvi(1,3);
      pvi[0][0]=c*momi.Perp();
      pvi[0][1]=s*momi.Perp();
      pvi[0][2]=momi.Z();
      if(fVerbose) {std::cout<<" #$#$#$# pvi "; pvi.Print();}
      
      
      
      
      
      // get perigee parameters for track
      Bool_t testprg = CalcPrgParams(tcand,fExpansionPoint);
      if(!testprg) return;
      TMatrixD xmi(1,3); // last measured position
      xmi[0][0]=posi.X();
      xmi[0][1]=posi.Y();
      xmi[0][2]=posi.Z();
      if(fVerbose) {std::cout<<" #$#$#$# xmi "; xmi.Print();}
      TMatrixD xpi(1,3); // perigee position
      xpi[0][0]=sin(fPrgParams[3])*fPrgParams[0];
      xpi[0][1]=-cos(fPrgParams[3])*fPrgParams[0];
      xpi[0][2]=fPrgParams[1];
      if(fVerbose) {std::cout<<" #$#$#$# xpi "; xpi.Print();}

      TMatrixD COVi(3,3); // track parameter cov for (epsilon,z_p,Phi_p)
      //double r=pocai.Perp();
      COVi[0][0]=fPrgCov[0][0];// D0-D0
      COVi[0][1]=fPrgCov[0][1];// D0-z0
      COVi[0][2]=fPrgCov[0][3];// D0-Phi0
      COVi[1][0]=fPrgCov[1][0];// D0-z0
      COVi[1][1]=fPrgCov[1][1];// z0-z0
      COVi[1][2]=fPrgCov[1][3];// z0-Phi0
      COVi[2][0]=fPrgCov[3][0];// D0-Phi0
      COVi[2][1]=fPrgCov[3][1];// z0-Phi0
      COVi[2][2]=fPrgCov[3][3];// phi0-phi0
      if(fVerbose) {std::cout<<" #$#$#$# COVi  "; COVi.Print();}      

      //det(aA) = a^n det(A)
      TMatrixD Wi(COVi);
      //Wi *= 100000; // multiply with a biggish number to get reasonable numerics for the inversion
      //if(fVerbose) {std::cout<<" #$#$#$# Wi before inversion "<<std::endl; Wi.Print();}
      Wi.InvertFast(&determinant);
      if (determinant==0) {
        printf("PndVtxPRG: COVi Inversion failed, skipping track %i\n",i);
        continue;
      }
      //Wi *= 100000; // and multiply with the biggish number again
      //TMatrixD Wi(TMatrixD::kInverted,COVi); // no determinant returned -> No check possible
      W.push_back(Wi);
      if(fDebug) {std::cout<<" #$#$#$# Wi (det(cov) = "<<determinant<<")"<<std::endl; Wi.Print();}
      
      TMatrixD Di(3,3); // Derivative in V
      Di[0][0]=s;
      Di[0][1]=-c;
      Di[0][2]=0;
      Di[1][0]=-t*c;
      Di[1][1]=-t*s;
      Di[1][2]=1;
      Di[2][0]=-fPrgParams[4]*c;
      Di[2][1]=-fPrgParams[4]*s;
      Di[2][2]=0;
      D.push_back(Di);
      if(fDebug) {std::cout<<" #$#$#$# Di  "; Di.Print();}
      
      TMatrixD Ei(3,3); // Derivative in p
      Ei[0][0]=0;
      Ei[0][1]=Q;
      Ei[0][2]=-Q*Q/2;
      Ei[1][0]=Q*(1+t*t);// sign?
      Ei[1][1]=-R*t;
      Ei[1][2]=Q*R*t;
      Ei[2][0]=0;
      Ei[2][1]=1;
      Ei[2][2]=-Q;
      E.push_back(Ei);
      if(fDebug) {std::cout<<" #$#$#$# Ei  "; Ei.Print();}
      
      // calculate matrices to be stored: A, Bi, Ci, T, Ui
      TMatrixD DitWi(Di,TMatrixD::kTransposeMult,Wi);
      if(fDebug) {std::cout<<" #$#$#$# DitWi  "; DitWi.Print();}
      
      TMatrixD EitWi(Ei,TMatrixD::kTransposeMult,Wi);
      if(fDebug) {std::cout<<" #$#$#$# EitWi  "; EitWi.Print();}
      
      //
      TMatrixD dqix(Di,TMatrixD::kMultTranspose,xmi);
      if(fDebug) {std::cout<<" #$#$#$# dqix  "; dqix.Print();}
      TMatrixD dqip(Ei,TMatrixD::kMultTranspose,pvi);
      if(fDebug) {std::cout<<" #$#$#$# dqip  "; dqip.Print();}
      
      TMatrixD dqi(dqip,TMatrixD::kPlus,dqix);
      dqi*=-1;
      dq.push_back(dqi);
      if(fDebug) {std::cout<<" #$#$#$# dqi  "; dqi.Print();}
      
      TMatrixD Ai(DitWi,TMatrixD::kMult,Di);
      A += Ai;
      if(fDebug) {std::cout<<" #$#$#$# Ai = DitWiDi "; Ai.Print();}
      
      TMatrixD Bi(DitWi,TMatrixD::kMult,Ei);
      B.push_back(Bi);
      if(fDebug) {std::cout<<" #$#$#$# Bi = DitWiEi "; Bi.Print();}
      
      TMatrixD Ci(EitWi,TMatrixD::kMult,Ei);
      if(fDebug) {std::cout<<" #$#$#$# Ci = EitWiEi "; Ci.Print();}
      
      TMatrixD CIi(Ci);
      //CIi *= 1e6; // catch numerics
      CIi.InvertFast(&determinant);
      if (determinant==0) {
        printf("PndVtxPRG: Ci Inversion failed, skipping track %i\n",i);
        continue;
      }
      //CIi *= 1e6; // catch numerics
      //TMatrixD CIi(TMatrixD::kInverted,Ci); // no determinant returned -> No check possible
      CI.push_back(CIi);
      if(fDebug) {std::cout<<" #$#$#$# CIi = Ci^-1 "; CIi.Print();}
      
      TMatrixD BiCIi(Bi,TMatrixD::kMult,CIi);
      BCI.push_back(BiCIi);
      if(fDebug) {std::cout<<" #$#$#$# BiCIi "; BiCIi.Print();}
      
      TMatrixD Ti(DitWi,TMatrixD::kMult,dqi);
      T += Ti;
      if(fDebug) {std::cout<<" #$#$#$# Ti = DitWi*dqi "; Ti.Print();}
      
      TMatrixD Ui(EitWi,TMatrixD::kMult,dqi);
      U.push_back(Ui);
      if(fDebug) {std::cout<<" #$#$#$# Ui = EitWi*dqi"; Ui.Print();}
      
    } // loop tracks
    
    // Calculate Vertex update
    TMatrixD WV(A);
    TMatrixD Vpre(T);
    for(int i=0;i<nTrk;i++)
    { 
      TMatrixD BiCIiUi(BCI[i],TMatrixD::kMult,U[i]);
      Vpre-=BiCIiUi;
      TMatrixD BiCIiBti(BCI[i],TMatrixD::kMultTranspose,B[i]);
      WV-=BiCIiBti;
    }
    TMatrixD CovVV(WV);
    CovVV.InvertFast(&determinant);
    if (determinant==0) {
      printf("PndVtxPRG: WV Inversion failed, Vertex cov screwed. What to do???\n");
    }
    //TMatrixD CovVV(TMatrixD::kInverted,WV); // no determinant returned -> No check possible
    TMatrixD V(CovVV,TMatrixD::kMult,Vpre);
    vtx.SetXYZ(V[0][0],V[1][0],V[2][0]); 
    if(fVerbose) {std::cout<<" #$#$#$# Vertex after          "; vtx.Print();}
    if(fVerbose) {std::cout<<" #$#$#$# Vertex after (global) "; (vtx+fExpansionPoint).Print();}
   
    // Calculate Momentum updates & Chi^2
    std::vector<TMatrixD> P;
    std::vector<TMatrixD> uq;
    Double_t chisquare=0;
    for(int i=0;i<nTrk;i++)
    { 
      TMatrixD BtiV(B[i],TMatrixD::kTransposeMult,V);
      TMatrixD Pi(CI[i],TMatrixD::kMult,U[i]-BtiV);
      P.push_back(Pi);
      // track params
      TMatrixD DiV(D[i],TMatrixD::kMult,V);
      TMatrixD uqi(E[i],TMatrixD::kMult,Pi);
      uqi+=DiV;
      uq.push_back(uqi);
      //chisquare
      TMatrixD dqitWi(dq[i],TMatrixD::kTransposeMult,W[i]);
      TMatrixD chis(dqitWi,TMatrixD::kMult,dq[i]);
      chisquare+=chis[0][0];
    }
     
    // TODO: end iteration loop at satisfying chisq?
    if(iteration != niterations - 1 ) continue;
    
    // calculating other cov matrices, switch off to be faster?
    std::vector<TMatrixD> CovVP;
    std::vector< std::vector<TMatrixD> > CovPP;
    for(int i=0;i<nTrk;i++)
    { 
      TMatrixD CovVPi(CovVV,TMatrixD::kMult,BCI[i]);
      CovVPi*=-1;
      CovVP.push_back(CovVPi);
      std::vector<TMatrixD> CovPPi;
      for(int j=0;j<nTrk;j++)
      {
        TMatrixD CovPPijtmp(CI[j],TMatrixD::kMultTranspose,B[j]);
        TMatrixD CovPPij(CovPPijtmp,TMatrixD::kMult,CovVPi);
        if(i==j) CovPPij+=CI[j];
        CovPPi.push_back(CovPPij);
      }
      CovPP.push_back(CovPPi);
    }
    
    // update particle candidates    
    vtx+=fExpansionPoint; // move back to lab coordinates
    TMatrixD CovP7(7,7);
    for(int k=0;k<3;k++) for(int l=0;l<3;l++) 
      CovP7[k][l]=CovVV[k][l];
    
    for(int i=0;i<nTrk;i++)
    { 
      // copy original candidate    
      TCandidate* tcand = fHeadOfTree->Daughter(i);
      TCandidate* fitted = new TCandidate(*tcand);
      // set fitted values
      fitted->SetPos(vtx);
      TVector3 momi( (P[i])[0][0],(P[i])[1][0],(P[i])[2][0] );
      fitted->SetP3(momi);
      for(int k=0;k<3;k++) for(int l=0;l<3;l++){
        CovP7[k+3][l+3]=CovPP[i][i][k][l]; // momentum cov
        CovP7[k+3][l]=(CovVP[i])[k][l];   // momentum-position cov
        CovP7[l][k+3]=CovP7[k+3][l];   // momentum-position cov
      }
      CovP7[6][6]= (tcand->GetErrP7())[27];// error in e
      fitted->SetCov7(CovP7);
      fitted->SetChi2(chisquare);
      //TODO: Update helix parameters & cov here, too.
      // put fitted candidate
      tcand->SetFit(fitted);
    }
    break;
  }// end of iteratoin loop
  
}


Bool_t PndVtxPRG::CalcPrgParams(TCandidate* tcand, TVector3 expansionpoint)
{
  // calculate helix and its covariance
  
  TLorentzVector mom = tcand->P4();
  TVector3 pos= tcand->GetPosition();
  double charge = tcand->GetCharge();
  TMatrixD cov77 = tcand->Cov7();
  Bool_t test = PndAnalysisCalcTools::P7toPRG(pos, mom, charge, cov77, expansionpoint, fPrgParams, fPrgCov, fJacobian, false);
  return test;
}

double PndVtxPRG::FitVertexFast(TVector3 &vtx, TMatrixD &cov, bool skipcov)
{
  // Calculate a vertex of n tracks without considering the changes in momentum vector
  // the variables vtx & cov (3x3) are written and the Chi^2 is returned.
  
  int nTrk = fHeadOfTree->NDaughters();
  
  std::vector<TMatrixD> w;
  std::vector<TMatrixD> xp;
  TMatrixD sumw(3,3);
  TMatrixD sumwx(3,1);
  Double_t determinant = 0.;
  
  for(int i=0;i<nTrk;i++)
  { 
    TCandidate* tcand = fHeadOfTree->Daughter(i);
    // get Helix in PRG notation from Billoir paper 
    // (epsilon,z0,theta,phi0,rho)
    Bool_t testprg = CalcPrgParams(tcand,fExpansionPoint);
    if(!testprg) return -2;
    
    if (fVerbose) {
      printf("#$#$#$# Helix params:\n\t epsilon\t = %.4g cm\n\t Z0\t = %.4g cm\n\t theta\t = %.4g\n\t phi0\t = %.4g\n\t rho\t = %.4g\n",fPrgParams[0],fPrgParams[1],fPrgParams[2],fPrgParams[3],fPrgParams[4]);
      std::cout<<" #$#$#$# Helix cov: "; fPrgCov.Print();
    }
    
    double s = sin(fPrgParams[3]);
    double c = cos(fPrgParams[3]);
    double t = TMath::Tan(fPrgParams[2]);
    if(t!=0.) t = 1/t; else return -3; // t = cot(theta) 
    
    TMatrixD xpi(1,3);
    xpi[0][0]=s*fPrgParams[0];
    xpi[0][1]=-c*fPrgParams[0];
    xpi[0][2]=fPrgParams[1];
    xp.push_back(xpi); // save for later use
    if(fVerbose) {std::cout<<" #$#$#$# xpi "; xpi.Print();}
    
    TMatrixD COVi(2,2); // track parameter cov for (epsilon,z0)
    COVi[0][0]=fPrgCov[0][0];
    COVi[0][1]=COVi[1][0]=fPrgCov[1][0];
    COVi[1][1]=fPrgCov[1][1];
    if(fVerbose) {std::cout<<" #$#$#$# COVi  "; COVi.Print();}
    
    //propagate to Vertex    
    TMatrixD Wi(COVi);
    Wi.Invert(&determinant);
    if (determinant == 0.) {
      printf("PndVtxPRG: COVi Inversion failed, skipping track %i\n",i);
      continue;
    }
    //TMatrixD Wi(TMatrixD::kInverted,COVi); // no determinant returned -> No check possible
    if(fDebug) {std::cout<<" #$#$#$# Wi"<<std::endl; Wi.Print();}
    TMatrixD Di(2,3); // Derivative in V, 2x3
    
    Di[0][0]=s;
    Di[0][1]=-c;
    Di[0][2]=0.;
    Di[1][0]=-t*c;
    Di[1][1]=-t*s;
    Di[1][2]=1.;
    if(fDebug) {std::cout<<" #$#$#$# Di  "; Di.Print();}
    
    TMatrixD wi(TMatrixD(Di,TMatrixD::kTransposeMult,Wi),TMatrixD::kMult,Di);
    w.push_back(wi); // save for later use
    if(fDebug) {std::cout<<" #$#$#$# wi  "; wi.Print();}
    TMatrixD wixpi(wi,TMatrixD::kMultTranspose,xpi);
    if(fDebug) {std::cout<<" #$#$#$# wixpi  "; wixpi.Print();}
    
    sumw+=wi; // sum up weights
    if(fDebug) {std::cout<<" #$#$#$# sumw  "; sumw.Print();}
    sumwx+=wixpi; // sum up weighted positions
    if(fDebug) {std::cout<<" #$#$#$# sumwx  "; sumwx.Print();}
  }
  
  TMatrixD cV(sumw); 
  cV.InvertFast(&determinant);
  if (determinant==0) {
    printf("PndVtxPRG: sumw Inversion failed, retunring -777 \n");
    return -777;
  }
  //TMatrixD cV(TMatrixD::kInverted,sumw); // no determinant returned -> No check possible
  if(fVerbose) {std::cout<<" #$#$#$# cV  "; cV.Print();}
  TMatrixD V(cV,TMatrixD::kMult,sumwx);// result vertex
  V.T(); // make it a row vector
  if(fVerbose) {std::cout<<" #$#$#$# V  "; V.Print();}
  vtx.SetXYZ(V[0][0],V[0][1],V[0][2]);
  vtx += fExpansionPoint; // move back to origin
  
  if (skipcov) return 0.; // skip chisqare calculation
  
  double chisq=0.; // calculate chisquare
  for(int i=0;i<nTrk;i++)
  {
    if(fDebug) {std::cout<<" #$#$#$# V  "; V.Print();}
    TMatrixD resid(xp[i],TMatrixD::kMinus,V);
    if(fDebug) {std::cout<<" #$#$#$# resid  "; resid.Print();}
    if(fDebug) {std::cout<<" #$#$#$# w["<<i<<"]  "; w[i].Print();}
    TMatrixD chisqi(TMatrixD(resid,TMatrixD::kMult,w[i]),TMatrixD::kMultTranspose,resid);
    if(fDebug) {std::cout<<" #$#$#$# chisqi  "; chisqi.Print();}
    chisq+=chisqi[0][0];
  }
  if(fVerbose) {std::cout<<" #$#$#$# chisq = "<<chisq<<std::endl;}
  cov=cV;
  return chisq;
  
}

double PndVtxPRG::FitVertexFull(TVector3 &vtx, TMatrixD &cov, int niterations)
{
  // Calculate the Vertex in the full scheme
  // TODO: Momentum is fitted, too
  // TODO: The candidates have to be updated
  // TODO: Do we want to set Vertex seed automatically? No!
  Calculate(niterations);
  TCandidate* tcand = fHeadOfTree->Daughter(0);
  const TCandidate* fitted = tcand->GetFit();
  vtx = fitted->Pos();
  TMatrixD cov7 = fitted->Cov7();
  for(int i=0;i<3;i++)for(int j=0;j<3;j++){
    cov[i][j]=cov7[i][j];
  }
  
  return fitted->Chi2(); 
  
}




