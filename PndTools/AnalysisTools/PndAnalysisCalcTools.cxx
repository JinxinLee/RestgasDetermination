// PndAnalysisCalcToolsCalcTools
// Needs a FairRunAna set up in the macro for file & parameter I/O

#include "PndAnalysisCalcTools.h"

#include <string>
#include <iostream>

//Root stuff
#include "TTree.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include "RhoParticleSelectorBase.h"
#include "FairRecoCandidate.h"

//RHO stuff
//#include "TRho.h"
#include "RhoFactory.h"
#include "RhoCandidate.h"
#include "RhoCandList.h"

#include "FairTrackParP.h"
#include "FairTrackParH.h"
#include "FairGeanePro.h"

#include "PndTrack.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "PndPidListMaker.h"
#include "PndMCTrack.h"
#include "PndVtxFitterParticle.h"


Int_t PndAnalysisCalcTools::fVerbose=0;

PndAnalysisCalcTools::PndAnalysisCalcTools()
{
}

PndAnalysisCalcTools::~PndAnalysisCalcTools()
{
}

// Bool_t PndAnalysisCalcTools::FillHelixParams(RhoCandidate* cand, Bool_t skipcov)
// {
//   if (0==cand) {
//     Error("FillHelixParams():", "RhoCandidate pointer is zero.");
//     return kFALSE;
//   }
//   // write helix parameters & helix cov to RhoCandidate/TFitParams
//   Float_t helixparams[5];
//   TMatrixD helixcov(5,5);
//   Bool_t rc = P7toHelix(cand->Pos(), cand->P4(), cand->GetCharge(), cand->Cov7(), helixparams, helixcov, skipcov);
//   if (!rc) {Warning("FillHelixParams()","P7toHelix failed"); return kFALSE;}
// 
//   cand->SetHelixParms(helixparams);
// 
//   if (fVerbose>2) {
//     std::cout<<"calculated helix Params:"
//     <<"\n\tD0    ="<<helixparams[0]<<"\t cm"
//     <<"\n\tPhi0  ="<<helixparams[1]<<"\t rad"
//     <<"\n\tRho   ="<<helixparams[2]<<"\t 1/cm"
//     <<"\n\tZ0    ="<<helixparams[3]<<"\t cm"
//     <<"\n\tcotTh ="<<helixparams[4]<<"\t "
//     <<std::endl;
//   }
// 
//   if(!skipcov){
//     Float_t rhohelixcov[15];
//     rhohelixcov[0]  = helixcov[0][0];
//     rhohelixcov[1]  = helixcov[1][0];
//     rhohelixcov[2]  = helixcov[2][0];
//     rhohelixcov[3]  = helixcov[3][0];
//     rhohelixcov[4]  = helixcov[4][0];
//     rhohelixcov[5]  = helixcov[1][1];
//     rhohelixcov[6]  = helixcov[2][1];
//     rhohelixcov[7]  = helixcov[3][1];
//     rhohelixcov[8]  = helixcov[4][1];
//     rhohelixcov[9]  = helixcov[2][2];
//     rhohelixcov[10] = helixcov[3][2];
//     rhohelixcov[11] = helixcov[4][2];
//     rhohelixcov[12] = helixcov[3][3];
//     rhohelixcov[13] = helixcov[4][3];
//     rhohelixcov[14] = helixcov[4][4];
//     cand->SetHelixCov(rhohelixcov);
//   }
// 
// 
//   return kTRUE;
// }

Bool_t PndAnalysisCalcTools::P7toHelix(const TVector3& pos, const TLorentzVector& p4, const Double_t Q,
                                       const TMatrixD& cov77, Float_t* helixparams, TMatrixD& helixCov, Bool_t skipcov)
{
  // Convert from fourmomentum (vx,vy,vz,px,py,pz,e)
  // to RHO helix parameters (D0,Phi0,rho(omega),Z0,tan(dip))
  // Assuming vx,vy,vz give the POCA to the z axis.
  if(p4.Perp()< 1e-9) {Warning("P7toHelix","Too small transverse momentum: %g",p4.Perp()); return kFALSE;}
//  Double_t pnt[3], Bf[3];
//  pnt[0]=pos.X();
//  pnt[1]=pos.Y();
//  pnt[2]=pos.Z();
//  FairRunAna::Instance()->GetField()->GetFieldValue(pnt, Bf); //[kGs]
//  //Double_t B = sqrt(Bf[0]*Bf[0]+Bf[1]*Bf[1]+Bf[2]*Bf[2]);
//  Double_t B = Bf[2]; // assume field in z only
//  //Double_t B = 20.;

  Double_t B = GetBz(pos);
  if(fVerbose>1) { printf("P7ToHelix: BField is %g kGs\n",B); }
  Double_t qBc = -0.000299792458*B*Q;//Mind factor from momenta being in GeV
  //Double_t pti=1/p4.Perp();
  //Double_t dfi=(pos.Phi()-p4.Phi())*TMath::RadToDeg();
  //if(dfi>180)dfi-=360;
  //if(dfi<-180)dfi+=360;
  //Double_t sign=-Q*((dfi>0)?1:-1); // TODO get a decent D0 sign!!!
  //helixparams[0]=sign*pos.Perp(); //D0
  //helixparams[1]=p4.Phi(); //phi0
  //helixparams[2]=qBc*pti; //omega=rho=1/R[cm]=-2.998*B[kGs]*Q[e]/p_perp[GeV/c]
  //helixparams[3]=pos.Z(); //z0
  //helixparams[4]=p4.Pz()/p4.Perp(); //lambda(averey)=cot(theta)=tan(lambda(geane))
  if(fVerbose>1) { printf("P7ToHelix: Charge is %g e-\n",Q); }
  if(fVerbose>1) { printf("P7ToHelix: QBc is %g \n",qBc); }

  const double xp = pos.X();
  const double yp = pos.Y();
  const double zp = pos.Z();
  const double px = p4.Px();
  const double py = p4.Py();
  const double pz = p4.Pz();
  const double phip = TMath::ATan2(py,px);
  const double pti = 1/TMath::Sqrt(px*px+py*py);
  //const double phip = p4.Phi();
  //const double pti = 1/p4.Perp();
  if(fVerbose>1) { printf("P7ToHelix: P_t is %g GeV/c\n",p4.Perp()); }
  if(fVerbose>1) { printf("P7ToHelix: P_t^-1 is %g c/GeV\n",pti); }

  // get rho
  const double rho = qBc*pti;
  if(fVerbose>1) { printf("P7ToHelix: rho is %g cm^-1\n",rho); }

  // get tan(dip)
  const double tanDip=pz*pti;
  const double R0 = 1./rho;
  if(fVerbose>1) { printf("P7ToHelix: tanDip is %g \n",tanDip); }
  if(fVerbose>1) { printf("P7ToHelix: R0 = rho^-1 is %g cm\n",R0); }

  //circle center
  const double xc = xp - py/qBc;
  const double yc = yp + px/qBc;
  //const double xc = xp - R0*p4.Py()*pti;
  //const double yc = yp + R0*p4.Px()*pti;
  //const double xc = xp - R0*TMath::Sin(phip);
  //const double yc = yp + R0*TMath::Cos(phip);
  const double RCsq = xc*xc+yc*yc;
  const double RC = TMath::Sqrt(xc*xc+yc*yc);

  //get phi0 at doca
  const double phi0 = -TMath::ATan2(xc,yc);
  if(fVerbose>1) { printf("P7ToHelix: phi0 is %g, phiP is %g, DeltaPhi = %g \n",phi0,phip,phip-phi0); }

  //get D0
  const double D0 = RC - TMath::Abs(R0);
  //const double x0 = D0*TMath::Cos(phi0);
  //const double y0 = D0*TMath::Sin(phi0);
  if(fVerbose>1) { printf("P7ToHelix: D0 is %g cm\n",D0); }

  //get z0
  const double z0 = zp - pz*(phip-phi0)/qBc;
  //const double z0 = zp - tanDip*R0*(phip-phi0);
  if(fVerbose>1) { printf("P7ToHelix: z0 is %g cm\n",z0); }

  helixparams[0]=D0;
  helixparams[1]=phi0;
  helixparams[2]=rho;
  helixparams[3]=z0;
  helixparams[4]=tanDip; // == lambda
  if(fVerbose>1) {
    for(int ai=0; ai<5; ai++) {
      std::cout<<"helixparams["<<ai<<"]="<<helixparams[ai]<<std::endl;
    }
  }

  if(!skipcov) {
    TMatrixD jacobian(5,7);

    jacobian[0][0] = xc/RC; // dD0  / dvx
    jacobian[0][1] = yc/RC; // dD0   /dvy
    jacobian[0][2] = 0.; // dD0   /dvz
    jacobian[0][3] = yc/(qBc*RC)-px*pti/fabs(qBc); // dD0   /dpx
    jacobian[0][4] = -1.*xc/(qBc*RC)-py*pti/fabs(qBc); // dD0   /dpy
    jacobian[0][5] = 0.; // dD0   /dpz
    jacobian[0][6] = 0.; // dD0   /de

    jacobian[1][0] = yc/(RCsq); // dPhi0 /dvx
    jacobian[1][1] = -1.*xc/(RCsq); // dPhi0 /dvy
    jacobian[1][2] = 0.; // dPhi0 /dvz
    jacobian[1][3] = -1.*xc/(RCsq*qBc); // dPhi0 /dpx
    jacobian[1][4] = -1.*yc/(RCsq*qBc); // dPhi0 /dpy
    jacobian[1][5] = 0.; // dPhi0 /dpz
    jacobian[1][6] = 0.; // dPhi0 /de

    jacobian[2][0] = 0.; // drho  /dvx
    jacobian[2][1] = 0.; // drho  /dvy
    jacobian[2][2] = 0.; // drho  /dvz
    jacobian[2][3] = -1.*rho*px*pti*pti; // drho  /dpx
    jacobian[2][4] = -1.*rho*py*pti*pti; // drho  /dpy
    jacobian[2][5] = 0.; // drho  /dpz
    jacobian[2][6] = 0.; // drho  /de

    jacobian[3][0] = pz*yc/(RCsq*qBc); // dZ0   /dvx
    jacobian[3][1] = -1.*pz*xc/(RCsq*qBc); // dZ0   /dvy
    jacobian[3][2] = 1.; // dZ0   /dvz
    jacobian[3][3] = pz/qBc * (py*pti*pti + xc/(qBc*RCsq)); // dZ0   /dpx
    jacobian[3][4] = -1.*pz/qBc * (px*pti*pti - yc/(qBc*RCsq)); // dZ0   /dpy
    jacobian[3][5] = (phi0-phip)/qBc; // dZ0   /dpz
    jacobian[3][6] = 0.; // dZ0   /de

    jacobian[4][0] = 0.; // dtLam /dvx
    jacobian[4][1] = 0.; // dtLam /dvy
    jacobian[4][2] = 0.; // dtLam /dvz
    jacobian[4][3] = -1.*tanDip*px*pti*pti; // dtLam /dpx
    jacobian[4][4] = -1.*tanDip*py*pti*pti; // dtLam /dpy
    jacobian[4][5] = pti; // dtLam /dpz
    jacobian[4][6] = 0.; // dtLam /de

    TMatrixD tempmat(jacobian,TMatrixD::kMult,cov77);
    TMatrixD covrho(tempmat,TMatrixD::kMultTranspose,jacobian);
    helixCov=covrho;

    if (fVerbose>2) {
      std::cout<<"cov77: ";
      cov77.Print();
      //std::cout<<"sigmas: "; sigmas.Print();
      std::cout<<"jacobian: ";
      jacobian.Print();
      std::cout<<"covrho (D0,Phi0,rho,Z0,tanDip): ";
      covrho.Print();
      if(fVerbose>1) {
        std::cout<<"helixparams[0] = D0 \t= ("<<helixparams[0]<<" \t+- "<<sqrt(helixCov[0][0])<<") cm"<<std::endl;
        std::cout<<"helixparams[1] = Phi0 \t= ("<<helixparams[1]<<" \t+- "<<sqrt(helixCov[1][1])<<") rad"<<std::endl;
        std::cout<<"helixparams[2] = rho \t= ("<<helixparams[2]<<" \t+- "<<sqrt(helixCov[2][2])<<") 1/cm"<<std::endl;
        std::cout<<"helixparams[3] = Z0 \t= ("<<helixparams[3]<<" \t+- "<<sqrt(helixCov[3][3])<<") cm"<<std::endl;
        std::cout<<"helixparams[4] = tanDip\t= ("<<helixparams[4]<<" \t+- "<<sqrt(helixCov[4][4])<<")"<<std::endl;
      }
    }
  } // skip cov or not
  return kTRUE;
}



Bool_t PndAnalysisCalcTools::P7toPRG(const TVector3& pos, const TLorentzVector& p4, const Double_t Q, const TMatrixD& cov77,
                                     const TVector3& expPoint, Float_t* helixparams, TMatrixD& helixCov, TMatrixD& jacobian, Bool_t skipcov)
{
  // Convert from fourmomentum (vx,vy,vz,px,py,pz,e)
  // to PRG helix parameters (epsilon,z0,theta,phi0,rho)
  // These parameters should NOT be written into the RhoCandidate!

  // check for enough transverse momentum
  const double pt = p4.Perp(); // transverse momentum
  if(pt < 1e-9) {
    Warning("P7toPRG","Too small transverse momentum: %g",pt);
    return kFALSE;
  }

  // get field
  const double B = GetBz(pos); // [kGs]
  const double qBc = -0.000299792458*B*Q; // [GeV/cm] using B/[kGs]
  //const double qBc = -0.299792458*B*Q; // momenta being in GeV length in cm
  if(fVerbose>1) { printf("P7toPRG: BField is %g kGs\n",B); }
  if(fVerbose>1) { printf("P7toPRG: Charge is %g e-\n",Q); }
  if(fVerbose>1) { printf("P7toPRG: QBc is %g \n",qBc); }

  // move system to expansion point
  const double xp = pos.X()-expPoint.X(); // reconstructed point
  const double yp = pos.Y()-expPoint.Y(); // reconstructed point
  const double zp = pos.Z()-expPoint.Z(); // reconstructed point
  const double px = p4.Px(); // reconstructed momentum
  const double py = p4.Py(); // reconstructed momentum
  const double pz = p4.Pz(); // reconstructed momentum
  const double p2 = px*px+py*py+pz*pz;
  const double p2i = (p2==0.)?0.:1./p2;
  if(fVerbose>1) { printf("P7toPRG: x is [%8g,%8g,%8g] cm \n",xp,yp,zp); }
  if(fVerbose>1) { printf("P7toPRG: p is [%8g,%8g,%8g] GeV/c \n",px,py,pz); }
  // phi_p
  const double phip = p4.Phi();
  if(fVerbose>1) { printf("P7toPRG: phi is %g \n",phip); }

  // get rho & R0
  const double pti = 1/pt;
  const double rho = qBc*pti;
  const double R0 = 1./rho; // signed radius
  if(fVerbose>1) { printf("P7toPRG: rho is %g cm^-1\n",rho); }
  if(fVerbose>1) { printf("P7toPRG: P_t is %g GeV/c\n",pt); }
  if(fVerbose>1) { printf("P7toPRG: P_t^-1 is %g c/GeV\n",pti); }
  if(fVerbose>1) { printf("P7toPRG: R0 = rho^-1 is %g cm\n",R0); }

  // get theta
  const double theta=p4.Theta();
  if(fVerbose>1) { printf("P7toPRG: theta is %g \n",theta); }
  //const double tanDip = TMath::Tan(0.5*TMath::Pi() - theta); //unused?

  //circle center in x-y projection
  const double xc = xp - py/qBc;
  const double yc = yp + px/qBc;
  const double RCsq = xc*xc + yc*yc;
  const double RC = TMath::Sqrt(RCsq);

  //get epsilon
  //epsilon is the distance of closest approach with its sign defined as being positive
  //if the origin lays at the lefthand side of the moving particle
  const double epsilon = Q*(RC - TMath::Abs(R0));
  if(fVerbose>1) { printf("P7toPRG: epsilon is %g cm\n",epsilon); }

  //get phi0 at doca
  const double direction = TMath::Sign(1.,Q);
  double phi0 = TMath::ATan2(yc,xc);
  if (yc < 0) { phi0 += TMath::Pi(); }
  else { phi0 -= TMath::Pi(); }
  phi0 -= direction*0.5*TMath::Pi();
  //get phi0 into [-pi,pi]
  if (phi0 < -TMath::Pi()) { phi0 += TMath::TwoPi(); }
  else if (phi0 > TMath::Pi()) { phi0 -= TMath::TwoPi(); }

  if(fVerbose>1) { printf("P7toPRG: phi0 = %.4g, \tphiP = %.4g, \tDeltaPhi = %.4g \tepsilon=%.4g, \tQ=%g\n",phi0,phip,phip-phi0, epsilon, Q); }
  if(fVerbose>1) {
    double xnew=epsilon*sin(phi0);
    double ynew=-epsilon*cos(phi0);
    printf("P7toPRG: xnew = %.4g, \tynew = %.4g, \t(x^2+y^2) = %.4g \tepsilon^2=%.4g\n",xnew,ynew,xnew*xnew+ynew*ynew, epsilon*epsilon);
  }

  //get z0
  const double z0 = zp - pz*(phip-phi0)/qBc;
  //const double z0 = zp - tanDip*R0*(phip-phi0);
  if(fVerbose>1) { printf("P7toPRG: z0 is %g cm from zp=%.3g cm\n",z0,zp); }

  helixparams[0]=epsilon;
  helixparams[1]=z0;
  helixparams[2]=theta;
  helixparams[3]=phi0;
  helixparams[4]=rho;
  if(fVerbose>0 && skipcov) {
    for(int ai=0; ai<5; ai++) {
      std::cout<<"helixparams["<<ai<<"]="<<helixparams[ai]<<std::endl;
    }
  }

  if(!skipcov) {
    //TMatrixD jacobian(5,7);

    jacobian[0][0] = Q*xc/RC; // dEpsilon  / dvx
    jacobian[0][1] = Q*yc/RC; // dEpsilon   /dvy
    jacobian[0][2] = 0.; // dEpsilon   /dvz
    jacobian[0][3] = Q*(yc/(qBc*RC)-TMath::Sign(pti,R0)*px/qBc); // dEpsilon   /dpx
    jacobian[0][4] = Q*(-xc/(qBc*RC)-TMath::Sign(pti,R0)*py/qBc); // dEpsilon   /dpy
    jacobian[0][5] = 0.; // dEpsilon   /dpz
    jacobian[0][6] = 0.; // dEpsilon   /de

    jacobian[1][0] = -pz*yc/(RCsq*qBc); // dZ0   /dvx
    jacobian[1][1] = pz*xc/(RCsq*qBc); // dZ0   /dvy
    jacobian[1][2] = 1.; // dZ0   /dvz
    jacobian[1][3] = (py*pti*pti + xc/(qBc*RCsq))*pz/qBc; // dZ0   /dpx
    jacobian[1][4] = -(px*pti*pti - yc/(qBc*RCsq))*pz/qBc; // dZ0   /dpy
    jacobian[1][5] = -(phip-phi0)/qBc; // dZ0   /dpz
    jacobian[1][6] = 0.; // dZ0   /de
    // ok

    jacobian[2][0] = 0.; // dTheta /dvx
    jacobian[2][1] = 0.; // dTheta /dvy
    jacobian[2][2] = 0.; // dTheta /dvz
    jacobian[2][3] = px*pz*pti*p2i; // dTheta /dpx
    jacobian[2][4] = py*pz*pti*p2i; // dTheta /dpy
    jacobian[2][5] = -pt*p2i; // dTheta /dpz
    jacobian[2][6] = 0.; // dTheta /de
    //ok

    jacobian[3][0] = -yc/(RCsq); // dPhi0 /dvx
    jacobian[3][1] = xc/(RCsq); // dPhi0 /dvy
    jacobian[3][2] = 0.; // dPhi0 /dvz
    jacobian[3][3] = xc/(RCsq*qBc); // dPhi0 /dpx
    jacobian[3][4] = yc/(RCsq*qBc); // dPhi0 /dpy
    jacobian[3][5] = 0.; // dPhi0 /dpz
    jacobian[3][6] = 0.; // dPhi0 /de

    jacobian[4][0] = 0.; // drho  /dvx
    jacobian[4][1] = 0.; // drho  /dvy
    jacobian[4][2] = 0.; // drho  /dvz
    jacobian[4][3] = -rho*px*pti*pti; // drho  /dpx
    jacobian[4][4] = -rho*py*pti*pti; // drho  /dpy
    jacobian[4][5] = 0.; // drho  /dpz
    jacobian[4][6] = 0.; // drho  /de
    //ok

    TMatrixD tempmat(jacobian,TMatrixD::kMult,cov77);
    TMatrixD covrho(tempmat,TMatrixD::kMultTranspose,jacobian);
    helixCov=covrho;

    if (fVerbose>2) {
      std::cout<<"cov77: ";
      cov77.Print();
      //std::cout<<"sigmas: "; sigmas.Print();
      std::cout<<"jacobian: ";
      jacobian.Print();
    }
    if (fVerbose>0) {
      std::cout<<"covrho (epsilon,Z0,Theta,Phi0,rho): ";
      covrho.Print();
      std::cout<<"helixparams[0] = epsilon\t= ("<<helixparams[0]<<" \t+- "<<sqrt(helixCov[0][0])<<") cm"<<std::endl;
      std::cout<<"helixparams[1] = Z0 \t= ("<<helixparams[1]<<" \t+- "<<sqrt(helixCov[1][1])<<") cm"<<std::endl;
      std::cout<<"helixparams[2] = Theta \t= ("<<helixparams[2]<<" \t+- "<<sqrt(helixCov[2][2])<<") rad"<<std::endl;
      std::cout<<"helixparams[3] = Phi0 \t= ("<<helixparams[3]<<" \t+- "<<sqrt(helixCov[3][3])<<") rad"<<std::endl;
      std::cout<<"helixparams[4] = rho\t= ("<<helixparams[4]<<" \t+- "<<sqrt(helixCov[4][4])<<") 1/cm"<<std::endl;
    }
  } // skip cov or not
  return kTRUE;
}


// Bool_t PndAnalysisCalcTools::SDtoHelix(FairTrackParH* par, RhoCandidate* cand, Bool_t skipcov)
// {
//   // Adding the helix parameters to the candidate (TFitParams)
//   // It is nice to know at analysis stage
//   // rho helix: (D0,Phi0,rho(omega),Z0,tan(dip))
//   // fair helix: (q/p,lambda, phi, y_perp, z_perp)
//   Double_t Q=par->GetQ();
//   if(0==Q) return kFALSE;
// //  Double_t pnt[3], Bf[3];
// //  pnt[0]=par->GetX();
// //  pnt[1]=par->GetY();
// //  pnt[2]=par->GetZ();
// //  FairRunAna::Instance()->GetField()->GetFieldValue(pnt, Bf); //[kGs]
// //  //Double_t B = sqrt(Bf[0]*Bf[0]+Bf[1]*Bf[1]+Bf[2]*Bf[2]);
// //  Double_t B = Bf[2];
// 
//   Double_t B = GetBz(par->GetPosition());
// 
//   Double_t qBc = -0.000299792458*B*Q;
//   Double_t icL = 1. / cos(par->GetLambda()); // inverted for practical reasons (better to multiply than to divide)
//   Double_t icLs = icL*icL;
//   Float_t helixparams[5];
//   helixparams[0]=par->GetY_sc() ; //D0
//   helixparams[1]=par->GetMomentum().Phi()+TMath::Pi(); //phi0
//   helixparams[2]=qBc/(par->GetMomentum().Perp()); //omega=rho=1/R[cm]=-2.998e-4*B[kGs]*Q[e]/p_perp[GeV/c]
//   helixparams[3]=par->GetZ_sc()*icL; //z0
//   helixparams[4]=tan(par->GetLambda()); //lambda(averey)=cot(theta)=tan(lambda(geane))
//   cand->SetHelixParms(helixparams);
// 
//   if(skipcov) return kTRUE; // stop here when skipping cov matrix calculation
// 
//   Double_t fairhelixcov[15];
//   par->GetCov(fairhelixcov);
// 
//   // in the poca to z axis yperp=D0, x_perp^2+z_perp^2 = z_perp/cos(Lambda)= Z0
//   Float_t rhohelixcov[15];
//   rhohelixcov[0]  = fairhelixcov[12];                    // sigma^2 D0
//   rhohelixcov[1]  = fairhelixcov[10];                    // cov D0 - Phi0
//   rhohelixcov[2]  = fairhelixcov[3]  * qBc * icL;        // cov D0 - rho
//   rhohelixcov[3]  = fairhelixcov[13] * icL;              // cov D0 - Z0
//   rhohelixcov[4]  = fairhelixcov[7]  * icLs;             // cov D0 - tan(dip)
//   rhohelixcov[5]  = fairhelixcov[9];                     // sigma^2 Phi0
//   rhohelixcov[6]  = fairhelixcov[2]  * qBc * icL;        // cov Phi0 - rho
//   rhohelixcov[7]  = fairhelixcov[11] * icL;              // cov Phi0 - Z0
//   rhohelixcov[8]  = fairhelixcov[6]  * icLs;             // cov Phi0 - tan(dip)
//   rhohelixcov[9]  = fairhelixcov[0]  * qBc * qBc * icLs; // sigma^2 rho
//   rhohelixcov[10] = fairhelixcov[4]  * qBc * icLs;       // cov rho - Z0
//   rhohelixcov[11] = fairhelixcov[1]  * qBc * icL * icLs; // cov rho - tan(dip)
//   rhohelixcov[12] = fairhelixcov[14] * icLs;             // sigma^2 Z0
//   rhohelixcov[13] = fairhelixcov[8]  * icL * icLs;       //cov Z0 - tan(dip)
//   rhohelixcov[14] = fairhelixcov[5]  * icLs * icLs;      // sigma^2 tan(dip) - from
//   cand->SetHelixCov(rhohelixcov);
// 
//   return kTRUE;
// }


