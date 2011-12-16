
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
  std::cout<<"Creating PndVtxPRG. This class is preliminary."<<std::endl;;
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
  //TVector3 bkExpPoint = fExpansionPoint; // backup vertex seed
  TMatrixD tmpcov(3,3);
  bool verbi=fVerbose;
  fVerbose=false;
  bool debi=fDebug;
  fDebug=false;
  //for (int afast=0;afast<4;afast++)
  //{
  FitVertexFast(vtx,tmpcov,false);//fast vertex seed (with expansion point used)
  //if(fVerbose) {std::cout<<" #$# Fit #$# Vertex before a, iteration"<<afast<<": "; vtx.Print();}
  //fExpansionPoint=vtx; // then rerun with new expansion point.
  //}
  //fExpansionPoint = bkExpPoint; // get back the seed.
  fVerbose=verbi;
  fDebug=debi;
  if(fVerbose) {std::cout<<" #$# Fit #$# Vertex after fast prefit: "; vtx.Print();}
  //vtx -= fExpansionPoint; // move to relative system
  
  Double_t determinant = 0.;
  for (int iteration = 0 ; iteration < niterations ; iteration++)
  {
    std::cout<<"Iteration "<<iteration<<std::endl;
    if(fVerbose) {std::cout<<" #$# Fit #$# Begin iteration "<<iteration<<" #$#$#$# "<<std::endl;}
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
    TMatrixD CovFitFull(3+3*nTrk,3+3*nTrk); //3 vtx coordinates, nTrk*3 momenta
    for(int i=0;i<nTrk;i++)
    { 
      if(fVerbose) {std::cout<<" #$# Fit #$# track "<<i<<" #$#$#$# "<<std::endl;}
      if(fVerbose) {std::cout<<" #$# Fit #$# Expansion point        "; fExpansionPoint.Print();}
      //if(fVerbose) {std::cout<<" #$# Fit #$# Vertex before          "; vtx.Print();}
      //if(fVerbose) {std::cout<<" #$# Fit #$# Vertex before (global) "; (vtx+fExpansionPoint).Print();}
      if(fVerbose) {std::cout<<" #$# Fit #$# Vertex before (global) "; (vtx).Print();}
      TCandidate* tcand = fHeadOfTree->Daughter(i);
      // we assume here that track parameters are delivered at 
      // distance of closest approach to the z axis of the extraction point.
      
      const double charge = tcand->GetCharge();
      if (fVerbose) {
        printf("#$# Fit #$# Helix params:\n\t epsilon = %.4g cm\n\t Z0\t = %.4g cm\n\t theta\t = %.4g\n\t phi0\t = %.4g\n\t rho\t = %.4g 1/cm\n\t charge\t = %g e\n",fPrgParams[0],fPrgParams[1],fPrgParams[2],fPrgParams[3],fPrgParams[4],charge);
        std::cout<<" #$# Fit #$# Helix cov: "; fPrgCov.Print();
      }
      
      
      TVector3 momi = tcand->GetMomentum();
      if(fDebug) {printf(" ##$# Fit #$# momi: ");momi.Print();}
      TVector3 posi = tcand->GetPosition(); // (printout only!)
      posi -= fExpansionPoint;
      if(fDebug) {printf(" ##$# Fit #$# pocai:");posi.Print();}
      
      
      
      // all positions relative to fExpansionPoint  !!!!
      
      // get parameters closest to vertex estimate
      Bool_t testvtx = CalcPrgParams(tcand,vtx); 
      //      Bool_t testvtx = CalcPrgParams(tcand,vtx+fExpansionPoint); 
      if(!testvtx) return;
      TMatrixD qiv(3,1);
      qiv[0][0]=fPrgParams[0];//epsilon
      qiv[1][0]=fPrgParams[1];//z0
      qiv[2][0]=fPrgParams[3];//phi0
      double s = sin(fPrgParams[3]);
      double c = cos(fPrgParams[3]);
      double t = tan(fPrgParams[2]);
      if(t!=0) t = 1/t; else return;
      // make an iterative loop to update it
      TMatrixD xvi(1,3); // point closest to estimated vertex
      xvi[0][0]=s*fPrgParams[0];
      xvi[0][1]=-c*fPrgParams[0];
      xvi[0][2]=fPrgParams[1];
      if(fVerbose) {std::cout<<" #$# Fit #$# xvi from seed "; xvi.Print();}
      xvi[0][0]+=vtx.X()-fExpansionPoint.X();
      xvi[0][1]+=vtx.Y()-fExpansionPoint.Y();
      xvi[0][2]+=vtx.Z()-fExpansionPoint.Z();
      if(fVerbose) {std::cout<<" #$# Fit #$# xvi "; xvi.Print();}
      double Q=xvi[0][0]*c+xvi[0][1]*s; // at estimated vertex perigee
      double R=xvi[0][1]*c-xvi[0][0]*s;
      if(fDebug) printf(" #$# Fit #$# Helper variables: Q = %g , R = %g\n",Q,R);
      
      TMatrixD pvi(1,3);
      pvi[0][0]=c*momi.Perp();
      pvi[0][1]=s*momi.Perp();
      pvi[0][2]=momi.Z();
      if(fVerbose) {std::cout<<" #$# Fit #$# pvi "; pvi.Print();}
      
      // get perigee parameters for measured track
      Bool_t testprg = CalcPrgParams(tcand,fExpansionPoint);
      if(!testprg) {printf("#$# Fit #$#   CANNOT CALCULATE TRACK PARAMETERS"); return;}
      TMatrixD qip(3,1);
      qip[0][0]=fPrgParams[0];//epsilon
      qip[1][0]=fPrgParams[1];//z0
      qip[2][0]=fPrgParams[3];//phi0
      //TMatrixD xmi(1,3); // last measured position
      //xmi[0][0]=posi.X();
      //xmi[0][1]=posi.Y();
      //xmi[0][2]=posi.Z();
      //if(fVerbose) {std::cout<<" #$# Fit #$# xmi "; xmi.Print();}
      TMatrixD xpi(1,3); // perigee position
      xpi[0][0]=sin(fPrgParams[3])*fPrgParams[0];
      xpi[0][1]=-cos(fPrgParams[3])*fPrgParams[0];
      xpi[0][2]=fPrgParams[1];
      if(fVerbose) {std::cout<<" #$# Fit #$# xpi "; xpi.Print();}
      
      TMatrixD COVi(3,3); // track parameter cov for (epsilon,z_p,Phi_p)
      //double r=pocai.Perp();
      COVi[0][0]=fPrgCov[0][0];// epsilon-epsilon
      COVi[0][1]=fPrgCov[0][1];// epsilon-z0
      COVi[0][2]=fPrgCov[0][3];// epsilon-Phi0
      COVi[1][0]=fPrgCov[1][0];// epsilon-z0
      COVi[1][1]=fPrgCov[1][1];// z0-z0
      COVi[1][2]=fPrgCov[1][3];// z0-Phi0
      COVi[2][0]=fPrgCov[3][0];// epsilon-Phi0
      COVi[2][1]=fPrgCov[3][1];// z0-Phi0
      COVi[2][2]=fPrgCov[3][3];// phi0-phi0
      if(true||fVerbose) {std::cout<<" #$# Fit #$# COVi  "; COVi.Print();}      
      
      //det(aA) = a^n det(A)
      TMatrixD Wi(COVi);
      //Wi *= 100000; // multiply with a biggish number to get reasonable numerics for the inversion
      //if(fVerbose) {std::cout<<" #$# Fit #$# Wi before inversion "<<std::endl; Wi.Print();}
      determinant=0.;
      Wi.InvertFast(&determinant);
      if (determinant==0) {
        std::cout<<"PndVtxPRG: COVi Inversion failed, abort fit."<<std::endl;
        return;
      }
      //Wi *= 100000; // and multiply with the biggish number again
      //TMatrixD Wi(TMatrixD::kInverted,COVi); // no determinant returned -> No check possible
      W.push_back(Wi);
      if(fDebug) {std::cout<<" #$# Fit #$# Wi (det(cov) = "<<determinant<<")"<<std::endl; Wi.Print();}
      for(int sdsd=0;sdsd<3;sdsd++) for(int asas=sdsd;asas<3;asas++){
        printf("W{%i}[%i][%i] = %6.9f\n",i,sdsd,asas,Wi[sdsd][asas]);
        printf("W{%i}[%i][%i] = %6.9f\n",i,asas,sdsd,Wi[asas][sdsd]);
      }
      
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
      if(fDebug) {std::cout<<" #$# Fit #$# Di  "; Di.Print();}
      
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
      if(fDebug) {std::cout<<" #$# Fit #$# Ei  "; Ei.Print();}
      
      // calculate matrices to be stored: A, Bi, Ci, T, Ui
      TMatrixD DitWi(Di,TMatrixD::kTransposeMult,Wi);
      if(fDebug) {std::cout<<" #$# Fit #$# DitWi  "; DitWi.Print();}
      
      TMatrixD EitWi(Ei,TMatrixD::kTransposeMult,Wi);
      if(fDebug) {std::cout<<" #$# Fit #$# EitWi  "; EitWi.Print();}
      
      //dqi = q_measured(Perigee) - F_0(xvi,pvi) == dip-qiv;
      TMatrixD dqi(qip,TMatrixD::kMinus,qiv);
      dq.push_back(dqi);
      if(fDebug) {std::cout<<" #$# Fit #$# dqi  "; dqi.Print();}
      
      TMatrixD Ai(DitWi,TMatrixD::kMult,Di);
      A += Ai;
      if(fDebug) {std::cout<<" #$# Fit #$# Ai ("<<i<<") = DitWiDi "; Ai.Print();}
      
      TMatrixD Bi(DitWi,TMatrixD::kMult,Ei);
      B.push_back(Bi);
      if(fDebug) {std::cout<<" #$# Fit #$# Bi = DitWiEi "; Bi.Print();}
      
      TMatrixD Ci(EitWi,TMatrixD::kMult,Ei);
      if(fDebug) {std::cout<<" #$# Fit #$# Ci = EitWiEi "; Ci.Print();}
      for(int sdsd=0;sdsd<3;sdsd++) for(int asas=sdsd;asas<3;asas++){
        printf("C{%i}[%i][%i] = %6.9f\n",i,sdsd,asas,Ci[sdsd][asas]);
        printf("C{%i}[%i][%i] = %6.9f\n",i,asas,sdsd,Ci[asas][sdsd]);
      }
      
      TMatrixD CIi(Ci);
      //CIi *= 1e6; // catch numerics
      determinant=0.;
      CIi.InvertFast(&determinant);
      if (determinant==0) {
        std::cout<<"PndVtxPRG: CIi Inversion failed, abort fit."<<std::endl;
        return;
      }
      //CIi *= 1e6; // catch numerics
      //TMatrixD CIi(TMatrixD::kInverted,Ci); // no determinant returned -> No check possible
      CI.push_back(CIi);
      for(int sdsd=0;sdsd<3;sdsd++) for(int asas=sdsd;asas<3;asas++){
        printf("CI{%i}[%i][%i] = %6.9f\n",i,sdsd,asas,CI[i][sdsd][asas]);
        printf("CI{%i}[%i][%i] = %6.9f\n",i,asas,sdsd,CI[i][asas][sdsd]);
      }
      if(fDebug) {std::cout<<" #$# Fit #$# CIi = Ci^-1 "; CIi.Print();}
      
      TMatrixD BiCIi(Bi,TMatrixD::kMult,CIi);
      BCI.push_back(BiCIi);
      if(fDebug) {std::cout<<" #$# Fit #$# BiCIi "; BiCIi.Print();}
      
      TMatrixD Ti(DitWi,TMatrixD::kMult,dqi);
      T += Ti;
      if(fDebug) {std::cout<<" #$# Fit #$# Ti = DitWi*dqi "; Ti.Print();}
      
      TMatrixD Ui(EitWi,TMatrixD::kMult,dqi);
      U.push_back(Ui);
      if(fDebug) {std::cout<<" #$# Fit #$# Ui = EitWi*dqi"; Ui.Print();}
      
    } // loop tracks
    if(fDebug) {std::cout<<" #$# Fit #$# A = "; A.Print();}

    // Calculate Vertex update
    TMatrixD WV(A);
    if(fDebug) {std::cout<<" #$# Fit #$# WV = "; WV.Print();}
    TMatrixD Vpre(T);
    for(int i=0;i<nTrk;i++)
    { 
      TMatrixD BCIi(B[i],TMatrixD::kMult,CI[i]);
      TMatrixD BiCIiUi(BCIi,TMatrixD::kMult,U[i]);
      Vpre-=BiCIiUi;
      if(true||fDebug) {std::cout<<" #$# Fit #$# CIi ("<<i<<") "; CI[i].Print();}
      if(true||fDebug) {std::cout<<" #$# Fit #$# Bi ("<<i<<") "; B[i].Print();}
      TMatrixD BiCIiBti(BCIi,TMatrixD::kMultTranspose,B[i]);
      if(true||fDebug) {std::cout<<" #$# Fit #$# BiCIiBti ("<<i<<") = "; BiCIiBti.Print();}
      WV-=BiCIiBti;
      if(true||fDebug) {std::cout<<" #$# Fit #$# WV = "; WV.Print();}
    }
    TMatrixD CovVV(WV);
    CovVV.InvertFast(&determinant);
    if (determinant==0) {
      std::cout<<"PndVtxPRG: WV Inversion failed, abort fit."<<std::endl;
      return;
    }
    if(true||fDebug) {std::cout<<" #$# Fit #$# CovVV = "; CovVV.Print();}
    //TMatrixD CovVV(TMatrixD::kInverted,WV); // no determinant returned -> No check possible
    TMatrixD dV(CovVV,TMatrixD::kMult,Vpre);
    vtx.SetXYZ(dV[0][0],dV[1][0],dV[2][0]); 
    if(fVerbose) {std::cout<<" #$# Fit #$# Vertex after          "; vtx.Print();}
    vtx+=fExpansionPoint; // move back to lab coordinates
    if(fVerbose) {std::cout<<" #$# Fit #$# Vertex after (global) "; vtx.Print();}
    
    // Calculate Momentum updates & Chi^2
    std::vector<TMatrixD> P;
    std::vector<TMatrixD> uq;
    //dV.T();
    Double_t chisquare=0;
    for(int i=0;i<nTrk;i++)
    { 
      TMatrixD BtiV(B[i],TMatrixD::kTransposeMult,dV);
      TMatrixD Pi(CI[i],TMatrixD::kMult,U[i]-BtiV);
      P.push_back(Pi);
      // track params
      TMatrixD uvi(D[i],TMatrixD::kMult,dV);
      TMatrixD uqi(E[i],TMatrixD::kMult,Pi);
      dq[i]-=uvi;
      dq[i]-=uqi;
      //chisquare
      TMatrixD dqitWi(dq[i],TMatrixD::kTransposeMult,W[i]);
      TMatrixD chis(dqitWi,TMatrixD::kMult,dq[i]);
      chisquare+=chis[0][0];
    }
    
    // TODO: end iteration loop at satisfying chisq?
    //if(iteration != niterations - 1 ) continue;
    
    // calculating other cov matrices, switch off to be faster?
    
    // POS COV
    for(int k=0;k<3;k++) for(int l=0;l<3;l++){
      CovFitFull[k][l]=CovVV[k][l];
    }
    if(fDebug) {std::cout<<" #$# Fit #$# CovVV: "; CovVV.Print();}
    
    // MOM-POS COV
    for(int i=0;i<nTrk;i++)
    { 
      TMatrixD CovVPi(CovVV,TMatrixD::kMult,BCI[i]);
      CovVPi*=-1.;
      for(int k=0;k<3;k++) for(int l=0;l<3;l++){
        CovFitFull[k+3*(i+1)][l]=CovVPi[k][l]; ;
        CovFitFull[l][k+3*(i+1)]=CovVPi[k][l]; ;
      }
      if(fDebug) {std::cout<<" #$# Fit #$# CovVPi "<<i<<": "; CovVPi.Print();}
      
      // MOM-MOM COV
      for(int j=0;j<nTrk;j++)
      {
        // Caution: Identity of tracks i and j switch, to use CovVPi from above.
        TMatrixD CovPPijtmp(CI[j],TMatrixD::kMultTranspose,B[j]);
        TMatrixD CovPPij(CovPPijtmp,TMatrixD::kMult,CovVPi);
        CovPPij*=-1.;
        if(i==j) CovPPij+=CI[j];
        for(int k=0;k<3;k++) for(int l=0;l<3;l++){
          CovFitFull[k+3*(j+1)][l+3*(i+1)]=CovPPij[k][l]; // momentum cov
          CovFitFull[l+3*(i+1)][k+3*(j+1)]=CovPPij[k][l]; // momentum cov
        }        
        if(fDebug) {std::cout<<" #$# Fit #$# CovPPij "<<i<<","<<j<<": "; CovPPij.Print();}
      }
    }
    if(fDebug) {std::cout<<" #$# Fit #$# CovFitFull: "; CovFitFull.Print();}
    
    
    //TODO... Continue checking
    
    // update particle candidates    
    TMatrixD CovP7(7,7);
    for(int k=0;k<3;k++) for(int l=0;l<3;l++) 
      CovP7[k][l]=CovVV[k][l];
    if(fDebug) {std::cout<<" #$# Fit #$# CovP7 only vtx: "; CovP7.Print();}
    
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
        CovP7[k+3][l+3]=CovFitFull[3*(i+1)+k][3*(i+1)+l]; // momentum cov
        CovP7[k+3][l]=CovFitFull[3*(i+1)+k][l];   // momentum-position cov
        CovP7[l][k+3]=CovFitFull[l][3*(i+1)+k];   // momentum-position cov
      }
      CovP7[6][6]= (tcand->GetErrP7())[27];// error in e
      fitted->SetCov7(CovP7);
      fitted->SetChi2(chisquare);
      //TODO: Update helix parameters & cov here, too.
      // put fitted candidate
      tcand->SetFit(fitted);
      if(fDebug) {std::cout<<" #$# Fit #$# CovP7 with mom for tracj "<<i<<": "; CovP7.Print();}
    }
    // TODO: combine fitted daughters to a fitted mother!
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
      printf("#$# Fast #$# Helix params:\n\t epsilon\t = %.4g cm\n\t Z0\t = %.4g cm\n\t theta\t = %.4g\n\t phi0\t = %.4g\n\t rho\t = %.4g\n",fPrgParams[0],fPrgParams[1],fPrgParams[2],fPrgParams[3],fPrgParams[4]);
      std::cout<<" #$# Fast #$# Helix cov: "; fPrgCov.Print();
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
    if(fVerbose) {std::cout<<" #$# Fast #$# xpi "; xpi.Print();}
    
    TMatrixD COVi(2,2); // track parameter cov for (epsilon,z0)
    COVi[0][0]=fPrgCov[0][0];
    COVi[0][1]=COVi[1][0]=fPrgCov[1][0];
    COVi[1][1]=fPrgCov[1][1];
    if(fVerbose) {std::cout<<" #$# Fast #$# COVi  "; COVi.Print();}
    
    //propagate to Vertex    
    TMatrixD Wi(COVi);
    Wi.Invert(&determinant);
    if (determinant == 0.) {
      std::cout<<"PndVtxPRG: COVi Inversion failed, abort fit -888"<<std::endl;
      return -888;
    }
    //TMatrixD Wi(TMatrixD::kInverted,COVi); // no determinant returned -> No check possible
    if(fDebug) {std::cout<<" #$# Fast #$# Wi"<<std::endl; Wi.Print();}
    TMatrixD Di(2,3); // Derivative in V, 2x3
    
    Di[0][0]=s;
    Di[0][1]=-c;
    Di[0][2]=0.;
    Di[1][0]=-t*c;
    Di[1][1]=-t*s;
    Di[1][2]=1.;
    if(fDebug) {std::cout<<" #$# Fast #$# Di  "; Di.Print();}
    
    TMatrixD wi(TMatrixD(Di,TMatrixD::kTransposeMult,Wi),TMatrixD::kMult,Di);
    w.push_back(wi); // save for later use
    if(fDebug) {std::cout<<" #$# Fast #$# wi  "; wi.Print();}
    TMatrixD wixpi(wi,TMatrixD::kMultTranspose,xpi);
    if(fDebug) {std::cout<<" #$# Fast #$# wixpi  "; wixpi.Print();}
    
    sumw+=wi; // sum up weights
    if(fDebug) {std::cout<<" #$# Fast #$# sumw  "; sumw.Print();}
    sumwx+=wixpi; // sum up weighted positions
    if(fDebug) {std::cout<<" #$# Fast #$# sumwx  "; sumwx.Print();}
  }
  
  TMatrixD cV(sumw); 
  cV.InvertFast(&determinant);
  if (determinant==0) {
    std::cout<<"PndVtxPRG: sumw Inversion failed, retunring -777"<<std::endl;
    return -777;
  }
  //TMatrixD cV(TMatrixD::kInverted,sumw); // no determinant returned -> No check possible
  if(fVerbose) {std::cout<<" #$# Fast #$# cV  "; cV.Print();}
  TMatrixD V(cV,TMatrixD::kMult,sumwx);// result vertex
  V.T(); // make it a row vector
  if(fVerbose) {std::cout<<" #$# Fast #$# V  "; V.Print();}
  vtx.SetXYZ(V[0][0],V[0][1],V[0][2]);
  vtx += fExpansionPoint; // move back to origin
  if(fVerbose) {std::cout<<" #$# Fast #$# Vtx: "; vtx.Print();}
  
  if (skipcov) return -1.; // skip chisqare calculation
  
  double chisq=0.; // calculate chisquare
  for(int i=0;i<nTrk;i++)
  {
    if(fDebug) {std::cout<<" #$# Fast #$# V  "; V.Print();}
    TMatrixD resid(xp[i],TMatrixD::kMinus,V);
    if(fDebug) {std::cout<<" #$# Fast #$# resid  "; resid.Print();}
    if(fDebug) {std::cout<<" #$# Fast #$# w["<<i<<"]  "; w[i].Print();}
    TMatrixD chisqi(TMatrixD(resid,TMatrixD::kMult,w[i]),TMatrixD::kMultTranspose,resid);
    if(fDebug) {std::cout<<" #$# Fast #$# chisqi  "; chisqi.Print();}
    chisq+=chisqi[0][0];
  }
  if(fVerbose) {std::cout<<" #$# Fast #$# chisq = "<<chisq<<std::endl;}
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




