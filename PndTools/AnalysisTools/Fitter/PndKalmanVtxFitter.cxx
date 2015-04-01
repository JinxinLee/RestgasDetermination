#include <iostream>
#include "PndKalmanVtxFitter.h"
#include "RhoBase/RhoCandListIterator.h"
#include "RhoBase/RhoFactory.h"
#include "TMatrixT.h"
#include "PndAnalysisCalcTools.h"

ClassImp(PndKalmanVtxFitter)

TBuffer& operator>>(TBuffer& buf, PndKalmanVtxFitter *&obj)
{
  obj = (PndKalmanVtxFitter*) buf.ReadObject(PndKalmanVtxFitter::Class());
  return buf;
}

PndKalmanVtxFitter::PndKalmanVtxFitter( RhoCandidate* b) :
  RhoFitterBase(b),
  fDebug(false),
  fNIterations(2),
  fExpansionPoint(0.,0.,0.),  // Assume Zero origin as default
  fPrgCov(5,5),
  fJacobian(5,7)
{
  fVerbose=false;
  fCurrentHead=fHeadOfTree;
}

PndKalmanVtxFitter::~PndKalmanVtxFitter()
{
}


Bool_t PndKalmanVtxFitter::CalcPrgParams(RhoCandidate* tcand, TVector3 expansionpoint)
{
  // calculate helix and its covariance in the preigee representation

  TLorentzVector mom = tcand->P4();
  TVector3 pos= tcand->GetPosition();
  Double_t charge = tcand->GetCharge();
  TMatrixD cov77 = tcand->Cov7();
  Bool_t test = PndAnalysisCalcTools::P7toPRG(pos, mom, charge, cov77, expansionpoint, fPrgParams, fPrgCov, fJacobian, false);
  return test;
}


Bool_t PndKalmanVtxFitter::FitNode(RhoCandidate* b)
{
  fCurrentHead=b;
  Bool_t check = Calculate();
  if(check == kFALSE) {
    fChiSquare=-22222;
    Error("PndKalmanVtxFitter::FitNode()","Fit failed for composite %p. Set chisquare to %f.",b,fChiSquare);
  }
  return check;
}


// Double_t PndKalmanVtxFitter::FitVertexFull(TVector3& vtx, TMatrixD& cov)
// {
//   // Calculate the Vertex in the full scheme and pass vertexing information directly
//   FitNode(fCurrentHead);
//   RhoCandidate* tcand = fCurrentHead->Daughter(0);
//   vtx = tcand->Pos();
//   TMatrixD cov7 = tcand->Cov7();
//   for(int i=0; i<3; i++)for(int j=0; j<3; j++) {
//       cov[i][j]=cov7[i][j];
//     }
//   return fChiSquare;
// }





Double_t PndKalmanVtxFitter::FitVertexFast(TVector3& vtx, TMatrixD& cov, bool skipcov, int niter)
{
  // Calculate a vertex of n tracks without considering the changes in momentum vector
  // the variables vtx & cov (3x3) are written and the Chi^2 is returned.
  // the current head candidate is set by FitNode during tree navigation or is the composite
  // which the fitter was initialized with.
  int nTrk = fCurrentHead->NDaughters();
  fNDegreesOfFreedom=2*nTrk-3;
  for (int iterate=0;iterate<niter;iterate++){
    fExpansionPoint=vtx;
    std::vector<TMatrixD> w;
    std::vector<TMatrixD> xp;
    TMatrixD sumw(3,3);
    TMatrixD sumwx(3,1);
    Double_t determinant = 0.;
    for(int i=0; i<nTrk; i++) {
      RhoCandidate* tcand = fCurrentHead->Daughter(i);
      // get Helix in PRG notation from Billoir paper
      // (epsilon,z0,theta,phi0,rho)
      Bool_t testprg = CalcPrgParams(tcand,fExpansionPoint);
      if(!testprg) { return -2; }

      if (fVerbose) {
        printf("#$# Fast #$# Helix params:\n\t epsilon\t = %.4g cm\n\t Z0\t = %.4g cm\n\t theta\t = %.4g\n\t phi0\t = %.4g\n\t rho\t = %.4g\n",fPrgParams[0],fPrgParams[1],fPrgParams[2],fPrgParams[3],fPrgParams[4]);
        std::cout<<" #$# Fast #$# Helix cov: ";
        fPrgCov.Print();
      }

      Double_t s = sin(fPrgParams[3]);
      Double_t c = cos(fPrgParams[3]);
      Double_t t = TMath::Tan(fPrgParams[2]);
      if(t!=0.) { t = 1/t; }
      else { return -3; }   // t = cot(theta)

        TMatrixD COVi(2,2); // track parameter cov for (epsilon,z0)
        COVi[0][0]=fPrgCov[0][0];
        COVi[0][1]=fPrgCov[0][1];
        COVi[1][0]=fPrgCov[1][0];
        COVi[1][1]=fPrgCov[1][1];
        if(fVerbose) {std::cout<<" #$# Fast #$# COVi  "; COVi.Print();}

        TMatrixD Di(2,3); // Derivative in V, 2x3
        Di[0][0]=fJacobian[0][0]; // dEpsilon  / dvx
        Di[0][1]=fJacobian[0][1]; // dEpsilon   /dvy
        Di[0][2]=fJacobian[0][2]; // dEpsilon   /dvz
        Di[1][0]=fJacobian[1][0]; // dZ0   /dvx
        Di[1][1]=fJacobian[1][1]; // dZ0   /dvy
        Di[1][2]=fJacobian[1][2]; // dZ0   /dvz
        if(fDebug) {std::cout<<" #$# Fast #$# Di  "; Di.Print();}

        TMatrixD xpi(1,3);
        xpi[0][0]=s*fPrgParams[0];
        xpi[0][1]=-c*fPrgParams[0];
        xpi[0][2]=fPrgParams[1];
        xp.push_back(xpi); // save for later use
        if(fVerbose) {std::cout<<" #$# Fast #$# xpi "; xpi.Print();}

        //propagate to Vertex
        TMatrixD Wi(COVi);
        Wi.InvertFast(&determinant);
        if (determinant == 0.) {
          std::cout<<"PndKalmanVtxFitter: COVi Inversion failed, abort fit -888"<<std::endl;
          return -888;
        }
        //TMatrixD Wi(TMatrixD::kInverted,COVi); // no determinant returned -> No check possible
        if(fDebug) {std::cout<<" #$# Fast #$# Wi"<<std::endl; Wi.Print();}
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
      std::cout<<"PndKalmanVtxFitter: sumw Inversion failed, retunring -777"<<std::endl;
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

    if (skipcov) {
      fChiSquare = -111.;// skip chisqare calculation
    } else {
      Double_t chisq=0.; // calculate chisquare
      for(int i=0; i<nTrk; i++) {
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
      fChiSquare=chisq;
    }
  }
  return fChiSquare;

}



Bool_t PndKalmanVtxFitter::Calculate()
{
  // We'll do a vertex finding and Fitting in empty space at a constant magnetic field along z-direction
  // We don't dive into the particle tree, it's just the daughters of this candidate

  int nTrk = fCurrentHead->NDaughters();
  fNDegreesOfFreedom=2*nTrk-3;

  TVector3 vtx(0.,0.,0.);
  std::vector<TVector3> momenta;
  TMatrixD tmpcov(3,3);
  Double_t chisquare=500000;//some high value
  Double_t determinant = 0.;
  TMatrixD qip(5,nTrk); // starting parameters of all tracks
  std::vector<TMatrixD> B;
  std::vector<TMatrixD> GI; // only Gi^-1 is used
  std::vector<TMatrixD> BGI; // helper to save on multiplications (more memory needed, though)
  std::vector<TMatrixD> D; // helper
  std::vector<TMatrixD> E; //helper
  TMatrixD A(3,3);
  TMatrixD T(3,1);
  std::vector<TMatrixD> U;
  std::vector<TMatrixD> W;
  std::vector<TMatrixD> dq; // helper
  TMatrixD CovFitFull(3+3*nTrk,3+3*nTrk); //3 vtx coordinates, nTrk*3 momenta
  TMatrixD CovVV(3,3);

  // Find a nice seed value
  bool verbi=fVerbose;
  fVerbose=false;
  bool debi=fDebug;
  fDebug=false; // temporary silence
  //FitVertexFast(vtx,tmpcov,false);//fast vertex seed (with expansion point used)
  fDebug=debi;
  fVerbose=verbi;
  if(fVerbose) {std::cout<<" #$# Fit #$# Vertex after fast prefit: "; vtx.Print();}
  //  fExpansionPoint = vtx; // move to vincinity
  for (int iteration = 0 ; iteration < fNIterations ; iteration++) {
    if(fVerbose) {
      std::cout<<"Iteration "<<iteration<<std::endl;
      std::cout<<" #$# Fit #$# Begin iteration "<<iteration<<" #$#$#$# "<<std::endl;
    }
    //cleanup
    B.clear();
    GI.clear(); // only Gi^-1 is used
    BGI.clear(); // helper to save on multiplications (more memory needed, though)
    D.clear(); // helper
    E.clear(); //helper
    U.clear();
    W.clear();
    dq.clear(); // helper
    A.Zero();
    T.Zero();
    CovFitFull.Zero();
    CovVV.Zero();

    for(int i=0; i<nTrk; i++) {
      if(fVerbose) {std::cout<<" #$# Fit #$# track "<<i<<" #$#$#$# "<<std::endl;}
      if(fVerbose) {std::cout<<" #$# Fit #$# Expansion point        "; fExpansionPoint.Print();}
      if(fVerbose) {std::cout<<" #$# Fit #$# Vertex before (global) "; (vtx).Print();}
      RhoCandidate* tcand = fCurrentHead->Daughter(i);

      const Double_t charge = tcand->GetCharge();

      if(iteration==0) {
        //On the first run fetch the starting values (being updated)
        //Bool_t testprg = CalcPrgParams(tcand,fExpansionPoint);
        Bool_t testprg = CalcPrgParams(tcand,vtx);
        if(!testprg) {printf("#$# Fit #$# CANNOT CALCULATE TRACK PARAMETERS (momenta at origin)"); return kFALSE;}
        qip[0][i]=0.;//fPrgParams[0];// the initial guess is AT the guess vertex. No epsilon_0
        qip[1][i]=0.;//fPrgParams[1];// the initial guess is AT the guess vertex. No z_0
        qip[2][i]=fPrgParams[2];//
        qip[3][i]=fPrgParams[3];//phi0
        qip[4][i]=fPrgParams[4];

        if(fDebug) {printf(" ##$# Fit #$# Measurement p: "); tcand->GetMomentum().Print();}
        if(fDebug) {printf(" ##$# Fit #$# Measurement x: "); tcand->GetPosition().Print();}
        momenta.push_back(tcand->GetMomentum());
        momenta[i].SetPhi(fPrgParams[3]); // set phi while keeping magnitude and theta constant
        if(fDebug) {printf(" ##$# Fit #$# Momentum at Expansionpoint (phi=%g): ",fPrgParams[3]); momenta[i].Print();}
      }

      // get parameters closest to current vertex estimate
      Bool_t testvtx = CalcPrgParams(tcand,vtx);
      if(!testvtx) { return kFALSE; }
      if (fVerbose) {
        printf("#$# Fit #$# Helix params:\n\t epsilon = %.4g cm\n\t Z0\t = %.4g cm\n\t theta\t = %.4g\n\t phi0\t = %.4g\n\t rho\t = %.4g 1/cm\n\t charge\t = %g e\n",fPrgParams[0],fPrgParams[1],fPrgParams[2],fPrgParams[3],fPrgParams[4],charge);
        std::cout<<" #$# Fit #$# Helix cov: ";
        fPrgCov.Print();
      }

      TMatrixD qiv(5,1); // parameters describing relative to expansion point the vertex seed
      qiv[0][0]=fPrgParams[0];//epsilon AT vertex zero
      qiv[1][0]=fPrgParams[1];//z0 AT vertex zero
      qiv[2][0]=fPrgParams[2];//
      qiv[3][0]=fPrgParams[3];//phi0
      qiv[4][0]=fPrgParams[4];//
      //dqi = q_measured - F_0 == div-qip;
      TMatrixD qipi(5,1);
      for (int kk=0; kk<5; kk++) { qipi[kk][0]=qip[kk][i]; }
      TMatrixD dqi(qiv,TMatrixD::kMinus,qipi);
      dq.push_back(dqi);
      if(fDebug) {std::cout<<" #$# Fit #$# dqi  "; dqi.Print();}

      TMatrixD COVi(5,5); // track parameter cov for (epsilon,z_p,Phi_p)
      //Double_t r=pocai.Perp();

      COVi=fPrgCov;
      if(fVerbose) {std::cout<<" #$# Fit #$# COVi  "; COVi.Print();}

      //Take the derivatives at the current vertex estimate (seed) (cf. Avery1 p.4)
      TMatrixD Di(5,3); // Derivative in V
      Di[0][0]=fJacobian[0][0]; // dEpsilon  / dvx
      Di[0][1]=fJacobian[0][1]; // dEpsilon   /dvy
      Di[0][2]=fJacobian[0][2]; // dEpsilon   /dvz
      Di[1][0]=fJacobian[1][0]; // dZ0   /dvx
      Di[1][1]=fJacobian[1][1]; // dZ0   /dvy
      Di[1][2]=fJacobian[1][2]; // dZ0   /dvz
      Di[2][0]=fJacobian[2][0]; //
      Di[2][1]=fJacobian[2][1]; //
      Di[2][2]=fJacobian[2][2]; //
      Di[3][0]=fJacobian[3][0]; // dPhi0 /dvx
      Di[3][1]=fJacobian[3][1]; // dPhi0 /dvy
      Di[3][2]=fJacobian[3][2]; // dPhi0 /dvz
      Di[4][0]=fJacobian[4][0]; //
      Di[4][1]=fJacobian[4][1]; //
      Di[4][2]=fJacobian[4][2]; //
      //Di.T();
      D.push_back(Di);
      if(fDebug) {std::cout<<" #$# Fit #$# Di  "; Di.Print();}

      TMatrixD Ei(5,3); // Derivative in p
      Ei[0][0]=fJacobian[0][3]; // dEpsilon   /dpx
      Ei[0][1]=fJacobian[0][4]; // dEpsilon   /dpy
      Ei[0][2]=fJacobian[0][5]; // dEpsilon   /dpz
      Ei[1][0]=fJacobian[1][3]; // dZ0   /dpx
      Ei[1][1]=fJacobian[1][4]; // dZ0   /dpy
      Ei[1][2]=fJacobian[1][5]; // dZ0   /dpz
      Ei[2][0]=fJacobian[2][3]; //
      Ei[2][1]=fJacobian[2][4]; //
      Ei[2][2]=fJacobian[2][5]; //
      Ei[3][0]=fJacobian[3][3]; // dPhi0 /dpx
      Ei[3][1]=fJacobian[3][4]; // dPhi0 /dpy
      Ei[3][2]=fJacobian[3][5]; // dPhi0 /dpz
      Ei[4][0]=fJacobian[4][3]; //
      Ei[4][1]=fJacobian[4][4]; //
      Ei[4][2]=fJacobian[4][5]; //
      //Ei.T();
      E.push_back(Ei);
      if(fDebug) {std::cout<<" #$# Fit #$# Ei  "; Ei.Print();}

      //det(aA) = a^n det(A)
      TMatrixD Wi(COVi);
      determinant=0.;
      Wi.InvertFast(&determinant);
      if (determinant==0) {
        std::cout<<"PndKalmanVtxFitter: COVi Inversion failed, abort fit."<<std::endl;
        return kFALSE;
      }
      W.push_back(Wi);
      if(fDebug) {
        std::cout<<" #$# Fit #$# Wi (det(cov) = "<<determinant<<")"<<std::endl;
        Wi.Print();
        for(int sdsd=0; sdsd<3; sdsd++) for(int asas=sdsd; asas<3; asas++) {
            printf("W{%i}[%i][%i] = %6.9f\n",i,sdsd,asas,Wi[sdsd][asas]);
            printf("W{%i}[%i][%i] = %6.9f\n",i,asas,sdsd,Wi[asas][sdsd]);
          }
      }

      // calculate matrices to be stored: A, Bi, Gi, T, Ui
      TMatrixD DitWi(Di,TMatrixD::kTransposeMult,Wi);
      if(fDebug) {std::cout<<" #$# Fit #$# DitWi  "; DitWi.Print();}

      TMatrixD EitWi(Ei,TMatrixD::kTransposeMult,Wi);
      if(fDebug) {std::cout<<" #$# Fit #$# EitWi  "; EitWi.Print();}

      TMatrixD Ai(DitWi,TMatrixD::kMult,Di);
      A += Ai;
      if(fDebug) {std::cout<<" #$# Fit #$# Ai ("<<i<<") = DitWiDi "; Ai.Print();}

      TMatrixD Bi(DitWi,TMatrixD::kMult,Ei);
      B.push_back(Bi);
      if(fDebug) {std::cout<<" #$# Fit #$# Bi = DitWiEi "; Bi.Print();}

      TMatrixD Gi(EitWi,TMatrixD::kMult,Ei);
      if(fDebug) {std::cout<<" #$# Fit #$# Gi = EitWiEi "; Gi.Print();}

      TMatrixD GIi(Gi);
      determinant=0.;
      GIi.InvertFast(&determinant);
      if (determinant==0) {
        std::cout<<"PndKalmanVtxFitter: GIi Inversion failed, abort fit."<<std::endl;
        return kFALSE;
      }
      GI.push_back(GIi);
      if(fDebug) {std::cout<<" #$# Fit #$# GIi = Gi^-1 (det="<<determinant<<")"; GIi.Print();}

      TMatrixD BiGIi(Bi,TMatrixD::kMult,GIi);
      BGI.push_back(BiGIi);
      if(fDebug) {std::cout<<" #$# Fit #$# BiGIi "; BiGIi.Print();}

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
    if(fDebug) {std::cout<<" #$# Fit #$# Vpre "; Vpre.Print();}
    for(int i=0; i<nTrk; i++) {
      TMatrixD BiGIi(BGI[i]);
      TMatrixD BiGIiUi(BiGIi,TMatrixD::kMult,U[i]);
      Vpre-=BiGIiUi;
      if(fDebug) {std::cout<<" #$# Fit #$# GIi ("<<i<<") "; GI[i].Print();}
      if(fDebug) {std::cout<<" #$# Fit #$# Bi ("<<i<<") "; B[i].Print();}
      if(fDebug) {std::cout<<" #$# Fit #$# BiGIi ("<<i<<") "; BiGIi.Print();}
      TMatrixD BiGIiBti(BiGIi,TMatrixD::kMultTranspose,B[i]);
      if(fDebug) {std::cout<<" #$# Fit #$# BiGIiBti ("<<i<<") = "; BiGIiBti.Print();}
      WV-=BiGIiBti;
      if(fDebug) {std::cout<<" #$# Fit #$# WV = "; WV.Print();}
      if(fDebug) {std::cout<<" #$# Fit #$# Vpre "; Vpre.Print();}
    }
    CovVV=WV;
    //CovVV*=1e9;//catch numerics
    CovVV.InvertFast(&determinant);
    //CovVV*=1e9;//catch numerics back
    if (determinant==0) {
      std::cout<<"PndKalmanVtxFitter: WV Inversion failed, abort fit."<<std::endl;
      return kFALSE;
    }
    if(fDebug) {std::cout<<" #$# Fit #$# WV= "; WV.Print();}
    if(fDebug) {std::cout<<" #$# Fit #$# CovVV  (det="<<determinant<<") ="; CovVV.Print();}
    //TMatrixD CovVV(TMatrixD::kInverted,WV); // no determinant returned -> No check possible
    TMatrixD uV(CovVV,TMatrixD::kMult,Vpre);
    vtx.SetXYZ(vtx.X()+uV[0][0],vtx.Y()+uV[1][0],vtx.Z()+uV[2][0]);
    if(fDebug) {std::cout<<" #$# Fit #$# Vertex update = "; uV.Print();}
    if(fVerbose) {std::cout<<" #$# Fit #$# Vertex after          "; vtx.Print();}
    if(fVerbose) {std::cout<<" #$# Fit #$# Vertex after (global) "; vtx.Print();}

//     //skip momentim update until the very last iteration
//     if(iteration < fNIterations-1){
//       fExpansionPoint=vtx; //
//       continue;
//     }

    // Calculate Momentum updates & Chi^2
    std::vector<TMatrixD> uq;
    //dV.T();
    double chiq=0;
    for(int i=0; i<nTrk; i++) {
      TMatrixD BtiV(B[i],TMatrixD::kTransposeMult,uV);
      TMatrixD uPi(GI[i],TMatrixD::kMult,U[i]-BtiV);
      // update momenta
      momenta[i]+=TVector3(uPi[0][0],uPi[1][0],uPi[2][0]);
      if(fVerbose) {std::cout<<" #$# Fit #$# Momenum update on particle %i"<<i; uPi.Print();}
      //qip[3][i]=momenta[i].Phi();
      // update track parameters at current vertex (phi_0)
      // track param residuals for chisquare
      TMatrixD uvi(D[i],TMatrixD::kMult,uV);
      TMatrixD upi(E[i],TMatrixD::kMult,uPi);
      dq[i]-=uvi;
      dq[i]-=upi;
      //chisquare
      TMatrixD dqitWi(dq[i],TMatrixD::kTransposeMult,W[i]);
      TMatrixD chis(dqitWi,TMatrixD::kMult,dq[i]);
      chiq+=chis[0][0];
	  if(fDebug){std::cout<<" #$# Fit #$# Insert Chisquare"<<std::endl;}
      InsertChi2(fCurrentHead->Daughter(i),chisquare);
    }
    if(chiq>0 && chiq<10000) { chisquare=chiq; }
    else { chisquare = -20; }
    // TODO: end iteration loop at satisfying chisq?
  }// end of iteratoin loop
  fChiSquare=chisquare;

  // calculating other cov matrices

  // POS COV
  for(int k=0; k<3; k++) for(int l=0; l<3; l++) {
      CovFitFull[k][l]=CovVV[k][l];
    }

  // MOM-POS COV
  for(int i=0; i<nTrk; i++) {
    TMatrixD CovVPi(CovVV,TMatrixD::kMult,BGI[i]);
    CovVPi*=-1.;
    for(int k=0; k<3; k++) for(int l=0; l<3; l++) {
        CovFitFull[k+3*(i+1)][l]=CovVPi[k][l]; ;
        CovFitFull[l][k+3*(i+1)]=CovVPi[k][l]; ;
      }
    // MOM-MOM COV
    for(int j=0; j<nTrk; j++) {
      // Caution: Identity of tracks i and j switch, to use CovVPi from above.
      TMatrixD CovPPijtmp(GI[j],TMatrixD::kMultTranspose,B[j]);
      TMatrixD CovPPij(CovPPijtmp,TMatrixD::kMult,CovVPi);
      CovPPij*=-1.;
      if(i==j) { CovPPij+=GI[j]; }
      for(int k=0; k<3; k++) for(int l=0; l<3; l++) {
          CovFitFull[k+3*(j+1)][l+3*(i+1)]=CovPPij[k][l]; // momentum cov
          CovFitFull[l+3*(i+1)][k+3*(j+1)]=CovPPij[k][l]; // momentum cov
        }
    }
  }
  if(fDebug) {std::cout<<" #$# Fit #$# CovFitFull: "; CovFitFull.Print();}


  //TODO... Continue checking

  // update particle candidates
  TMatrixD CovP7(7,7);
  for(int k=0; k<3; k++) for(int l=0; l<3; l++) {
      CovP7[k][l]=CovVV[k][l];
    }

  for(int i=0; i<nTrk; i++) {
    // The candidate tree is the to-be-fitted copy
    // It's safe to modify!
    RhoCandidate* tcand = fCurrentHead->Daughter(i);
    // set fitted values
    tcand->SetPos(vtx);
    if(fDebug) {std::cout<<" #$# Fit #$# Final Momentum :("<<i<<") "; momenta[i].Print();}
    tcand->SetP3(momenta[i]);

    for(int k=0; k<3; k++) for(int l=0; l<3; l++) {
        CovP7[k+3][l+3]=CovFitFull[3*(i+1)+k][3*(i+1)+l]; // momentum cov
        CovP7[k+3][l]=CovFitFull[3*(i+1)+k][l];   // momentum-position cov
        CovP7[l][k+3]=CovFitFull[l][3*(i+1)+k];   // momentum-position cov
      }
    // Calculate energy covariances:
    double invE = 1./tcand->E();
    CovP7[3][6] = CovP7[6][3] = (momenta[i].X()*CovP7[3][3]+momenta[i].Y()*CovP7[3][4]+momenta[i].Z()*CovP7[3][5])*invE;
    CovP7[4][6] = CovP7[6][4] = (momenta[i].X()*CovP7[4][3]+momenta[i].Y()*CovP7[4][4]+momenta[i].Z()*CovP7[4][5])*invE;
    CovP7[5][6] = CovP7[6][5] = (momenta[i].X()*CovP7[5][3]+momenta[i].Y()*CovP7[5][4]+momenta[i].Z()*CovP7[5][5])*invE;

    CovP7[6][6] = (momenta[i].X()*momenta[i].X()*CovP7[3][3]+momenta[i].Y()*momenta[i].Y()*CovP7[4][4]+momenta[i].Z()*momenta[i].Z()*CovP7[5][5]
                       +2.0*momenta[i].X()*momenta[i].Y()*CovP7[3][4]
                       +2.0*momenta[i].X()*momenta[i].Z()*CovP7[3][5]
                       +2.0*momenta[i].Y()*momenta[i].Z()*CovP7[4][5])*invE*invE;

    CovP7[6][0] = CovP7[0][6] = (momenta[i].X()*CovP7[3][0]+momenta[i].Y()*CovP7[4][0]+momenta[i].Z()*CovP7[5][0])*invE;
    CovP7[6][1] = CovP7[1][6] = (momenta[i].X()*CovP7[3][1]+momenta[i].Y()*CovP7[4][1]+momenta[i].Z()*CovP7[5][1])*invE;
    CovP7[6][2] = CovP7[2][6] = (momenta[i].X()*CovP7[3][2]+momenta[i].Y()*CovP7[4][2]+momenta[i].Z()*CovP7[5][2])*invE;

    tcand->SetCov7(CovP7);
    //tcand->Lock(); //?? autolocking
    //if(fDebug) {std::cout<<" #$# Fit #$# CovP7 with mom for traj "<<i<<": "; CovP7.Print();}
  }
  fCurrentHead->SetPos(vtx); // that's where the P4 is defined
  SetFourMomentumByDaughters(fCurrentHead);//Cov7 updated by daughters, too!
  SetDecayVertex(fCurrentHead,vtx,CovVV);
  return kTRUE; // all good now!
}

