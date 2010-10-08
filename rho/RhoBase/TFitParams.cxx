//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TFitParams								//
//                                                                      //
// Kinematics base class for TCandidates				//
//									//
// Author List:								//
// Marcel Kunze,  RUB, Feb. 00						//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//									//
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/TFitParams.h"
#include "RhoBase/TRho.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include <assert.h>

ClassImp(TFitParams)

#include <iostream>
using namespace std;

TFitParams::TFitParams() : 
fCharge(0),
fXposition(0),
fYposition(0),
fZposition(0),
fXmomentum(0),
fYmomentum(0),
fZmomentum(0),
fEnergy(0),
fChi2(0),
fStatus(unknown),
fFit(0)
{
  int i;
  for (i=0; i<MATRIXSIZE;i++) fErrP7[i] = 0;
  for (i=0; i<5;i++) fParams[i] = 0;
  for (i=0; i<15;i++) fCov[i] = 0;
}
/*
 TFitParams::TFitParams( Double_t mass, 
 const BtaAbsRecoObject& recoObject,
 const TVector3& origin )
 : _q(0),
 _cov7(7,0)
 {
 Set( mass, recoObject, origin );
 }
 */

TFitParams::TFitParams( Double_t charge,
                       const TVector3& pos,
                       const TLorentzVector& p4,
                       const TMatrixD& cov7 )
:
fChi2(0),
fStatus(unknown),
fFit(0)
{
  SetCharge( charge );
  Set( pos, p4, cov7 );
  for (int i=0; i<15;i++) fCov[i] = 0;
}

TFitParams::TFitParams( Double_t charge,Float_t* par,Float_t* cov )
:
fChi2(0),
fStatus(unknown),
fFit(0)
{
  Set(charge,par,cov);
}

void TFitParams::Set( Double_t charge,Float_t* par,Float_t* cov )
{
  SetCharge( charge );
  SetHelixParms( par );
  if (cov!=0) SetHelixCov( cov );
  
  // Calculate position and momentum
  
  if (charge!=0.0) {
    
    // The helix parameters
    const double d0      = par[0];
    const double phi     = par[1];
    const double rho     = par[2]; // Omega
    const double z0      = par[3];
    const double tanDip  = par[4];
    
    // Search DOCA, move along trajectory
    const double cDip = 1./sqrt(1.+tanDip*tanDip);
    const double sDip = tanDip * cDip;
    const double phi00 = phi;  // Don't normalize
    const double dphi = cDip*rho;
    const double sphi0 = TMath::Sin(phi00);
    const double cphi0 = TMath::Cos(phi00);
    const double x0 = d0*sphi0;
    const double y0 = d0*cphi0;
    const double rhoinverse = 1./rho;
    
    double cosphi, sinphi;
    double f=0.0;
    int n = 0;
    
    static TVector3 pos,dir,delDir;
    while (n++<50) {
	    cosphi = TMath::Cos(phi00 + f*dphi);
	    sinphi = TMath::Sin(phi00 + f*dphi);
	    fXposition =  (sinphi - sphi0)*rhoinverse - x0;
	    fYposition = -(cosphi - cphi0)*rhoinverse + y0;
	    fZposition = z0 + f*sDip;
	    pos.SetXYZ(fXposition,fYposition,fZposition);
	    dir.SetXYZ(cosphi*cDip,sinphi*cDip,sDip);
	    const double delX = -rho * cDip * cDip * sinphi;
	    const double delY =  rho * cDip * cDip * cosphi;
	    delDir.SetXYZ(delX,delY,0.0);
	    const double denom = 1. + pos.Dot(delDir);
	    double df = -pos.Dot(dir) / fabs(denom);
	    if (fabs(df)<1.E-7) break;
	    int pathDir = (df > 0.) ? 1 : -1;
	    df += 0.001 * pathDir * 1.E-5;
	    f += df;
    }
    
    const static double c=2.99792458E-3*TRho::Instance()->GetMagnetField();
    const double pt = -charge*c/rho; // Rho, omega
    fXmomentum = pt*cosphi;
    fYmomentum = pt*sinphi;
    fZmomentum = pt*tanDip; // 1/TanDip
    
    static TDatabasePDG *pdg = TRho::Instance()->GetPDG();
    const static double mass = pdg->GetParticle("pi-")->Mass();
    fEnergy = TMath::Sqrt(fXmomentum*fXmomentum+fYmomentum*fYmomentum+fZmomentum*fZmomentum+mass*mass);
    if (cov!=0) {
	    // Error matrix
	    static TMatrixD covMatrix(5,5);
	    int row=4;
	    int col=4;
	    for( int i= 14; i >= 0; i-- ) {
        covMatrix( row, col )= covMatrix( col, row ) = fCov[i];
        col--;
        if( col < 0 ) {
          row--;
          col= row;
        }
	    }
	    //covMatrix.Print();
	    
	    // Transform covariance matrix
	    // First, construct the Jacobian for the parameter transformation.
	    // For notation clarification, 'eta' refers to vector of parameters,
	    // 'q' refers to momentum and position, {p, x}. Transformed (new) quantites
	    // are primed.
	    // Jacobian is d(eta')/d(eta) = [d(eta')/d(q')][d(q')/d(eta)]
	    
	    // Get matrix of derivatives of new parameters wrt momentum and position.
	    const TVector3 &posit = GetPosition();
	    const TVector3 &mom = GetMomentum();
	    const TMatrixD& DetaDq = DerivsParWrtCoord(mom, posit, (Int_t) charge);
	    const TMatrixD& DqDeta = DerivsCoordWrtPar(par,(Int_t)charge, f);
	    // Jacobian is DetaDq' times Dq'Deta 
	    static TMatrixD JacEta(5,5);
	    JacEta.Mult(DetaDq,DqDeta);
	    
	    // New covariance matrix is V' = JacEta * V * JacEta.T
	    static TMatrixD temp(5,5); temp.Mult(JacEta,covMatrix);
	    double tmp;
	    for (int r = 0; r < 5; r++) {
        for (int cc = 0; cc <= r; cc++) {
          tmp = 0.;
          for (int k = 0; k < 5; k++) {
            tmp += temp(r,k)*JacEta(cc,k);
          }
          covMatrix(r,cc) = covMatrix(cc,r) = tmp;
        }
	    }
	    
	    // Transform to ErrP7 (Covariances in fourvector notation)
	    
	    // Position part
	    
	    //covMatrix.Print();
	    //DqDeta.Print();
	    
	    fErrP7[0] = Correlation(0,0,DqDeta,covMatrix); // x x
	    fErrP7[1] = Correlation(0,1,DqDeta,covMatrix); // x y
	    fErrP7[2] = Correlation(1,1,DqDeta,covMatrix); // y y
	    fErrP7[3] = Correlation(0,2,DqDeta,covMatrix); // x z
	    fErrP7[4] = Correlation(1,2,DqDeta,covMatrix); // y z
	    fErrP7[5] = Correlation(2,2,DqDeta,covMatrix); // z z
	    
	    // Position-Momentum part
	    
	    fErrP7[6]  = Correlation(0,3,DqDeta,covMatrix); // x px
	    fErrP7[7]  = Correlation(1,3,DqDeta,covMatrix); // y px
	    fErrP7[8]  = Correlation(2,3,DqDeta,covMatrix); // z px
	    fErrP7[9]  = Correlation(0,4,DqDeta,covMatrix); // x py
	    fErrP7[10] = Correlation(1,4,DqDeta,covMatrix); // y py
	    fErrP7[11] = Correlation(2,4,DqDeta,covMatrix); // z py
	    fErrP7[12] = Correlation(0,5,DqDeta,covMatrix); // x pz
	    fErrP7[13] = Correlation(1,5,DqDeta,covMatrix); // y pz
	    fErrP7[14] = Correlation(2,5,DqDeta,covMatrix); // z pz
	    fErrP7[15] = Correlation(0,6,DqDeta,covMatrix); // x e
	    fErrP7[16] = Correlation(1,6,DqDeta,covMatrix); // y e
	    fErrP7[17] = Correlation(2,6,DqDeta,covMatrix); // z e
	    
	    // Momentum part
	    
	    fErrP7[18] = Correlation(3,3,DqDeta,covMatrix); // px px
	    fErrP7[19] = Correlation(3,4,DqDeta,covMatrix); // px py
	    fErrP7[20] = Correlation(4,4,DqDeta,covMatrix); // py py
	    fErrP7[21] = Correlation(3,5,DqDeta,covMatrix); // px pz
	    fErrP7[22] = Correlation(4,5,DqDeta,covMatrix); // py pz
	    fErrP7[23] = Correlation(5,5,DqDeta,covMatrix); // pz pz
	    fErrP7[24] = Correlation(3,6,DqDeta,covMatrix); // px e
	    fErrP7[25] = Correlation(4,6,DqDeta,covMatrix); // py e
	    fErrP7[26] = Correlation(5,6,DqDeta,covMatrix); // pz e
	    fErrP7[27] = Correlation(6,6,DqDeta,covMatrix); // e  e
    }
  }
  else {
    
    // Determine position and energy of neutrals
    
    fXposition = par[0];
    fYposition = par[1];
    fZposition = par[2];
    fEnergy    = par[3];
    
    static TVector3 pos;
    pos.SetXYZ(fXposition,fYposition,fZposition);
    const TVector3 &mom = fEnergy * pos.Unit();
    fXmomentum = mom.X();
    fYmomentum = mom.Y();
    fZmomentum = mom.Z();
    
    if (cov!=0) {
	    
	    // Analytical calculation of the error matrix
	    
	    const double theta = pos.Theta();
	    const double ctheta = TMath::Cos(theta);
	    const double stheta = TMath::Sin(theta);
	    const double phi = pos.Phi();
	    const double cphi = TMath::Cos(phi);
	    const double sphi = TMath::Sin(phi);
	    const double r = pos.Mag();
	    
	    const TError &cov4 = ErrorMatrixOfEmcClusters(fEnergy,theta,phi);
	    
	    // Position part
	    
	    // Covariances in tracking parameters
	    const double ee = cov4(0,0); // Energy
	    const double tt = cov4(1,1); // Theta
	    const double pp = cov4(2,2); // Phi
	    const double rr = cov4(3,3); // Radius
	    
	    // Derivatives
	    const double dxdtheta =  r*cphi*ctheta;
	    const double dxdphi   = -r*sphi*stheta;
	    const double dxdr     =    cphi*stheta;
	    const double dydtheta =  r*sphi*ctheta;
	    const double dydphi   =  r*cphi*stheta;
	    const double dydr     =    sphi*stheta;
	    const double dzdtheta = -r     *stheta;
	    //const double dzdphi   =            0.0;
	    const double dzdr     =         ctheta;
	    
	    // Covariances in fourvector notation
	    fErrP7[0] = dxdtheta*dxdtheta*tt +  dxdphi*dxdphi*pp + dxdr*dxdr*rr; // x x
	    fErrP7[1] = dxdtheta*dydtheta*tt +  dxdphi*dydphi*pp + dxdr*dydr*rr; // x y
	    fErrP7[2] = dydtheta*dydtheta*tt +  dydphi*dydphi*pp + dydr*dydr*rr; // y y
	    fErrP7[3] = dxdtheta*dzdtheta*tt                     + dxdr*dzdr*rr; // x z
	    fErrP7[4] = dydtheta*dzdtheta*tt                     + dydr*dzdr*rr; // y z
	    fErrP7[5] = dzdtheta*dzdtheta*tt                     + dzdr*dzdr*rr; // z z
	    
	    // Position-Momentum part (n/a)
	    
	    fErrP7[6]=0.0;
	    fErrP7[7]=0.0;
	    fErrP7[8]=0.0;
	    fErrP7[9]=0.0;
	    fErrP7[10]=0.0;
	    fErrP7[11]=0.0;
	    fErrP7[12]=0.0;
	    fErrP7[13]=0.0;
	    fErrP7[14]=0.0;
	    fErrP7[15]=0.0;
	    fErrP7[16]=0.0;
	    fErrP7[17]=0.0;
	    fErrP7[18]=0.0;
	    fErrP7[19]=0.0;
	    fErrP7[20]=0.0;
	    fErrP7[21]=0.0;
	    fErrP7[22]=0.0;
	    fErrP7[23]=0.0;
	    
	    // Energy part
	    
	    fErrP7[24]=0.0;
	    fErrP7[25]=0.0;
	    fErrP7[26]=0.0;
	    fErrP7[27]=ee;
    }
  }
}

TFitParams::TFitParams( const TFitParams& o ) :
fCharge(o.fCharge),
fXposition(o.fXposition),
fYposition(o.fYposition),
fZposition(o.fZposition),
fXmomentum(o.fXmomentum),
fYmomentum(o.fYmomentum),
fZmomentum(o.fZmomentum),
fEnergy(o.fEnergy),
fChi2(o.fChi2),
fStatus(o.fStatus),
fFit(o.fFit)
{
  int i;
  for (i=0; i<MATRIXSIZE;i++) fErrP7[i] = o.fErrP7[i];
  for (i=0; i<5;i++) fParams[i] = o.fParams[i];
  for (i=0; i<15;i++) fCov[i] = o.fCov[i];
}

//--------------
// Destructor --
//--------------

TFitParams::~TFitParams( )
{
}

//--------------------
// Member Functions --
//--------------------

void 
TFitParams::SetMass( Double_t m )
{
  SetP4( m, P3() );
}

void 
TFitParams::SetMassAndEnergy( Double_t mass, Double_t mEnergy )
{
  Double_t oldP = P();
  assert( oldP>0 );
  Double_t P2 = pow( mEnergy, 2 ) - pow( mass, 2 );
  assert( P2>0 );
  Double_t newP = sqrt(P2);
  fXmomentum = fXmomentum*newP/oldP;
  fYmomentum = fYmomentum*newP/oldP;
  fZmomentum = fZmomentum*newP/oldP;
  fEnergy    = mEnergy;
}

void 
TFitParams::SetPosition( const TVector3& pos )
{
  fXposition = pos.X();
  fYposition = pos.Y();
  fZposition = pos.Z();
}

void 
TFitParams::SetP3( const TVector3& p3 ) 
{
  // assumes mass is well defined
  Double_t mass = M();
  SetP4( mass, p3 );
}

void 
TFitParams::SetP4( Double_t mass, const TVector3& p3 )
{
  fXmomentum = p3.X();
  fYmomentum = p3.Y();
  fZmomentum = p3.Z();
  Double_t En = sqrt( mass*mass + p3.Mag2() );
  //assert( En>0 );
  fEnergy = En;
}

void 
TFitParams::SetP4( const TLorentzVector& p4 )
{
  fXmomentum = p4.X();
  fYmomentum = p4.Y();
  fZmomentum = p4.Z();
  fEnergy    = p4.T();
}

void 
TFitParams::SetP7( const TVector3& pos, const TLorentzVector& p4 )
{
  SetPos( pos );
  SetP4( p4 );
}

void 
TFitParams::SetCov7( const TMatrixD& cov7 )
{
  // position error
  
  fErrP7[0] = cov7(0,0);  fErrP7[1] = cov7(1,0); fErrP7[2] = cov7(1,1);  
  fErrP7[3] = cov7(2,0);  fErrP7[4] = cov7(2,1); fErrP7[5] = cov7(2,2);
  
  // position-momentum covariance
  
  fErrP7[6] = cov7(3,0);   fErrP7[7] = cov7(3,1);  fErrP7[8] = cov7(3,2);
  fErrP7[9] = cov7(4,0);   fErrP7[10] = cov7(4,1); fErrP7[11] = cov7(4,2);
  fErrP7[12] = cov7(5,0);  fErrP7[13] = cov7(5,1); fErrP7[14] = cov7(5,2);
  fErrP7[15] = cov7(6,0);  fErrP7[16] = cov7(6,1); fErrP7[17] = cov7(6,2);
  
  // momentum error
  fErrP7[18] = cov7(3,3);  fErrP7[19] = cov7(4,3); fErrP7[20] = cov7(4,4);
  fErrP7[21] = cov7(5,3);  fErrP7[22] = cov7(5,4); fErrP7[23] = cov7(5,5);
  fErrP7[24] = cov7(6,3);  fErrP7[25] = cov7(6,4); fErrP7[26] = cov7(6,5); 
  fErrP7[27] = cov7(6,6);
}

void 
TFitParams::SetCov7( const TMatrixD& covPos, const TMatrixD& covP4 )
{
  // position error
  
  fErrP7[0] = covPos(0,0);  fErrP7[1] = covPos(1,0); fErrP7[2] = covPos(1,1);  
  fErrP7[3] = covPos(2,0);  fErrP7[4] = covPos(2,1); fErrP7[5] = covPos(2,2);
  
  // position-momentum covariance
  
  fErrP7[6] = 0;   fErrP7[7] = 0;  fErrP7[8] = 0;
  fErrP7[9] = 0;   fErrP7[10] = 0; fErrP7[11] = 0;
  fErrP7[12] = 0;  fErrP7[13] = 0; fErrP7[14] = 0;
  fErrP7[15] = 0;  fErrP7[16] = 0; fErrP7[17] = 0;
  
  // momentum error
  fErrP7[18] = covP4(0,0);  fErrP7[19] = covP4(1,0); fErrP7[20] = covP4(1,1);
  fErrP7[21] = covP4(2,0);  fErrP7[22] = covP4(2,1); fErrP7[23] = covP4(2,2);
  fErrP7[24] = covP4(3,0);  fErrP7[25] = covP4(3,1); fErrP7[26] = covP4(3,2); 
  fErrP7[27] = covP4(3,3);
}

void 
TFitParams::SetCov7( const TMatrixD& covPos, const TMatrixD& covP4, const TMatrixD& covPosP4 )
{
  // position error
  
  fErrP7[0] = covPos(0,0);  fErrP7[1] = covPos(1,0); fErrP7[2] = covPos(1,1);  
  fErrP7[3] = covPos(2,0);  fErrP7[4] = covPos(2,1); fErrP7[5] = covPos(2,2);
  
  // position-momentum covariance
  
  fErrP7[6] = covPosP4(0,0);   fErrP7[7] = covPosP4(0,1);  fErrP7[8] = covPosP4(0,2);
  fErrP7[9] = covPosP4(1,0);   fErrP7[10] = covPosP4(1,1); fErrP7[11] = covPosP4(1,2);
  fErrP7[12] = covPosP4(2,0);  fErrP7[13] = covPosP4(2,1); fErrP7[14] = covPosP4(2,2);
  fErrP7[15] = covPosP4(3,0);  fErrP7[16] = covPosP4(3,1); fErrP7[17] = covPosP4(3,2);
  
  // momentum error
  fErrP7[18] = covP4(0,0);  fErrP7[19] = covP4(1,0); fErrP7[20] = covP4(1,1);
  fErrP7[21] = covP4(2,0);  fErrP7[22] = covP4(2,1); fErrP7[23] = covP4(2,2);
  fErrP7[24] = covP4(3,0);  fErrP7[25] = covP4(3,1); fErrP7[26] = covP4(3,2); 
  fErrP7[27] = covP4(3,3);
}

void 
TFitParams::SetCovP4( const TMatrixD& covP4 )
{
  // position error
  
  fErrP7[0] = 0;  fErrP7[1] = 0; fErrP7[2] = 0;  
  fErrP7[3] = 0;  fErrP7[4] = 0; fErrP7[5] = 0;
  
  // position-momentum covariance
  
  fErrP7[6] = 0;   fErrP7[7] = 0;  fErrP7[8] = 0;
  fErrP7[9] = 0;   fErrP7[10] = 0; fErrP7[11] = 0;
  fErrP7[12] = 0;  fErrP7[13] = 0; fErrP7[14] = 0;
  fErrP7[15] = 0;  fErrP7[16] = 0; fErrP7[17] = 0;
  
  // momentum error
  fErrP7[18] = covP4(0,0);  fErrP7[19] = covP4(1,0); fErrP7[20] = covP4(1,1);
  fErrP7[21] = covP4(2,0);  fErrP7[22] = covP4(2,1); fErrP7[23] = covP4(2,2);
  fErrP7[24] = covP4(3,0);  fErrP7[25] = covP4(3,1); fErrP7[26] = covP4(3,2); 
  fErrP7[27] = covP4(3,3);
}
/*
 void
 TFitParams::Set( Double_t mass, 
 const BtaAbsRecoObject& recoObject,
 const TVector3& origin )
 {
 SetCharge( recoObject.charge() );
 Set( mass, 
 recoObject.positionErr( origin ), 
 recoObject.momentumErr( origin ) );
 }
 */
void 
TFitParams::Set( const TVector3& pos, 
                const TLorentzVector& p4, 
                const TMatrixD& cov7 )
{
  SetP7( pos, p4 );
  SetCov7( cov7 );
}

void 
TFitParams::Set( Double_t mass,
                const TPointErr& posErr, 
                const TVectorErr& p3Err,
                const TMatrixD& xpErr)
{
  SetPos( posErr );
  SetP4( mass, p3Err );
  
  // compute the jacobian, assuming that mass is a well defined quantity  
  static TMatrixD jacobian( 4, 3);
  jacobian(0,0) = 1.0;
  jacobian(0,1) = 0.0;
  jacobian(0,2) = 0.0;
  jacobian(1,0) = 0.0;
  jacobian(1,1) = 1.0;
  jacobian(1,2) = 0.0;
  jacobian(2,0) = 0.0;
  jacobian(2,1) = 0.0;
  jacobian(2,2) = 1.0;
  jacobian(3,0) = fXmomentum/fEnergy;
  jacobian(3,1) = fYmomentum/fEnergy;
  jacobian(3,2) = fZmomentum/fEnergy;
  static TMatrixD xp4Cov(3,4);
  for(int i=0;i<3;i++){
    xp4Cov(i,3) = 0;
    for(int j=0;j<3;j++){
	    xp4Cov(i,j)  = xpErr(i,j);
	    xp4Cov(i,3) += xpErr(i,j)*jacobian(3,j);
    }
  }
  
  // Sets the covariance matrix
  SetCov7( posErr.CovMatrix(), p3Err.CovMatrix().Similarity(jacobian),xp4Cov );
}



Bool_t 
TFitParams::operator== (const TFitParams& c) const
{ 
  return this == &c;
}

void TFitParams::Boost(Double_t bx, Double_t by, Double_t bz){
  Double_t b2 = bx*bx + by*by + bz*bz;
  Double_t gamma = 1.0 / sqrt(1.0 - b2);
  Double_t bp = bx*fXmomentum + by*fYmomentum + bz*fZmomentum;
  Double_t gamma2 = b2 > 0 ? (gamma - 1.0)/b2 : 0.0;
  
  fXmomentum = fXmomentum + gamma2*bp*bx + gamma*bx*fEnergy;
  fYmomentum = fYmomentum + gamma2*bp*by + gamma*by*fEnergy;
  fZmomentum = fZmomentum + gamma2*bp*bz + gamma*bz*fEnergy;
  fEnergy = gamma*(fEnergy + bp);
}

void TFitParams::SetHelixParms(float *p) 
{
  for (int i=0; i<5;i++) fParams[i] = p[i];
}

void TFitParams::SetHelixCov(float *p)
{
  for (int i=0; i<15;i++) fCov[i] = p[i];
}

void TFitParams::PrintOn(std::ostream& o) const {
  o << "(" << fXmomentum << "," << fYmomentum << "," << fZmomentum << ";" << fEnergy <<  ") q=" << Charge() << " at ";
  o << "(" << fXposition << "," << fYposition << "," << fZposition << ") " << endl;
  o << "Position covariance:" << endl;
  o << fErrP7[0] << '\t' << fErrP7[1] << '\t' << fErrP7[3] << endl;
  o << '\t'      << '\t' << fErrP7[2] << '\t' << fErrP7[4] << endl;
  o << '\t'      << '\t' << '\t'      << '\t' << fErrP7[5] << endl;
  o << "Momentum covariance:" << endl;
  o << fErrP7[18] << '\t' << fErrP7[19] << '\t' << fErrP7[21] << '\t' << fErrP7[24] << endl;
  o << '\t'       << '\t' << fErrP7[20] << '\t' << fErrP7[22] << '\t' << fErrP7[25] << endl;
  o << '\t'       << '\t' << '\t'       << '\t' << fErrP7[23] << '\t' << fErrP7[26] << endl;
  o << '\t'       << '\t' << '\t'       << '\t' << '\t'       << '\t' << fErrP7[27] << endl;
  o << "Position-Momentum covariance:" << endl;
  o << fErrP7[6] << '\t' << fErrP7[9]  << '\t' << fErrP7[12] << '\t' << fErrP7[15] << endl;
  o << fErrP7[7] << '\t' << fErrP7[10] << '\t' << fErrP7[13] << '\t' << fErrP7[16] << endl;
  o << fErrP7[8] << '\t' << fErrP7[11] << '\t' << fErrP7[14] << '\t' << fErrP7[17] << endl;
}

TError&
TFitParams::ErrorMatrixOfEmcClusters(Double_t clEnergy,Double_t clTheta,Double_t clPhi) const{
  
  //  Jan 11 - 2000, Per reinertsen
  //  Okey, I'll hardwire the results from our study in Bergen. A Babar note will 
  //  be ready pretty soon. 
  //  The energy resolution is the relative resolution (Delta E/E) , the theta 
  //  resolution is Delta (cos theta) while the phi resolution is simply 
  //  Delta phi in radians. The position resolution results are based upon the 
  //  logarithmic weighting method (max[0,4.0+log(E_{i}/E_clus)]). The correlation 
  //  entries are found to be very small and are thus set equal to zero.   
  
  // Seven regions are probed 
  Double_t thetaMean[7]={0.1700636,0.3557819,0.7937707,0.8437179,0.8925519,0.9041466,0.9334562};
  
  //Each resolution parameter for each region is represented as a function of energy
  //The energy resolution is represented by a three parameter function  
  Double_t fit1Energy[7]={2.220309,2.345498,2.551324,2.555908,2.304174,2.186032,2.118749};
  Double_t fit2Energy[7]={0.6664312,0.2305636,1.7593733E-04,1.8059720E-04,0.9898133,0.1151342,2.5358578E-04};
  Double_t fit3Energy[7]={0.3464189,0.3311598,0.2569567,0.1612448,0.2685481,0.3284982,0.3220451}; 
  //The phi and theta resolution is represented by a two parameter function 
  Double_t fit1Phi[7]={0.5607618,0.5731843,0.5849221,0.5878536,0.5823427,0.5744874,0.6685444};
  Double_t fit2Phi[7]={1.0343187E-05,1.1479809E-06,1.2926652E-05,2.0522260E-05,1.6713226E-05,1.7717166E-04,2.5775073E-05};
  Double_t fit1Theta[7]={0.5307526,0.4813908,0.2160047,0.1632318,0.1277156,0.1124882,9.2022523E-02};
  Double_t fit2Theta[7]={0.1508664,0.1566977,6.0490336E-02,5.8269389E-02,3.5151571E-02,4.0520933E-02,2.7675632E-02};
  Double_t clCosTheta = cos(clTheta);
  Double_t clSinTheta = sin(clTheta);
  
  
  // Create a position parameter in theta (maps) that will be the 
  // effective position from where the resolution is found. 
  
  Double_t maps  = clCosTheta ; 
  
  // In the straight uppward region use the first measured point
  
  if ( (clCosTheta < 0.1700636 ) && (clCosTheta >  -0.1700636 ) )  {
    maps =  0.1700636 ;
  }
  
  // In the backward region use the corresponding region in the forward region 
  if (clCosTheta < - 0.1700636 ) {
    maps = - clCosTheta ;
  }
  // If in the very forward region, use the last measured point. 
  
  if ( maps >= 0.9334562 ) {
    maps = 0.9334562 ;
  } 
  
  //Do a linear interpolation between current position and the measured positions
  
  Double_t temp1Phi=0.,temp2Phi=0.,temp1Theta=0.,temp2Theta=0.,temp1Energy=0.,temp2Energy=0.,delta=0.,distance=0.;
  
  for (int i = 0; i < 6 ; i++) {
    if ( ( maps <= thetaMean[i+1] ) && ( maps >= thetaMean[i] ) ) {
	    Double_t en1  = fit1Phi[i]/( pow(clEnergy,0.4) ) ;
	    Double_t en2  = fit2Phi[i] ;
	    temp1Phi    = sqrt( en1*en1 + en2*en2 ) ;
	    en1  = fit1Phi[i+1]/( pow(clEnergy,0.4) ) ;
	    en2  = fit2Phi[i+1] ;
	    temp2Phi    = sqrt( en1*en1 + en2*en2 ) ;
	    
	    en1  = fit1Theta[i]/( pow(clEnergy,0.4) ) ;
	    en2  = fit2Theta[i] ;
	    temp1Theta    = sqrt( en1*en1 + en2*en2 ) ;
	    en1  = fit1Theta[i+1]/( pow(clEnergy,0.4) ) ;
	    en2  = fit2Theta[i+1] ;
	    temp2Theta    = sqrt( en1*en1 + en2*en2 ) ;
	    
	    en1  = fit1Energy[i]/( sqrt(sqrt(clEnergy)) ) ;
	    en2  = fit2Energy[i] ;
	    Double_t en3  = fit3Energy[i]*clEnergy ;
	    temp1Energy = sqrt( en1*en1 + en2*en2 + en3*en3 ) ;
	    en1  = fit1Energy[i+1]/( pow(clEnergy,0.4) ) ;
	    en2  = fit2Energy[i+1] ;
	    en3  = fit3Energy[i+1]*clEnergy ;
	    temp2Energy = sqrt( en1*en1 + en2*en2 + en3*en3 ) ;
	    
	    
	    delta    = thetaMean[i+1]-thetaMean[i] ;
	    distance = maps - thetaMean[i] ;
    }   
  }
  
  Double_t errPhi    = ( temp2Phi - temp1Phi )*distance/delta + temp1Phi ;
  errPhi /= 100. ;
  Double_t errTheta  = ( temp2Theta - temp1Theta )*distance/delta + temp1Theta ;
  errTheta /= 100. ;
  //HM: turn Delta[cos(theta)] into Delta[theta]
  errTheta /= clSinTheta;
  Double_t errEnergy = ( temp2Energy - temp1Energy )*distance/delta + temp1Energy ;
  errEnergy /= 100. ;
  //HM: turn Delta[E]/E into Delta[E]
  errEnergy *= clEnergy;
  
  // Copy from below
  // I presume this is measured in cm, so I'll put in a largish number
  Double_t errR = 10;  
  
  static TError theError(4);
  
  theError(0,0) = errEnergy*errEnergy ; 
  theError(1,1) = errTheta*errTheta ; 
  theError(2,2) = errPhi*errPhi ; 
  theError(3,3) = errR*errR ; 
  
  return theError ;
}

const TMatrixD&
TFitParams::DerivsParWrtCoord(TVector3 mom, TVector3 pos, Int_t charge) const 
{
  
  static TMatrixD derivs(5,7);
  derivs.Zero();
  
  // Calcualte some common stuff
  const double A = -2.99792458E-3*TRho::Instance()->GetMagnetField()*charge;
  const double pT = mom.Perp();
  const double pT2 = mom.Perp2();
  const double pT3 = pow(pT, 3.);
  const double pYx = mom.Y() - A*pos.X();
  const double pXy = mom.X() + A*pos.Y();
  const double denom2 = pYx*pYx + pXy*pXy;
  const double denom = sqrt(denom2);
  const double r2 = pos.X()*pos.X() + pos.Y()*pos.Y();
  
  assert(0. != A);
  assert(0. != pT);
  assert(0. != denom);
  
  // d0 wrt pX
  derivs(0,3) = (1/A) * (pXy/denom - mom.X()/pT);
  // d0 wrt pY
  derivs(0,4) = (1/A) * (pYx/denom - mom.Y()/pT);
  // d0 wrt X
  derivs(0,0) = -pYx/denom;
  // d0 wrt Y
  derivs(0,1) = pXy/denom;
  // d0 wrt E
  derivs(0,6) = 0.0;
  
  // phi0 wrt pX
  derivs(1,3) = -pYx/denom2;
  // phi0 wrt pY
  derivs(1,4) = pXy/denom2;
  // phi0 wrt X
  derivs(1,0) = -A * pXy/denom2;
  // phi0 wrt Y
  derivs(1,1) = -A * pYx/denom2;
  // phi0 wrt E
  derivs(1,6) = 0.0;
  
  // omega wrt pX
  derivs(2,3) = -A * mom.X()/pT3;
  // omega wrt pY
  derivs(2,4) = -A * mom.Y()/pT3;
  // omega wrt E
  derivs(2,6) = -0.5*A * (1+mom.Z()*mom.Z()/pT2)/pow(mom.Mag(),3.);
  
  // z0 wrt pX
  derivs(3,3) = 
	mom.Z()*(mom.X()*mom.X()*pos.X() + 2*mom.X()*mom.Y()*pos.Y() -
           mom.Y()*mom.Y()*pos.X() + A*mom.Y()*r2) / (pT2 * denom2);
  // z0 wrt pY
  derivs(3,4) = 
	-mom.Z()*(mom.X()*mom.X()*pos.Y() - 2*mom.X()*mom.Y()*pos.X() -
            mom.Y()*mom.Y()*pos.Y() + A*mom.X()*r2) / (pT2 * denom2);
  // z0 wrt pZ
  derivs(3,5) =
	-(1./A) * atan(A*(mom.X()*pos.X() + mom.Y()*pos.Y())/
                 (mom.X()*pXy + mom.Y()*pYx));
  // z0 wrt E
  derivs(3,6) = 0.0;
  // z0 wrt X
  derivs(3,0) = -mom.Z()*pXy/denom2;
  // z0 wrt Y
  derivs(3,1) = -mom.Z()*pYx/denom2;
  // z0 wrt Z
  derivs(3,2) = 1.;
  
  // tanDip wrt pX
  derivs(4,3) = -mom.X()*mom.Z()/pT3;
  // tanDip wrt pY
  derivs(4,4) = -mom.Y()*mom.Z()/pT3;
  // tanDip wrt pZ
  derivs(4,5) = 1./pT;
  // tanDip wrt E
  double pp = mom.Mag2() / pT2;
  derivs(4,6) = pp / sqrt(pp - 1.);
  
  return derivs;
}

const TMatrixD&
TFitParams::DerivsCoordWrtPar(Float_t *params, Int_t charge, Double_t fltlen) const 
{
  
  static TMatrixD derivs(7,5);
  derivs.Zero();
  
  // Calcualte some common stuff
  const double A = -2.99792458E-3*TRho::Instance()->GetMagnetField()*charge;
  const double d0 = params[0];
  const double phi0 = params[1];
  const double omega = params[2];
  //const double z0 = params[3]; //unused?
  const double tanDip = params[4];
  const double cosDip = 1./sqrt(1. + tanDip*tanDip);
  const double sinDip = cosDip*tanDip;
  const double sincosDip = cosDip*sinDip;
  const double translen = fltlen*cosDip;
  const double arc = translen*omega;
  const double cosPhi = TMath::Cos(phi0);
  const double sinPhi = TMath::Sin(phi0);
  const double cosAng = TMath::Cos(phi0 + arc);
  const double sinAng = TMath::Sin(phi0 + arc);
  const double omega2 = omega*omega;
  
  assert (0 != omega);
  
  const double omegainv  = 1./omega;
  const double omegainv2 = omegainv/omega;
  const double Aomegainv  = A*omegainv;
  const double Aomegainv2 = A*omegainv2;
  
  // X wrt d0
  derivs(0,0) = -sinPhi;
  // X wrt phi0
  derivs(0,1) =  omegainv * (cosAng - cosPhi) - d0*cosPhi;
  // X wrt omega
  derivs(0,2) =  omegainv * cosAng*translen - omegainv2 * (sinAng - sinPhi);
  // X wrt tanDip
  derivs(0,4) = -omegainv * cosAng * arc*sincosDip;
  
  // Y wrt d0
  derivs(1,0) =  cosPhi;
  // Y wrt phi0
  derivs(1,1) =  omegainv * (sinAng - sinPhi) - d0*sinPhi;
  // Y wrt omega
  derivs(1,2) =  omegainv * sinAng*translen - omegainv2 * (-cosAng + cosPhi);
  // Y wrt tanDip
  derivs(1,4) = -omegainv * sinAng * arc*sincosDip;
  
  // Z wrt z0
  derivs(2,3) = 1.;
  // Z wrt tanDip
  derivs(2,4) = -(tanDip*tanDip)*translen*cosDip*cosDip + translen;
  
  // pX wrt phi0
  derivs(3,1) = -Aomegainv * sinAng;
  // pX wrt omega
  derivs(3,2) = -Aomegainv * sinAng*translen - Aomegainv2 * cosAng;
  // pX wrt tanDip
  derivs(3,4) =  Aomegainv * sinAng * arc*sincosDip;
  
  // pY wrt phi0
  derivs(4,1) =  Aomegainv * cosAng;
  // pY wrt omega
  derivs(4,2) =  Aomegainv * cosAng*translen - Aomegainv2 * sinAng;
  // pY wrt tanDip
  derivs(4,4) = -Aomegainv * cosAng * arc*sincosDip;
  
  // pZ wrt omega
  derivs(5,2) = -Aomegainv2 * tanDip;
  // pZ wrt tanDip
  derivs(5,4) =  Aomegainv;
  
  // Energy wrt d0
  derivs(6,0) = 0.0;
  // Energy wrt phi0
  derivs(6,1) = 0.0;
  // Energy wrt omega
  derivs(6,2) = -(A*A/(omega2*omega))*(1.+tanDip*tanDip)/fEnergy;
  // Energy wrt z0
  derivs(6,3) = 0.0;
  // Energy wrt tanDip
  derivs(6,4) = 0.5*(A*A/omega2)*(1.+2.*tanDip)/fEnergy;
  
  return derivs;
}

TError&          
TFitParams::PosCov()   const
{
  static TError covPos(3);
  covPos(0,0) = fErrP7[0];  covPos(1,0) = covPos(0,1) = fErrP7[1]; covPos(1,1) = fErrP7[2];  
  covPos(2,0) = covPos(0,2) = fErrP7[3];  covPos(2,1) = covPos(1,2) = fErrP7[4]; 
  covPos(2,2) = fErrP7[5];
  return covPos;
}
//Dipak
void TFitParams::SetPosCov(const TMatrixD& errPos)
{
	//Int_t ind = 0;
  // 	for(int i=0;i<3; i++)
  // 		for(int j=i;j<3;j++)fErrP7[ind++] = errPos[j][i];
	fErrP7[0] = errPos[0][0]; fErrP7[1] = errPos[1][0]; fErrP7[2] = errPos[1][1];
	fErrP7[3] = errPos[2][0]; fErrP7[4] = errPos[2][1]; fErrP7[5] = errPos[2][2];
}

TPointErr          
TFitParams::PosWCov()   const
{
  return TPointErr(Pos(),PosCov());
}


TError&          
TFitParams::P4Cov()   const
{
  static TError covP4(4);
  covP4(0,0) = fErrP7[18];  covP4(1,0) = fErrP7[19]; covP4(1,1) = fErrP7[20];
  covP4(2,0) = fErrP7[21];  covP4(2,1) = fErrP7[22]; covP4(2,2) = fErrP7[23];
  covP4(3,0) = fErrP7[24];  covP4(3,1) = fErrP7[25]; covP4(3,2) = fErrP7[26]; 
  covP4(3,3) = fErrP7[27];
  return covP4;
}

TError&          
TFitParams::P3Cov()   const
{
  static TError covP3(3);
  covP3(0,0) = fErrP7[18];  covP3(1,0) = fErrP7[19]; covP3(1,1) = fErrP7[20];
  covP3(2,0) = fErrP7[21];  covP3(2,1) = fErrP7[22]; covP3(2,2) = fErrP7[23];
  return covP3;
}

TVectorErr          
TFitParams::P3WErr()   const
{
  return TVectorErr(P3(),P3Cov());
}


TLorentzVectorErr          
TFitParams::P4WErr()   const
{
  return TLorentzVectorErr(P4(),P4Cov());
}


Double_t           
TFitParams::EVar()   const
{
  return fErrP7[27];
}

Double_t                
TFitParams::P()   const
{
  return TVector3(fXmomentum,fYmomentum,fZmomentum).Mag();
}
/*
 Double_t                
 TFitParams::PVar()   const
 {
 return  P3WErr().covRTPMatrix()[TVectorErr::Rho][TVectorErr::Rho];
 }
 */
Double_t             
TFitParams::M()   const
{
  return TLorentzVector(fXmomentum,fYmomentum,fZmomentum,fEnergy).Mag();
}
/*
 Double_t             
 TFitParams::MVar()   const
 {
 return P4WErr().covMRTPMatrix()[TLorentzVectorErr::Mass][TLorentzVectorErr::Mass];
 }
 */
TMatrixD&
TFitParams::Cov7()   const
{
  static TMatrixD cov(7,7);
  
  // position error
  
  cov(0,0) = fErrP7[0];  cov(1,0) = fErrP7[1]; cov(1,1) = fErrP7[2];  
  cov(2,0) = fErrP7[3];  cov(2,1) = fErrP7[4]; cov(2,2) = fErrP7[5];
  
  // position-momentum covariance
  
  cov(3,0) = fErrP7[6];   cov(3,1) = fErrP7[7];  cov(3,2) = fErrP7[8];
  cov(4,0) = fErrP7[9];   cov(4,1) = fErrP7[10]; cov(4,2) = fErrP7[11];
  cov(5,0) = fErrP7[12];  cov(5,1) = fErrP7[13]; cov(5,2) = fErrP7[14];
  cov(6,0) = fErrP7[15];  cov(6,1) = fErrP7[16]; cov(6,2) = fErrP7[17];
  
  // momentum error
  cov(3,3) = fErrP7[18];  cov(4,3) = fErrP7[19]; cov(4,4) = fErrP7[20];
  cov(5,3) = fErrP7[21];  cov(5,4) = fErrP7[22]; cov(5,5) = fErrP7[23];
  cov(6,3) = fErrP7[24];  cov(6,4) = fErrP7[25]; cov(6,5) = fErrP7[26]; 
  cov(6,6) = fErrP7[27];
  
  //latest modification :Dipak
  for (int i=0; i<6; i++)
    for (int j=i+1; j<7; j++)
      cov(i,j)=cov(j,i);
  
  return cov;
}

TMatrixD&
TFitParams::XPCov()   const
{
  static TMatrixD covPosP4(4,3);
  
  // position-momentum covariance
  
  covPosP4(0,0) = fErrP7[6];   covPosP4(0,1) = fErrP7[7];  covPosP4(0,2) = fErrP7[8];
  covPosP4(1,0) = fErrP7[9];   covPosP4(1,1) = fErrP7[10]; covPosP4(1,2) = fErrP7[11];
  covPosP4(2,0) = fErrP7[12];  covPosP4(2,1) = fErrP7[13]; covPosP4(2,2) = fErrP7[14];
  covPosP4(3,0) = fErrP7[15];  covPosP4(3,1) = fErrP7[16]; covPosP4(3,2) = fErrP7[17];
  
  return covPosP4;
}

Double_t 
TFitParams::Correlation(Int_t x1,Int_t x2,const TMatrixD& deriv,const TMatrixD &cov) const 
{
  const int nparm = cov.GetNcols();
  Double_t error = 0.;
  for(int i=0; i<nparm; i++) {
    const double d1 = deriv(x1,i);
    if (d1==0.0) continue; // No need to accumulate 0
    for(int j=0; j<nparm; j++) {
	    const double d2 = deriv(x2,j);
	    if (d2==0.0) continue; // No need to accumulate 0
	    error+=d1*cov(i,j)*d2;
    }
  }
  return error;
}
