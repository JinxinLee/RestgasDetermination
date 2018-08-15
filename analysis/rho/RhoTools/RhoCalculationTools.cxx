//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoCalculationTools                                                  //
//                                                                      //
// Toolset to calculate things centrally.                               //
//                                                                      //
// Authors:                                                             //
// Ralf Kliemt, HIM/GSI Feb.2013                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoCalculationTools.h"
#include "FairRun.h"
#include "FairRunSim.h"
#include "FairRunAna.h"
#include "FairField.h"

Double_t RhoCalculationTools::fBz=0.;
Int_t RhoCalculationTools::fVerbose=0;
Bool_t RhoCalculationTools::fBzSet=kFALSE;

Double_t RhoCalculationTools::GetBz ( const TVector3& pos )
{
  // If field was forced, we return that.
  if ( kTRUE==fBzSet ) { return fBz; }
  // Read magnetic filed strength in z direction [kGs]
  double pnt[3], Bf[3];
  pnt[0]=pos.X();
  pnt[1]=pos.Y();
  pnt[2]=pos.Z();
  // retrieve the field from the framework
  if( FairRun::Instance()->IsAna() )
  {
	FairRunAna::Instance()->GetField()->GetFieldValue ( pnt, Bf ); //[kGs]
  } else {
	FairRunSim::Instance()->GetField()->GetFieldValue ( pnt, Bf ); //[kGs]
  }

  return Bf[2];
}






/*
Convert the Cov matrices from x,y,z,px,py,pz ==> px,py,pz,x,y,z
or vice versa*/
TMatrixD RhoCalculationTools::GetConverted6(TMatrixD tmpMat)
{
  TMatrixD cMat(6,6);
  for(Int_t i=0; i<6; i++) {
    for(Int_t j=0; j<6; j++) {
      if(i>=3) {
        if(j>=3) { cMat[i-3][j-3] = tmpMat[i][j]; }
        else { cMat[i-3][j+3] = tmpMat[i][j]; }
      } else {
        if(j>=3) { cMat[i+3][j-3] = tmpMat[i][j]; }
        else { cMat[i+3][j+3] = tmpMat[i][j]; }
      }
    }
  }
  return cMat;
}
/*
Convert the Cov matrices from x,y,z,px,py,pz ==> px,py,pz,x,y,z
or vice versa*/
TMatrixDSym RhoCalculationTools::GetConverted6(TMatrixDSym tmpMat)
{
  TMatrixDSym cMat(6);
  for(Int_t i=0; i<6; i++) {
    for(Int_t j=0; j<6; j++) {
      if(i>=3) {
        if(j>=3) { cMat[i-3][j-3] = tmpMat[i][j]; }
        else { cMat[i-3][j+3] = tmpMat[i][j]; }
      } else {
        if(j>=3) { cMat[i+3][j-3] = tmpMat[i][j]; }
        else { cMat[i+3][j+3] = tmpMat[i][j]; }
      }
    }
  }
  return cMat;
}
//convert cov matrices x,y,z,px,py,pz,E (7x7) ==> px,py,pz,E,x,y,z (7x7)
TMatrixDSym RhoCalculationTools::GetCovMat1(TMatrixD tmpMat)
{
  TMatrixDSym cMat(7);
  for(Int_t i=0; i<7; i++) {
    for(Int_t j=0; j<7; j++) {
      if(i>=3) {
        if(j>=3) { cMat[i-3][j-3] = tmpMat[i][j]; }
        else { cMat[i-3][j+3] = tmpMat[i][j]; }
      } else {
        if(j>=3) { cMat[i+4][j-3] = tmpMat[i][j]; }
        else { cMat[i+4][j+4] = tmpMat[i][j]; }
      }
    }
  }
  return cMat;
}

// Convert the Cov matrices from px,py,pz,E,x,y,z ==> x,y,z,px,py,pz,E
TMatrixD RhoCalculationTools::GetConverted7(TMatrixD tmpMat)
{
  TMatrixD cMat(7,7);
  for(Int_t i=0; i<7; i++) {
    for(Int_t j=0; j<7; j++) {
      if(i>=4) {
        if(j>=4) { cMat[i-4][j-4] = tmpMat[i][j]; }
        else { cMat[i-4][j+3] = tmpMat[i][j]; }
      } else {
        if(j>=4) { cMat[i+3][j-4] = tmpMat[i][j]; }
        else { cMat[i+3][j+3] = tmpMat[i][j]; }
      }
    }
  }
  return cMat;
}

// Convert x,y,z,px,py,pz,E (7x7) ==> px,py,pz,x,y,z (6x6)
TMatrixDSym RhoCalculationTools::GetCovMat(TMatrixD tmpMat)
{
  TMatrixDSym cMat(6);
  for(Int_t i=0; i<6; i++) {
    for(Int_t j=0; j<6; j++) {
      if(i>=3) {
        if(j>=3) { cMat[i-3][j-3] = tmpMat[i][j]; }
        else { cMat[i-3][j+3] = tmpMat[i][j]; }
      } else {
        if(j>=3) { cMat[i+3][j-3] = tmpMat[i][j]; }
        else { cMat[i+3][j+3] = tmpMat[i][j]; }
      }
    }
  }
  return cMat;
}

//converting 7x7 cov matrix to 6x6 matrix
TMatrixDSym RhoCalculationTools::GetFitError(TMatrixDSym e) 
{
  TMatrixDSym err(6);
  for(unsigned i=0; i<3; ++i) {
    for(unsigned j=i; j<3; ++j) {
      err[i][j] = err[j][i] = e[i][j];
      err[3+i][3+j] = err[3+j][3+i] = e[4+i][4+j];
    }
  }
  for(unsigned i=0; i<3; ++i) {
    for(unsigned j=0; j<3; ++j) {
      err[i][3+j] = err[3+j][i]  = e[i][4+j];
    }
  }
  return err;
}

// converting 6x6 cov matrices to 7x7
TMatrixD RhoCalculationTools::GetFitError(TLorentzVector p,TMatrixD e) 
{
//    Error(6x6,e) ==> Error(7x7,output(hsm)) using Momentum(p).
  TMatrixD hsm(7,7);
  for(unsigned i=0; i<3; ++i) {
    for(unsigned j=i; j<3; ++j) {
      hsm[i][j] = hsm[j][i] = e[i][j];
      hsm[4+i][4+j] = hsm[4+j][4+i] = e[3+i][3+j];
    }
  }
  for(unsigned i=0; i<3; ++i) {
    for(unsigned j=0; j<3; ++j) {
      hsm[i][4+j] = hsm[4+j][i] = e[i][3+j];
    }
  }
  double invE = 1./p.E();
  hsm[0][3] = hsm[3][0] = (p.X()*hsm[0][0]+p.Y()*hsm[0][1]+p.Z()*hsm[0][2])*invE;
  hsm[1][3] = hsm[3][1] = (p.X()*hsm[0][1]+p.Y()*hsm[1][1]+p.Z()*hsm[1][2])*invE;
  hsm[2][3] = hsm[3][2] = (p.X()*hsm[0][2]+p.Y()*hsm[1][2]+p.Z()*hsm[2][2])*invE;
  hsm[3][3] = (p.X()*p.X()*hsm[0][0]+p.Y()*p.Y()*hsm[1][1]+p.Z()*p.Z()*hsm[2][2]
               +2.0*p.X()*p.Y()*hsm[0][1]
               +2.0*p.X()*p.Z()*hsm[0][2]
               +2.0*p.Y()*p.Z()*hsm[1][2])*invE*invE;
  hsm[3][4] = hsm[4][3] = (p.X()*hsm[0][4]+p.Y()*hsm[1][4]+p.Z()*hsm[2][4])*invE;
  hsm[3][5] = hsm[5][3] = (p.X()*hsm[0][5]+p.Y()*hsm[1][5]+p.Z()*hsm[2][5])*invE;
  hsm[3][6] = hsm[6][3] = (p.X()*hsm[0][6]+p.Y()*hsm[1][6]+p.Z()*hsm[2][6])*invE;
  return hsm;
}




// Bool_t RhoCalculationTools::FillHelixParams(RhoCandidate* cand, Bool_t skipcov)
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

Bool_t RhoCalculationTools::P7toHelix(const TVector3& pos, const TLorentzVector& p4, const Double_t Q,
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

Bool_t RhoCalculationTools::P7toPRG(const TVector3& pos, const TLorentzVector& p4, const Double_t Q, const TMatrixD& cov77,
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
  const double pti = 1/pt;

  if(fVerbose>1) { printf("P7toPRG: x is [%8g,%8g,%8g] cm \n",xp,yp,zp); }
  if(fVerbose>1) { printf("P7toPRG: p is [%8g,%8g,%8g] GeV/c \n",px,py,pz); }
  // phi_p
  const double phip = p4.Phi();
  if(fVerbose>1) { printf("P7toPRG: phi is %g \n",phip); }



  //check if daughter is charged or neutral
  if(fabs(Q)<1e-6){

    /*
     * edited by J. Puetz
     * reference for the helixparameters for neutral particles
     * "Vertex reconstruction by means of the method of Kalman filtering" from Rolf Luchsinger and Christoph Grab
     * doi:10.1016/0010-4655(93)90055-H
     */

    double r = TMath::Sqrt(xp*xp+yp*yp);
    double eta = phip - TMath::ACos(xp/r);
    double tht = p4.Theta();

    helixparams[0] = r*sin(eta);
    helixparams[1] = zp - r*cos(eta)/tan(tht);
    helixparams[2] = tht;
    helixparams[3] = phip;
    helixparams[4] = TMath::Sqrt(p2);


    if(!skipcov){

      jacobian[0][0] = xp*sin(eta)/r + TMath::Sqrt(1-xp*xp/(r*r)) * cos(eta);     //d(r*sin(eta))/dxp
      jacobian[0][1] = yp*sin(eta)/r - xp*yp*cos(eta)/(r*r*TMath::Sqrt(1-xp*xp/(r*r))); //d(r*sin(eta))/dyp
      jacobian[0][2] = 0.;                                //d(r*sin(eta))/dzp
      jacobian[0][3] = 0.;                                //d(r*sin(eta))/dpx
      jacobian[0][4] = 0.;                                //d(r*sin(eta))/dpy
      jacobian[0][5] = 0.;                                //d(r*sin(eta))/dpz
      jacobian[0][6] = 0.;                                //d(r*sin(eta))/dE

      jacobian[1][0] = TMath::Sqrt(1-xp*xp/(r*r))*sin(eta)/tan(tht) - xp/r * cos(eta)/tan(tht);       //d(helixparam[1])/dxp
      jacobian[1][1] = -(yp/r*cos(eta)/tan(tht)+xp*yp/(r*r)*sin(eta)/tan(tht)/TMath::Sqrt(1-xp*xp/(r*r)));  //d(helixparam[1])/dyp
      jacobian[1][2] = 1.;                                          //d(helixparam[1])/dzp
      jacobian[1][3] = 0.;                                          //d(helixparam[1])/dpx
      jacobian[1][4] = 0.;                                          //d(helixparam[1])/dpy
      jacobian[1][5] = 0.;                                          //d(helixparam[1])/dpz
      jacobian[1][6] = 0.;                                          //d(helixparam[1])/dE

      jacobian[2][0] = 0.;        // dTheta /dxp
      jacobian[2][1] = 0.;        // dTheta /dyp
      jacobian[2][2] = 0.;        // dTheta /dzp
      jacobian[2][3] = px*pz*pti*p2i;   // dTheta /dpx
      jacobian[2][4] = py*pz*pti*p2i;   // dTheta /dpy
      jacobian[2][5] = -pt*p2i;     // dTheta /dpz
      jacobian[2][6] = 0.;         // dTheta /dE

      jacobian[3][0] = 0.;        // dPhi /dxp
      jacobian[3][1] = 0.;        // dPhi /dyp
      jacobian[3][2] = 0.;        // dPhi /dzp
      jacobian[3][3] = -py*pti;     // dPhi /dpx
      jacobian[3][4] = px*pti;      // dPhi /dpy
      jacobian[3][5] = 0.;        // dPhi /dpz
      jacobian[3][6] = 0.;        // dPhi /dE

      jacobian[4][0] = 0.;          // dp /dxp
      jacobian[4][1] = 0.;          // dp /dyp
      jacobian[4][4] = 0.;          // dp /dzp
      jacobian[4][3] =px*TMath::Sqrt(p2i);  // dp /dpx
      jacobian[4][4] =py*TMath::Sqrt(p2i);; // dp /dpy
      jacobian[4][5] =pz*TMath::Sqrt(p2i);; // dp /dpz
      jacobian[4][6] = 0.;          // dp /dE
    }
    } //end neutral particle

    else{
    // get rho & R0

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
    }
    }//end charged particle

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
  return kTRUE;
}


// Bool_t RhoCalculationTools::SDtoHelix(FairTrackParH* par, RhoCandidate* cand, Bool_t skipcov)
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

// --------------------------------------------
Bool_t RhoCalculationTools::P6FromTrajectory( TVectorD& mom6, TMatrixDSym& cov6, RhoCandidate* cand, Double_t z, Double_t ztolerance=-1. ) 
{
  // here we transform from Pos&P4 of the RhoCandidate to DecayTreeFitter mom6
  TVector3 pos = cand->Pos();
  TLorentzVector mom = cand->P4(); 
  TMatrixD cov7 = cand->Cov7();
  if(ztolerance>0. && fabs(cand->Pos().Z()-z) > ztolerance) 
  {
    // input/output is (x0,y0,z0,px0,py0,pz0)
    // transporting on helix to (x1,y1,z,px1,py1,pz0) (z from input, pz stays)
    const double dz = z-pos.Z();
    const double px = mom.Px();
    const double py = mom.Py();
    const double pzi = 1./mom.Pz();
    //const double A = dz*pzi;
    const double B = GetBz(pos); // [kGs]
    const double a = -0.000299792458*B*cand->Charge(); // [GeV/cm] using B/[kGs]
    const double ai = 1./a;
    const double rho = a/mom.P();
    const double rhoTimesS = a*dz*pzi;
    const double L=sin(rhoTimesS);
    //const double K=cos(rhoTimesS);  
    const double K=sqrt(1-L*L);  
  
    TMatrixD J(7,7);
    for(int i=0;i<7;i++) J[i][i]=1.;
    //J[2][0] = pzi*(px*K-py*L); // dx/dz0
    //J[2][1] = pzi*(py*K+px*L); // dy/dz0
    //J[2][3] = pzi*a*(-px*L-py*K); // dpx/dz0
    //J[2][4] = pzi*a*(-py*L+px*K); // dpy/dz0
    J[0][3] = L*ai; // dx/dpx0
    J[1][3] = (1.-K)*ai; // dy/dpx0
    J[0][4] = (K-1.)*ai; // dx/dpy0
    J[1][4] = L*ai; // dy/dpy0
    J[3][3] = K; // dpx/dpx0
    J[4][3] = L; // dpy/dpx0
    J[3][4] = -L; // dpx/dpy0
    J[4][4] = K; // dpy/dpy0
    J[3][5] = rho*pzi*(px*L+py*K); // dpx/dpz0
    J[4][5] = rho*pzi*(py*L-px*K); // dpy/dpz0
  
    double x1 = pos.X() + (L*px-(1.-K)*py)*ai;
    double y1 = pos.Y() + (L*py+(1.-K)*px)*ai;
  
    double px1 = px*K-py*L;
    double py1 = py*K+px*L;
    TMatrixD Jcov(J,TMatrixD::kMult,cov7);
    TMatrixD newcov(Jcov,TMatrixD::kMultTranspose,J);

    pos.SetXYZ(x1,y1,z);
    mom.SetX(px1);
    mom.SetY(py1);
    cov7=newcov;
  }
  
  mom6[0] = pos.x();
  mom6[1] = pos.y();
  mom6[2] = pos.z();
  mom6[3] = mom.Px();
  mom6[4] = mom.Py();
  mom6[5] = mom.Pz();

  for(int i=0;i<6;i++){
    for(int j=0;j<6;j++){
      cov6[i][j]=cov7[i][j];
    }
  }
  return kTRUE;
};

Double_t RhoCalculationTools::StateFromTrajectory( TVectorD& state, TMatrixDSym& cov, RhoCandidate* cand, 
                                                  double , double , double vz,  double ztolerance=-1. )  // vx vy //[R.K.03/2017] unused variable(s)
{
  //std::cout<<"RhoCalculationTools::StateFromTrajectory()"<<std::endl;
  // here we transform from Pos&P4 of the RhoCandidate to DecayTreeFitter state
  // (x,y,tx,ty,qoverp)
  
  //-----
//  TVector3 pos0=cand->GetPosition();
//  TVector3 mom0=cand->GetMomentum();
//  double dx0 = vx-pos0.x(); // connect cand position with Vertex V
//  double dy0 = vy-pos0.y();
//  double dz0 = vz-pos0.z();
//  double px0 = mom0.x();
//  double py0 = mom0.y();
//  double pz0 = mom0.z();
//  double pt0 = mom0.Pt();
//  double pt0i = 1./pt0;
//  double cf0 = px0*pt0i;
//  double sf0 = py0*pt0i;
//  double tl0 = pz0*pt0i;
//  double B0 = GetBz(pos0); // [kGs]
//  double a0 = -0.000299792458*B0*cand->Charge(); // [GeV/cm] using B/[kGs]
//  double a0i = 1./a0;
//  double R0 = pt0*a0i;
//  double dd = dx0*sf0 - dy0*cf0 + R0*(tl0*tl0 + 1);
//  double ddi = 1./dd;
//  double ddiq = ddi*ddi;
//  double dal = ddi*(dx0*cf0 + dy0*sf0 + dz0*tl0);
//  double dz1 = tl0*R0*dal; // z shift on helix until poca of V
//  double myz = cand->Pos().Z() + dz1;
   double myz = vz; //requested z
 //-----
  
  //if(ztolerance>0. && fabs(dz0) > ztolerance)   TransportToZ(cand,myz);
  if(ztolerance>0. && fabs(cand->Pos().Z()-vz) > ztolerance) TransportToZ(cand,vz);
  TVector3 pos=cand->Pos();
  TLorentzVector mom = cand->P4(); 
  //TVector3 mom=cand->GetMomentum();
  double px=mom.Px();
  double py=mom.Py();
  double pz=mom.Pz();
  double pzi=1./pz;
  double p2=px*px+py*py+pz*pz;
  double p2i=1./p2;
  double p=sqrt(p2);
  
  if (pz == 0. || p == 0.) {std::cout<<"RhoCalculationTools::StateFromTrajectory(): Zero protection: pz="<<pz<<" Gev/c     p="<<p<<" GeV/c"<<std::endl; return kFALSE;}
  state[0] = pos.x();
  state[1] = pos.y();
  state[2] = px*pzi;
  state[3] = py*pzi;
  state[4] = cand->Charge()/p;

  TMatrixD J(5,7);
  J[0][0] = 1.;
  J[1][1] = 1.;
  J[2][3] = pzi;
  J[2][5] = -pzi*state[2];
  J[3][4] = pzi;
  J[3][5] = -pzi*state[3];
  J[4][3] = -px*state[4]*p2i;
  J[4][4] = -py*state[4]*p2i;
  J[4][5] = -pz*state[4]*p2i;

  TMatrixD cov7 = cand->Cov7();
  TMatrixD tempmat(J,TMatrixD::kMult,cov7);
  TMatrixD cov5(tempmat,TMatrixD::kMultTranspose,J);
  for(int i=0;i<5;i++){
    for(int j=0;j<5;j++){
      cov[i][j]=cov5[i][j];
    }
  }
  
//std::cout<<"RhoCalculationTools::StateFromTrajectory() particle = "<<cand->PdgCode()<<"  Q = "<<cand->Charge()<<std::endl;
//std::cout<<"RhoCalculationTools::StateFromTrajectory() p7 (x,y,z,px,py,pz,E) = ("<<pos.x()<<","<<pos.y()<<","<<pos.z()<<","<<px<<","<<py<<","<<pz<<","<<cand->E()<<")"<<std::endl;
//std::cout<<"RhoCalculationTools::StateFromTrajectory() state (x,y,(z),tx,ty,q/p) = ("<<state[0]<<","<<state[1]<<","<<myz<<","<<state[2]<<","<<state[3]<<","<<state[4]<<")"<<std::endl;

  return myz;

};


void RhoCalculationTools::TransportToZ(RhoCandidate* cand, Double_t z)
{
  //std::cout<<"RhoCalculationTools::TransportToZ(): z="<<z<<std::endl;
  // input/output is (x0,y0,z0,px0,py0,pz0)
  // transporting on helix to (x1,y1,z,px1,py1,pz0) (z from input, pz stays)
  TVector3 pos = cand->Pos();
  TLorentzVector mom = cand->P4(); 
  TMatrixD cov7 = cand->Cov7();
  const double dz = z-pos.Z();
  const double px = mom.Px();
  const double py = mom.Py();
  const double pzi = 1./mom.Pz();
  //const double A = dz*pzi;
  const double B = GetBz(pos); // [kGs]
  const double a = -0.000299792458*B*cand->Charge(); // [GeV/cm] using B/[kGs]
  const double ai = 1./a;
  const double rho = a/mom.P();
  const double rhoTimesS = a*dz*pzi;
  const double L=sin(rhoTimesS);
  //const double K=cos(rhoTimesS);  
  const double K=sqrt(1-L*L);  
  
  TMatrixD J(7,7);
  for(int i=0;i<7;i++) J[i][i]=1.;
  //J[2][0] = pzi*(px*K-py*L); // dx/dz0
  //J[2][1] = pzi*(py*K+px*L); // dy/dz0
  //J[2][3] = pzi*a*(-px*L-py*K); // dpx/dz0
  //J[2][4] = pzi*a*(-py*L+px*K); // dpy/dz0
  J[0][3] = L*ai; // dx/dpx0
  J[1][3] = (1.-K)*ai; // dy/dpx0
  J[0][4] = (K-1.)*ai; // dx/dpy0
  J[1][4] = L*ai; // dy/dpy0
  J[3][3] = K; // dpx/dpx0
  J[4][3] = L; // dpy/dpx0
  J[3][4] = -L; // dpx/dpy0
  J[4][4] = K; // dpy/dpy0
  J[3][5] = rho*pzi*(px*L+py*K); // dpx/dpz0
  J[4][5] = rho*pzi*(py*L-px*K); // dpy/dpz0
  
  double x1 = pos.X() + (L*px-(1.-K)*py)*ai;
  double y1 = pos.Y() + (L*py+(1.-K)*px)*ai;
  
  double px1 = px*K-py*L;
  double py1 = py*K+px*L;
  TMatrixD Jcov(J,TMatrixD::kMult,cov7);
  TMatrixD newcov(Jcov,TMatrixD::kMultTranspose,J);

  //std::cout<<"RhoCalculationTools::TransportToZ(z="<<z<<") with K="<<std::setprecision(9)<<K<<", L="<<L<<", a="<<a<<", rho="<<rho<<", rhoTimesS="<<rhoTimesS<<std::endl;
  //std::cout<<"RhoCalculationTools::TransportToZ(): original pos = ( "
  //                <<std::setw(6)<< pos.X() <<", "<<std::setw(6)<< pos.Y() <<", "<<std::setw(6)<< pos.Z()
  //  <<")  mom = ("<<std::setw(6)<< px <<", "<<std::setw(6)<< py <<", "<<std::setw(6)<<std::setw(6)<< mom.Pz() <<", "<< mom.E()<<")"<<std::endl;

  pos.SetXYZ(x1,y1,z);
  mom.SetX(px1);
  mom.SetY(py1);
  //std::cout<<"RhoCalculationTools::TransportToZ(): updated pos  = ( "
  //                <<std::setw(6)<< pos.X() <<", "<<std::setw(6)<< pos.Y() <<", "<<std::setw(6)<< pos.Z()
  //  <<")  mom = ("<<std::setw(6)<< px1 <<", "<<std::setw(6)<< py1 <<", "<<std::setw(6)<<std::setw(6)<< mom.Pz() <<", "<< mom.E()<<")"<<std::endl;

  //std::cout<<"RhoCalculationTools::TransportToZ(): original cov:"<<std::endl; cov7.Print();
  //std::cout<<"RhoCalculationTools::TransportToZ(): jacobian:    "<<std::endl; J.Print();
  //std::cout<<"RhoCalculationTools::TransportToZ(): updated cov: "<<std::endl; newcov.Print();

  cand->SetPos(pos);
  cand->SetP4(mom);
  cand->SetCov7(newcov);
  //std::cout<<"RhoCalculationTools::TransportToZ(): done."<<std::endl;
}













//______________________________________________________________________________
void RhoCalculationTools::PrintMatrix(TMatrixT<double> m)
{
  // Print the matrix as a table of elements.
  // By default the format "%11.4g" is used to print one element.
  // One can specify an alternative format with eg
  //  option ="f=  %6.2f  "
   if (!m.IsValid()) {
     Error("Print","Matrix is invalid");
     return;
  }
  const Int_t ncols  = m.GetNcols();
  const Int_t nrows  = m.GetNrows();
  const Int_t collwb = m.GetColLwb();
  const Int_t rowlwb = m.GetRowLwb();
  const Int_t nelems = m.GetNoElements();
  //build format
  const char *format = "%11.4g ";
  //if (option) {
  //   const char *f = strstr(option,"f=");
  //   if (f) format = f+2;
  //}
  char topbar[25];
  const char *topbarstart="-----";
  
  snprintf(topbar,25,format,123.456789);
  Int_t nch = strlen(topbar);//+1;
  if (nch > 18) nch = 18;

  const char *ftopbar=Form(" %s%dd   |","%",nch-5);
  std::cout<<std::endl<<nrows<<"x"<<ncols<<" matrix is as follows"<<std::endl;
  //Int_t nk = 5+nch*ncols;
  //for (Int_t i = 0; i < nk; i++) topbar[i] = '-';
  //topbar[nk] = 0;
  for (Int_t i = 0; i < nch; i++) topbar[i] = '-';
  topbar[nch] = 0;

  std::cout<<std::endl<<std::endl<<"     |"<<std::flush;
  for (Int_t j = 1; j <= ncols; j++) 
  {
    std::cout<<Form(ftopbar,j+collwb-1)<<std::flush;
  }
  std::cout<<std::endl<<topbarstart;
  for (Int_t j = 1; j <= ncols; j++) std::cout<<topbar;
  std::cout<<std::endl;
  
  if (nelems <= 0) return;
  for (Int_t i = 1; i <= nrows; i++) 
  {
    std::cout<<Form("%4d |",i+rowlwb-1)<<std::flush;
    for (Int_t j = 1; j <= ncols; j++) 
    {
      std::cout<<Form(format,m(i+rowlwb-1,j+collwb-1))<<std::flush;
    }
    std::cout<<std::endl;
   }
  std::cout<<std::endl;
}

//______________________________________________________________________________
void RhoCalculationTools::PrintMatrix(TMatrixTSym<double> m)
{
   if (!m.IsValid()) {
     Error("Print","Matrix is invalid");
     return;
  }
  const Int_t ncols  = m.GetNcols();
  const Int_t nrows  = m.GetNrows();
  const Int_t collwb = m.GetColLwb();
  const Int_t rowlwb = m.GetRowLwb();
  const Int_t nelems = m.GetNoElements();
  //build format
  const char *format = "%11.4g ";
  //if (option) {
  //   const char *f = strstr(option,"f=");
  //   if (f) format = f+2;
  //}
  char topbar[25];
  const char *topbarstart="-----";
  snprintf(topbar,25,format,123.456789);
  Int_t nch = strlen(topbar);//+1;
  if (nch > 18) nch = 18;
  if (nch < 7)  nch = 7;
  const char *ftopbar=Form(" %s%dd   |","%",nch-5);
  std::cout<<std::endl<<nrows<<"x"<<ncols<<" matrix is as follows"<<std::endl;
  for (Int_t i = 0; i < nch; i++) topbar[i] = '-';
  topbar[nch] = 0;
  
  std::cout<<std::endl<<std::endl<<" SYM |"<<std::flush;
  for (Int_t j = 1; j <= ncols; j++) 
  {
    std::cout<<Form(ftopbar,j+collwb-1)<<std::flush;
  }
  std::cout<<std::endl<<topbarstart;
  for (Int_t j = 1; j <= ncols; j++) std::cout<<topbar;
  std::cout<<std::endl;
  
  if (nelems <= 0) return;
  for (Int_t i = 1; i <= nrows; i++) 
  {
    std::cout<<Form("%4d |",i+rowlwb-1)<<std::flush;
    for (Int_t j = 1; j <= ncols; j++) 
    {
      //std::cout<<Form(format,m(i+rowlwb-1,j+collwb-1))<<std::flush;
      std::cout<< (i==j ? "\e[1m" : "\e[0m") <<Form(format,m(i+rowlwb-1,j+collwb-1))<<"\e[0m"<<std::flush;
    }
    std::cout<<std::endl;
   }
  std::cout<<std::endl;
}
//______________________________________________________________________________
void RhoCalculationTools::PrintMatrix(RhoError m)
{
   if (!m.IsValid()) {
     Error("Print","Matrix is invalid");
     return;
  }
  const Int_t ncols  = m.GetNcols();
  const Int_t nrows  = m.GetNrows();
  const Int_t collwb = m.GetColLwb();
  const Int_t rowlwb = m.GetRowLwb();
  const Int_t nelems = m.GetNoElements();
  //build format
  const char *format = "%11.4g ";
  //if (option) {
  //   const char *f = strstr(option,"f=");
  //   if (f) format = f+2;
  //}
  char topbar[25];
  const char *topbarstart ="-----";
  snprintf(topbar,25,format,123.456789);
  Int_t nch = strlen(topbar);//+1;
  if (nch > 18) nch = 18;
  if (nch < 7)  nch = 7;
  const char *ftopbar=Form(" %s%dd   |","%",nch-5);
  std::cout<<std::endl<<nrows<<"x"<<ncols<<" matrix is as follows"<<std::endl;
  for (Int_t i = 0; i < nch; i++) topbar[i] = '-';
  topbar[nch] = 0;
  
  std::cout<<std::endl<<std::endl<<" RHO |"<<std::flush;
  for (Int_t j = 1; j <= ncols; j++) 
  {
    std::cout<<Form(ftopbar,j+collwb-1)<<std::flush;
  }
  std::cout<<std::endl<<topbarstart;
  for (Int_t j = 1; j <= ncols; j++) std::cout<<topbar;
  std::cout<<std::endl;

  if (nelems <= 0) return;
  for (Int_t i = 1; i <= nrows; i++) 
  {
    std::cout<<Form("%4d |",i+rowlwb-1)<<std::flush;
    for (Int_t j = 1; j <= ncols; j++) 
    {
      //std::cout<<Form(format,m(i+rowlwb-1,j+collwb-1))<<std::flush;
      std::cout<< (i==j ? "\e[1m" : "\e[0m") <<Form(format,m(i+rowlwb-1,j+collwb-1))<<"\e[0m"<<std::flush;
    }
    std::cout<<std::endl;
   }
  std::cout<<std::endl;
}
