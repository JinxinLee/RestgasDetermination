//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoCandidate                                                         //
//                                                                      //
// Contains (polymorphic) contents for RhoCandidate objects             //
// Candidate "Tracks" or "Particles" for analysis use                   //
//                                                                      //
// Author List:                                                         //
// Sascha Berger, RUB, Feb.99                                           //
// Marcel Kunze,  RUB, Aug.99                                           //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <assert.h>

//#include "RhoBase/TRho.h"
#include "RhoBase/RhoFactory.h"
#include "RhoBase/RhoCandidate.h"
#include "RhoBase/RhoCandidate.h"
#include "RhoBase/RhoCandList.h"
#include "RhoBase/RhoCandListIterator.h"
#include "RhoMath/RhoError.h"
#include "RhoMath/RhoVector3Err.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "RhoCalculationTools.h"

#include "TDatabasePDG.h"

ClassImp ( RhoCandidate )

#include <iostream>
#include <iomanip>
using namespace std;

RhoCandidate::RhoCandidate() :
  fFastMode ( kFALSE ),
  fLocked ( kFALSE ),
  fTheMother ( 0 ),
//  fDecayVtx ( 0 ),
  fPdtEntry ( 0 ),
  fPdgCode ( 0 ),
  fIsAResonance ( kFALSE ),
  //fTruth ( 0 ),
  fMicroCand ( 0 ),
  fTrackNumber ( -1 ),
  fUid ( 0 ),
  fNDaug ( 0 ),
  //fDaugList ( 0 ),
  fNCons ( 0 ),
  fChi2 ( 0 ),
  fMcTruth ( 0 ),
  fFit ( 0 )
{
  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
  SetUid();
  SetPidInfo ( 0 );
  for ( int i=0; i<30; i++ ) { fPidLH[i]=-999999.;}
  for ( int i=0; i<MATRIXSIZE; i++ ) { fErrP7[i] = 0.; }
}

RhoCandidate::RhoCandidate ( const TLorentzVector& v, Double_t charge, RhoVector3Err* vp ) :
  fFastMode ( kFALSE ),
  fLocked ( kFALSE ),
  fTheMother ( 0 ),
//  fDecayVtx ( vp ),
  fPdtEntry ( 0 ),
  fPdgCode ( 0 ),
  fIsAResonance ( kFALSE ),
  // fTruth ( 0 ),
  fMicroCand ( 0 ),
  fTrackNumber ( -1 ),
  fUid ( 0 ),
  fNDaug ( 0 ),
  //fDaugList ( 0 ),
  fNCons ( 0 ),
  fChi2 ( 0 ),
  fMcTruth ( 0 ),
  fFit ( 0 )
{
  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
  SetCharge ( charge );
  SetP7 ( ( vp!=0 ? *((TVector3*)vp) : TVector3 ( 0.,0.,0. ) ), v );
  SetUid();
  SetPidInfo ( 0 );
  for ( int i=0; i<30; i++ ) { fPidLH[i]=-999999.;}
  for ( int i=0; i<MATRIXSIZE; i++ ) { fErrP7[i] = 0.; }
}


RhoCandidate::RhoCandidate ( const TVector3& v, const TParticlePDG* pdt, RhoVector3Err* vp ) :
  fFastMode ( kFALSE ),
  fLocked ( kFALSE ),
  fTheMother ( 0 ),
//  fDecayVtx ( vp ),
  fPdtEntry ( 0 ),
  fPdgCode ( 0 ),
  fIsAResonance ( kFALSE ),
//       fTruth ( 0 ),
  fMicroCand ( 0 ),
  fTrackNumber ( -1 ),
  fUid ( 0 ),
  fNDaug ( 0 ),
 // fDaugList ( 0 ),
  fNCons ( 0 ),
  fChi2 ( 0 ),
  fMcTruth ( 0 ),
  fFit ( 0 )
{
  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
  SetPos ( vp!=0 ? *((TVector3*)vp) : TVector3 ( 0.,0.,0. ) );
  SetP3 ( v );
  SetType ( pdt );
  SetUid();
  SetPidInfo ( 0 );
  for ( int i=0; i<30; i++ ) {fPidLH[i]=-999999.; }
  for ( int i=0; i<MATRIXSIZE; i++ ) { fErrP7[i] = 0.; }
}


RhoCandidate::RhoCandidate ( const RhoCandidate& o ) : FairMultiLinkedData_Interface(o)
{
  fFastMode = o.fFastMode;
  fLocked = kFALSE;
  fTheMother = 0;
  fDecayVtx = o.fDecayVtx;
  fPdtEntry = o.fPdtEntry;
  fPdgCode  = o.fPdgCode;
  fIsAResonance = o.fIsAResonance;
//   fTruth = o.fTruth;
  fTrackNumber = o.fTrackNumber;
  fUid = o.fUid;
  fNDaug = o.fNDaug;
  fMicroCand = o.fMicroCand;

  fMarker[0] = o.fMarker[0];
  fMarker[1] = o.fMarker[1];
  fMarker[2] = o.fMarker[2];
  fMarker[3] = o.fMarker[3];

  fCharge    = o.fCharge;
  fXposition = o.fXposition;
  fYposition = o.fYposition;
  fZposition = o.fZposition;
  fXmomentum = o.fXmomentum;
  fYmomentum = o.fYmomentum;
  fZmomentum = o.fZmomentum;
  fEnergy    = o.fEnergy;

  if ( !fFastMode ) {
    int i;
    for ( i=0; i<MATRIXSIZE; i++ ) { fErrP7[i] = o.fErrP7[i]; }
  }

  for ( int i=0; i<o.fNDaug; i++ ) {
    fDaughters[i] = o.fDaughters[i];
  }

  fNCons = 0;
//  if (o.nCons > 0) {
//    for (int i=0;i<o.nCons;i++) AddConstraint(*o.fConstraints[i]);
//  }

  for ( int i=0; i<30; i++ ) {
    fPidLH[i]=o.fPidLH[i];
  }

  fMcTruth=o.fMcTruth;
  SetInsertHistory(kFALSE);
  AddLinks(o.GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack")));

  //FIXME Do we want to carry these status flags?
  fChi2 = 0 ;
  fFit = 0 ;
}

// This is the special constructor to bring a RhoCandidate into
// life from the MicroCandidate

RhoCandidate::RhoCandidate ( FairRecoCandidate& a, Int_t n) :
  fFastMode ( kFALSE ),
  fLocked ( kFALSE ),
  fTheMother ( 0 ),
  fDecayVtx (  ), //default?
  fPdtEntry ( 0 ),
  fPdgCode ( 0 ),
  fIsAResonance ( kFALSE ),
  //      fTruth ( 0 ),
  fMicroCand ( &a ),
  fTrackNumber ( -1 ),
  fNDaug ( 0 ),
  //fDaugList ( 0 ),
  fNCons ( 0 ),
  fChi2 ( 0 ),
  fMcTruth ( 0 ),
  fFit ( 0 )
{
  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
  // Set kinematics from MicroCandidate

  SetP4 ( a.GetLorentzVector() );
  SetPos ( a.GetPosition() );
  SetCharge ( a.GetCharge() );

  if ( !fFastMode ) {
    const Float_t* err = a.GetErrorP7(); // 4 momentum & point
    //const Float_t* cov = a.GetCov(); // helix cov
    //const Float_t* par = a.GetParams(); // helix params
    int i;
    if ( err!=0 ) for ( i=0; i<MATRIXSIZE; i++ ) { fErrP7[i] = err[i]; }
  }

  SetUid ( n );
  SetInsertHistory(kFALSE);
  AddLinks(a.GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack")));

  SetPidInfo ( 0 );
  SetPidInfo ( 0,a.GetElectronPidLH() );
  SetPidInfo ( 1,a.GetMuonPidLH() );
  SetPidInfo ( 2,a.GetPionPidLH() );
  SetPidInfo ( 3,a.GetKaonPidLH() );
  SetPidInfo ( 4,a.GetProtonPidLH() );
//  SetMcTruth ( a.GetMcTruth() ); // set the MCTuth object when building the mc list
}


RhoCandidate::RhoCandidate ( FairRecoCandidate& a, Int_t n, RhoVector3Err& vp, Bool_t fast ) :
  fFastMode ( fast ),
  fLocked ( kFALSE ),
  fTheMother ( 0 ),
  fDecayVtx ( vp ),
  fPdtEntry ( 0 ),
  fPdgCode ( 0 ),
  fIsAResonance ( kFALSE ),
  //      fTruth ( 0 ),
  fMicroCand ( &a ),
  fTrackNumber ( -1 ),
  fNDaug ( 0 ),
  //fDaugList ( 0 ),
  fNCons ( 0 ),
  fChi2 ( 0 ),
  fMcTruth ( 0 ),
  fFit ( 0 )
{
  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
  // Set kinematics from MicroCandidate

  SetP4 ( a.GetLorentzVector() );
  SetPos ( a.GetPosition() );
  SetCharge ( a.GetCharge() );

  if ( !fFastMode ) {
    const Float_t* err = a.GetErrorP7(); // 4 momentum & point
    //const Float_t* cov = a.GetCov(); // helix cov
    //const Float_t* par = a.GetParams(); // helix params
    int i;
    if ( err!=0 ) for ( i=0; i<MATRIXSIZE; i++ ) { fErrP7[i] = err[i]; }
  }

  SetUid ( n );
  SetInsertHistory(kFALSE);
  AddLinks(a.GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack")));

  SetPidInfo ( 0 );
  SetPidInfo ( 0,a.GetElectronPidLH() );
  SetPidInfo ( 1,a.GetMuonPidLH() );
  SetPidInfo ( 2,a.GetPionPidLH() );
  SetPidInfo ( 3,a.GetKaonPidLH() );
  SetPidInfo ( 4,a.GetProtonPidLH() );

//  SetMcTruth ( a.GetMcTruth() ); // set the MCTuth object when building the mc list
}



//--------------
// Destructor --
//--------------

RhoCandidate::~RhoCandidate( )
{
  //RemoveAssociations();
}

//--------------
// Operations --
//--------------

// assignment double
RhoCandidate&
RhoCandidate::operator = ( const RhoCandidate& o )
{
  FairMultiLinkedData_Interface::operator=(o);
  fFastMode = o.fFastMode;
  fLocked = kFALSE;
  fTheMother = 0;
  fDecayVtx = o.fDecayVtx;
  fPdtEntry = o.fPdtEntry;
  fPdgCode  = o.fPdgCode;
  fIsAResonance = o.fIsAResonance;
//    fTruth = o.fTruth;
  fTrackNumber = o.fTrackNumber;
  fUid = o.fUid;
  fNDaug = o.fNDaug;
  fMicroCand = o.fMicroCand;

  fMarker[0] = o.fMarker[0];
  fMarker[1] = o.fMarker[1];
  fMarker[2] = o.fMarker[2];
  fMarker[3] = o.fMarker[3];

  fCharge    = o.fCharge;
  fXposition = o.fXposition;
  fYposition = o.fYposition;
  fZposition = o.fZposition;
  fXmomentum = o.fXmomentum;
  fYmomentum = o.fYmomentum;
  fZmomentum = o.fZmomentum;
  fEnergy    = o.fEnergy;

  if ( !fFastMode ) {
    int i;
    for ( i=0; i<MATRIXSIZE; i++ ) { fErrP7[i] = o.fErrP7[i]; }
  }

  for ( int i=0; i<o.fNDaug; i++ ) {
    fDaughters[i] = o.fDaughters[i];
  }

  fNCons = 0;
//  if (o.nCons > 0) {
//    for (int i=0;i<o.nCons;i++) AddConstraint(*o.fConstraints[i]);
//  }

  for ( int i=0; i<30; i++ ) {
    fPidLH[i]=o.fPidLH[i];
  }

  fMcTruth=o.fMcTruth;

  return *this;
}



void
RhoCandidate::SetMass ( Double_t m )
{
  SetP4 ( m, P3() );
}

void
RhoCandidate::SetMassAndEnergy ( Double_t mass, Double_t mEnergy )
{
  Double_t oldP = P();
  assert ( oldP>0 );
  Double_t P2 = pow ( mEnergy, 2 ) - pow ( mass, 2 );
  assert ( P2>0 );
  Double_t newP = sqrt ( P2 );
  fXmomentum = fXmomentum*newP/oldP;
  fYmomentum = fYmomentum*newP/oldP;
  fZmomentum = fZmomentum*newP/oldP;
  fEnergy    = mEnergy;
}

void
RhoCandidate::SetPosition ( const TVector3& pos )
{
  fXposition = pos.X();
  fYposition = pos.Y();
  fZposition = pos.Z();
}

void
RhoCandidate::SetP3 ( const TVector3& p3 )
{
  // assumes mass is well defined
  Double_t mass = M();
  SetP4 ( mass, p3 );
}

void
RhoCandidate::SetP4 ( Double_t mass, const TVector3& p3 )
{
  fXmomentum = p3.X();
  fYmomentum = p3.Y();
  fZmomentum = p3.Z();
  Double_t En = sqrt ( mass*mass + p3.Mag2() );
  //assert( En>0 );
  fEnergy = En;
}

void
RhoCandidate::SetP4 ( const TLorentzVector& p4 )
{
  fXmomentum = p4.X();
  fYmomentum = p4.Y();
  fZmomentum = p4.Z();
  fEnergy    = p4.T();
}

void
RhoCandidate::SetP7 ( const TVector3& pos, const TLorentzVector& p4 )
{
  SetPos ( pos );
  SetP4 ( p4 );
}

void RhoCandidate::SetCovPos ( const TMatrixD& errPos )
{
  //Int_t ind = 0;
  //  for(int i=0;i<3; i++)
  //      for(int j=i;j<3;j++)fErrP7[ind++] = errPos[j][i];
  fErrP7[0] = errPos[0][0];
  fErrP7[1] = errPos[1][0];
  fErrP7[2] = errPos[1][1];
  fErrP7[3] = errPos[2][0];
  fErrP7[4] = errPos[2][1];
  fErrP7[5] = errPos[2][2];
}

void
RhoCandidate::SetCov7 ( const TMatrixD& cov7 )
{
  // position error

  fErrP7[0] = cov7 ( 0,0 );
  fErrP7[1] = cov7 ( 1,0 );
  fErrP7[2] = cov7 ( 1,1 );
  fErrP7[3] = cov7 ( 2,0 );
  fErrP7[4] = cov7 ( 2,1 );
  fErrP7[5] = cov7 ( 2,2 );

  // position-momentum covariance

  fErrP7[6] = cov7 ( 3,0 );
  fErrP7[7] = cov7 ( 3,1 );
  fErrP7[8] = cov7 ( 3,2 );
  fErrP7[9] = cov7 ( 4,0 );
  fErrP7[10] = cov7 ( 4,1 );
  fErrP7[11] = cov7 ( 4,2 );
  fErrP7[12] = cov7 ( 5,0 );
  fErrP7[13] = cov7 ( 5,1 );
  fErrP7[14] = cov7 ( 5,2 );
  fErrP7[15] = cov7 ( 6,0 );
  fErrP7[16] = cov7 ( 6,1 );
  fErrP7[17] = cov7 ( 6,2 );

  // momentum error
  fErrP7[18] = cov7 ( 3,3 );
  fErrP7[19] = cov7 ( 4,3 );
  fErrP7[20] = cov7 ( 4,4 );
  fErrP7[21] = cov7 ( 5,3 );
  fErrP7[22] = cov7 ( 5,4 );
  fErrP7[23] = cov7 ( 5,5 );
  fErrP7[24] = cov7 ( 6,3 );
  fErrP7[25] = cov7 ( 6,4 );
  fErrP7[26] = cov7 ( 6,5 );
  fErrP7[27] = cov7 ( 6,6 );
}

void
RhoCandidate::SetCov7 ( const TMatrixD& covPos, const TMatrixD& covP4 )
{
  // position error

  fErrP7[0] = covPos ( 0,0 );
  fErrP7[1] = covPos ( 1,0 );
  fErrP7[2] = covPos ( 1,1 );
  fErrP7[3] = covPos ( 2,0 );
  fErrP7[4] = covPos ( 2,1 );
  fErrP7[5] = covPos ( 2,2 );

  // position-momentum covariance

  fErrP7[6] = 0;
  fErrP7[7] = 0;
  fErrP7[8] = 0;
  fErrP7[9] = 0;
  fErrP7[10] = 0;
  fErrP7[11] = 0;
  fErrP7[12] = 0;
  fErrP7[13] = 0;
  fErrP7[14] = 0;
  fErrP7[15] = 0;
  fErrP7[16] = 0;
  fErrP7[17] = 0;

  // momentum error
  fErrP7[18] = covP4 ( 0,0 );
  fErrP7[19] = covP4 ( 1,0 );
  fErrP7[20] = covP4 ( 1,1 );
  fErrP7[21] = covP4 ( 2,0 );
  fErrP7[22] = covP4 ( 2,1 );
  fErrP7[23] = covP4 ( 2,2 );
  fErrP7[24] = covP4 ( 3,0 );
  fErrP7[25] = covP4 ( 3,1 );
  fErrP7[26] = covP4 ( 3,2 );
  fErrP7[27] = covP4 ( 3,3 );
}

void
RhoCandidate::SetCov7 ( const TMatrixD& covPos, const TMatrixD& covP4, const TMatrixD& covPosP4 )
{
  // position error

  fErrP7[0] = covPos ( 0,0 );
  fErrP7[1] = covPos ( 1,0 );
  fErrP7[2] = covPos ( 1,1 );
  fErrP7[3] = covPos ( 2,0 );
  fErrP7[4] = covPos ( 2,1 );
  fErrP7[5] = covPos ( 2,2 );

  // position-momentum covariance

  fErrP7[6] = covPosP4 ( 0,0 );
  fErrP7[7] = covPosP4 ( 0,1 );
  fErrP7[8] = covPosP4 ( 0,2 );
  fErrP7[9] = covPosP4 ( 1,0 );
  fErrP7[10] = covPosP4 ( 1,1 );
  fErrP7[11] = covPosP4 ( 1,2 );
  fErrP7[12] = covPosP4 ( 2,0 );
  fErrP7[13] = covPosP4 ( 2,1 );
  fErrP7[14] = covPosP4 ( 2,2 );
  fErrP7[15] = covPosP4 ( 3,0 );
  fErrP7[16] = covPosP4 ( 3,1 );
  fErrP7[17] = covPosP4 ( 3,2 );

  // momentum error
  fErrP7[18] = covP4 ( 0,0 );
  fErrP7[19] = covP4 ( 1,0 );
  fErrP7[20] = covP4 ( 1,1 );
  fErrP7[21] = covP4 ( 2,0 );
  fErrP7[22] = covP4 ( 2,1 );
  fErrP7[23] = covP4 ( 2,2 );
  fErrP7[24] = covP4 ( 3,0 );
  fErrP7[25] = covP4 ( 3,1 );
  fErrP7[26] = covP4 ( 3,2 );
  fErrP7[27] = covP4 ( 3,3 );
}

void
RhoCandidate::SetCovP4 ( const TMatrixD& covP4 )
{
  // position error

  fErrP7[0] = 0;
  fErrP7[1] = 0;
  fErrP7[2] = 0;
  fErrP7[3] = 0;
  fErrP7[4] = 0;
  fErrP7[5] = 0;

  // position-momentum covariance

  fErrP7[6] = 0;
  fErrP7[7] = 0;
  fErrP7[8] = 0;
  fErrP7[9] = 0;
  fErrP7[10] = 0;
  fErrP7[11] = 0;
  fErrP7[12] = 0;
  fErrP7[13] = 0;
  fErrP7[14] = 0;
  fErrP7[15] = 0;
  fErrP7[16] = 0;
  fErrP7[17] = 0;

  // momentum error
  fErrP7[18] = covP4 ( 0,0 );
  fErrP7[19] = covP4 ( 1,0 );
  fErrP7[20] = covP4 ( 1,1 );
  fErrP7[21] = covP4 ( 2,0 );
  fErrP7[22] = covP4 ( 2,1 );
  fErrP7[23] = covP4 ( 2,2 );
  fErrP7[24] = covP4 ( 3,0 );
  fErrP7[25] = covP4 ( 3,1 );
  fErrP7[26] = covP4 ( 3,2 );
  fErrP7[27] = covP4 ( 3,3 );
}


void
RhoCandidate::Set ( const TVector3& pos,
                    const TLorentzVector& p4,
                    const TMatrixD& cov7 )
{
  SetP7 ( pos, p4 );
  SetCov7 ( cov7 );
}

void
RhoCandidate::Set ( Double_t mass,
                    const RhoVector3Err& posErr,
                    const RhoVector3Err& p3Err,
                    const TMatrixD& xpErr )
{
  SetPos ( posErr );
  SetP4 ( mass, p3Err );

  // compute the jacobian, assuming that mass is a well defined quantity
  static TMatrixD jacobian ( 4, 3 );
  jacobian ( 0,0 ) = 1.0;
  jacobian ( 0,1 ) = 0.0;
  jacobian ( 0,2 ) = 0.0;
  jacobian ( 1,0 ) = 0.0;
  jacobian ( 1,1 ) = 1.0;
  jacobian ( 1,2 ) = 0.0;
  jacobian ( 2,0 ) = 0.0;
  jacobian ( 2,1 ) = 0.0;
  jacobian ( 2,2 ) = 1.0;
  jacobian ( 3,0 ) = fXmomentum/fEnergy;
  jacobian ( 3,1 ) = fYmomentum/fEnergy;
  jacobian ( 3,2 ) = fZmomentum/fEnergy;
  static TMatrixD xp4Cov ( 3,4 );
  for ( int i=0; i<3; i++ ) {
    xp4Cov ( i,3 ) = 0;
    for ( int j=0; j<3; j++ ) {
      xp4Cov ( i,j )  = xpErr ( i,j );
      xp4Cov ( i,3 ) += xpErr ( i,j ) *jacobian ( 3,j );
    }
  }

  // Sets the covariance matrix
  SetCov7 ( posErr.CovMatrix(), p3Err.CovMatrix().Similarity ( jacobian ),xp4Cov );
}






void
RhoCandidate::SetMotherLink ( RhoCandidate* m , bool verbose)
{
  assert ( m!=0 ); // what kind of mother is it ?!

  // we have already a mother ?  not allowed !
  assert ( fTheMother==0 );

  // should not be called for a local candidate...
  //assert( !isLocal() );

  // commented by KG, 2/2012
  //  if (m->nDaug>=5) {
  //  cerr << "RhoCandidate::SetMotherLink: Can not add more than 5 daughters." << endl;
  //  return;
  //}

  // set the mother link
  fTheMother = m;

  // ... and the mother's daughter link
  //fTheMother->fDaughters.push_back(this);
  fTheMother->AddDaughterLinkSimple ( this , verbose);
  // No markers set. MC candidates will not be "combined".

  // special for MC trees
  if ( fTheMother==0 ) { fTheMother=m; }

  // if it's a resonance and the mother has already a decay vertex,
  // set the decay vertex of the mother to the daughter
  if ( IsAResonance()) {
    SetDecayVtx ( fTheMother->DecayVtx() );
    // if it is a resonance and the mother has no other vertex, the vertex of the reonance is given to her.
  }

}

void
RhoCandidate::DropMotherLink()
{
  // no mother
  if ( fTheMother==0 ) { return; }

  // the mother looses this as a daughter
  for ( int i=0; i<fTheMother->NDaughters(); i++ )
  {
    if ( fTheMother->fDaughters[i] == this ) {
      fTheMother->RemoveDaughter(this);
      break;
    }
  }
  fTheMother = 0;
}


// void
// RhoCandidate::AddToVertexingList ( RhoCandList& outGoing )
// {
//     if ( IsAResonance() )
//     {
//         for ( int i=0;i<nDaug;i++ ) fDaughters[i]->AddToVertexingList ( outGoing );
//     }
//     else
//         outGoing.Put ( *this );
// }

const RhoCandidate*
RhoCandidate::CloneInTree ( const RhoCandidate& c ) const
{
  if ( IsCloneOf ( c ) ) { return this; }
  for ( int i=0; i<NDaughters(); i++ ) {
    const RhoCandidate* b = fDaughters[i]->CloneInTree ( c );
    if ( b!=0 ) { return b; }
  }
  return 0;
}


void
RhoCandidate::InvalidateFit()
{
  // new function (GHM 05/99)
  // this function invalidates the fit to let fitters know
  // that if given this candidate, they have to refit it.

//     TVector3* theDecayVtx = DecayVtx();
//     if ( theDecayVtx!=0 )
//     {
//         if ( theDecayVtx->Status() != VAbsVertex::UnFitted )
//         {
//             // clone the vertex...
//             VAbsVertex* newDecayVtx = ( VAbsVertex* ) theDecayVtx->Clone();
//
//             // ...and invalid the clone
//             newDecayVtx->SetStatus ( VAbsVertex::UnFitted );
//
//             // now set the new decay vertex
//             SetDecayVtx ( newDecayVtx );
//         }
//     }

  // and do that recursively
  for ( int i=0; i<NDaughters(); i++ ) { fDaughters[i]->InvalidateFit(); }
}


void RhoCandidate::Boost ( Double_t bx, Double_t by, Double_t bz )
{
  Double_t b2 = bx*bx + by*by + bz*bz;
  Double_t gamma = 1.0 / sqrt ( 1.0 - b2 );
  Double_t bp = bx*fXmomentum + by*fYmomentum + bz*fZmomentum;
  Double_t gamma2 = b2 > 0 ? ( gamma - 1.0 ) /b2 : 0.0;

  fXmomentum = fXmomentum + gamma2*bp*bx + gamma*bx*fEnergy;
  fYmomentum = fYmomentum + gamma2*bp*by + gamma*by*fEnergy;
  fZmomentum = fZmomentum + gamma2*bp*bz + gamma*bz*fEnergy;
  fEnergy = gamma* ( fEnergy + bp );
}


std::ostream&
operator << ( std::ostream& o, const RhoCandidate& a )
{
  a.PrintOn ( o );
  return o;
}

// const RhoVector3Err* RhoCandidate::ProductionVtx() const
// {
//   if ( fTheMother!=0 ) { return fTheMother->DecayVtx(); }
//   else { return 0; }
// }

Bool_t RhoCandidate::operator== ( const RhoCandidate* c ) const
{
  return Equals ( c ) ;
}

Bool_t RhoCandidate::operator!= (  RhoCandidate* c ) const
{
  // return ( (*this)!=(*c) ) ;
  return !Equals(c);
}

// This comes from the content



void
RhoCandidate::SetEnergy ( Double_t mEnergy )
{
  if ( IsComposite() ) {
    // this implementation leaves P unchanged; subclasses may differ
    fEnergy =  mEnergy;
    //RhoCandidate::SetEnergy ( mEnergy );
  } else {
    SetMassAndEnergy ( Mass(), mEnergy );
  }
}

void
RhoCandidate::SetMomentum ( Double_t newp )
{
  // this implementation leaves mass unchanged; subclasses may differ
  Double_t pscale = ( P() == 0 ? 0 : newp / P() );
  SetP3 ( pscale*P3() );
}

void
RhoCandidate::SetType ( int pdgcode )
{
  fPdgCode=pdgcode;
  TDatabasePDG* pdg = TDatabasePDG::Instance();   // Access particle DB
  TParticlePDG* pdt=pdg->GetParticle ( pdgcode );
  if ( pdt ) { SetType ( pdt ); }
  //else Warning("RhoCandidate::SetType","unknown pdg code %i",pdgcode);
}

void
RhoCandidate::SetType ( const char* name )
{
  TDatabasePDG* pdg = TDatabasePDG::Instance();   // Access particle DB
  TParticlePDG* pdt=pdg->GetParticle ( name );
  if ( pdt ) { SetType ( pdt ); }
  else Warning("RhoCandidate::SetType","unknown particle \"%s\"",name);
}

void
RhoCandidate::SetType ( const TParticlePDG* pdt )
{
  if ( pdt==fPdtEntry || pdt==0 ) { return; }
  fPdtEntry = pdt;
  fPdgCode = pdt->PdgCode();

  //
  // by default :
  //   if the proper lifetime multiplied by light velocity is less
  //    than a nanometer, the candidate is considered a resonance
  //      (a state that does not fly)
  //
  fIsAResonance=kFALSE;
  if ( fPdtEntry->Width() >1E-15 /*Lifetime()<1e-08*/ ) { fIsAResonance=kTRUE; }

  if ( !IsComposite() ) {

    // the mass has changed since the type has changed
    //SetMass ( Mass() );
    SetMass(fPdtEntry->Mass());
    // set the charge
    SetCharge ( pdt->Charge()/3. ); // TParticlePDG contains charge in units of |e|/3
  } else {
    if ( Charge() !=pdt->Charge()/3. ) {
      cerr
          << "ERROR: attempt to call RhoCandidate::SetType(\""
          << pdt->GetName()<<" | "<<pdt->ParticleClass() <<" | Q=" << pdt->Charge()/3.<< "\") for a composite" << endl
          << "       RhoCandidate whose daughters have total charge "
          << Charge() << endl;
      //assert( Charge()==pdt->Charge() );
    }
  }
}


void
RhoCandidate::SetDecayVtx ( RhoVector3Err  theVtx )
{

  // protection against null pointers
  //if ( theVtx==0 ) { return; }

  //
  // Warning : this is a recursive algorithm
  //

  // this vertex is not already set
  //if ( fDecayVtx!=theVtx ) {
    // set the new decay vertex reference
    fDecayVtx = theVtx;

 // } else {
 //   cerr << "trying to reset the same vertex ! " << endl;
    // this is used to refresh the vertex links
    //theVtx->_inComingCand=0;
    //theVtx->_outGoingCands.clear();
    //theVtx->_resonances.clear();
 // }

  // update the vertex back links
  /*   if ( IsAResonance() )
         theVtx->SetResonances ( this );
     else
     {
         //assert( theVtx->GetInComingCand()==0 );
         theVtx->SetInComingCand ( this );
     }
     // now loop on daughters
     for ( int i=0;i<nDaug;i++ )
     {
         if ( fDaughters[i]->IsAResonance() )
             fDaughters[i]->SetDecayVtx ( theVtx );
         else
             theVtx->SetOutGoingCands ( fDaughters[i] );
     }*/
}

// --------------------------------------------------
// Geneology functions, no longer in a separate class
// --------------------------------------------------

Int_t
RhoCandidate::NDaughters() const
{
  return fNDaug;
}

// void
// RhoCandidate::AddDaughterLink ( const RhoCandidate* cand )
// {
//   //assert( cand!=0 );
//
//   // first copy the candidate pointer
//   RhoCandidate* d = const_cast<RhoCandidate*> ( cand );
//
//   // as soon as there are daughters, the charge is
//   // given by the sum of the daughter charges
//   if ( NDaughters()==0 ) { SetCharge ( 0 ); }
//   SetCharge ( Charge() +cand->Charge() );
//
//   // set the daughter's mother link
//   // ******** modified K Goetzen
//   d->SetMotherLink ( this );
//
//   fMarker[0] |= d->GetMarker ( 0 );
//   fMarker[1] |= d->GetMarker ( 1 );
//   fMarker[2] |= d->GetMarker ( 2 );
//   fMarker[3] |= d->GetMarker ( 3 );
// }

void
RhoCandidate::AddDaughterLinkSimple ( const RhoCandidate* cand , bool verbose)
{
  //assert( cand!=0 );

  // first copy the candidate pointer
  RhoCandidate* d = const_cast<RhoCandidate*> ( cand );
  // as soon as there are daughters, the charge is
  // given by the sum of the daughter charges
  if ( NDaughters()==0 ) { SetCharge ( 0 ); }
  SetCharge ( Charge() +cand->Charge() );

  if (NDaughters()>=MAXDAUG) {
/*   if(verbose)
   {
		cerr << "RhoCandidate::AddDaughterLinkSimple: Can not add more than "<<MAXDAUG<<" daughters." << endl;
		cout <<PdgCode()<<" -> ";
		for (int i=0;i<fNDaug;++i) cout <<Daughter(i)->PdgCode()<<" ";
		cout <<"; want to add "<< d->PdgCode() <<endl;
   }*/
    return;
  }

  fDaughters[fNDaug++] = d;
  //fDaughters.push_back(d);

  // set the daughter's mother link
  // ******** modified K Goetzen
  //d->SetMotherLink(this);

  //fMarker[0] |= d->GetMarker(0);
  //fMarker[1] |= d->GetMarker(1);
  //fMarker[2] |= d->GetMarker(2);
  //fMarker[3] |= d->GetMarker(3);
}

void
RhoCandidate::RemoveDaughter ( RhoCandidate* d )
{
  // check for consistency
  assert ( d!=0 && d->TheMother() ==this );

  // the charge
  SetCharge ( Charge() - d->Charge() );

  for(int i=0;i<NDaughters()-1;i++){
    if(fDaughters[i]==d){
      //put last element to a safe place, daughter order is screwed
      fDaughters[i]=fDaughters[NDaughters()-1];
      break;
    }
  }
  fNDaug--;
  //fDaughters.pop_back(); //remove last element
  // destroy the daughter
  //delete d;

}


//
// Access functions
//

Double_t
RhoCandidate::Mass() const
{
  if ( !IsComposite() && fPdtEntry!=0 ) {
    return fPdtEntry->Mass();
  } else {
    return M();
  }
}

RhoCandidate*
RhoCandidate::Daughter ( Int_t n )
{
  if ( n >=0 && n < fNDaug ) {
    return fDaughters[n];
  } else {
    return 0;
  }
}

// RhoCandListIterator
// RhoCandidate::DaughterIterator() const
// {
//   static RhoCandList emptyList;
//   static RhoCandListIterator empty ( emptyList );
//   if ( NDaughters() ==0 ) { return empty; }
//
//   // return an iterator to the daughter list
//   RhoCandList* l = const_cast<RhoCandList*> ( fDaugList );
//   if ( l==0 ) { l = new RhoCandList ( "DaugList",NDaughters() ); }
//   l->Cleanup();
//   for ( int i=0; i<NDaughters(); i++ ) { l->Put ( *fDaughters[i] ); }
//   return RhoCandListIterator ( *l );
// }

// TConstraint&
// RhoCandidate::AddConstraint( TConstraint& oneMore )
// {
//   TConstraint *newConstraint = new TConstraint(oneMore);
//   fConstraints[nCons++] = newConstraint;
//   return *newConstraint;
// }
//
// TConstraint&
// RhoCandidate::AddConstraint( TConstraint::Type  type )
// {
//   TConstraint oneMore( type );
//   return AddConstraint( oneMore );
// }
//
// void
// RhoCandidate::RemoveConstraint( const TConstraint& oneLess )
// {
// }
//
// void
// RhoCandidate::RemoveConstraint( TConstraint::Type type )
// {
//   TConstraint oneLess(type);
//   RemoveConstraint( oneLess );
// }
//
//
// Int_t
// RhoCandidate::NConstraints() const
// {
//   return nCons;
// }
//
// const TConstraint*
// RhoCandidate::Constraint( Int_t i )
// {
//   if(i>=nCons) return 0;
//   return fConstraints[i];
// }
//
// const TConstraint*
// RhoCandidate::Constraint( TConstraint::Type type ) const
// {
//   Short_t i;
//   for( i=0; i<nCons; i++ )
//   {
//     if( type==fConstraints[i]->GetType() ) return fConstraints[i];
//   }
//   return 0;
// }

// former inline implementations

// RhoVector3Err*
// RhoCandidate::ProductionVtx()
// {
//   if ( fTheMother!=0 ) { return fTheMother->DecayVtx(); }
//   else { return 0; }
// }
//
// const RhoVector3Err*
// RhoCandidate::DecayVtx() const
// {
//   return fDecayVtx;
// }
//
// RhoVector3Err*
// RhoCandidate::DecayVtx()
// {
//   return fDecayVtx;
// }

const TParticlePDG*
RhoCandidate::PdtEntry() const
{
  return fPdtEntry;
}


Bool_t
RhoCandidate::IsComposite() const
{
  return NDaughters()>0;
}

Bool_t
RhoCandidate::IsAResonance() const
{
  return fIsAResonance;
}

void
RhoCandidate::SetFly()
{
  if ( !fIsAResonance ) { return; }
  else {
    // can only change to new value is a vertex not already present
    //assert( fDecayVtx==0 );
    fIsAResonance = kFALSE;
  }
}

void
RhoCandidate::SetNoFly()
{
  if ( fIsAResonance ) { return; }
  else {
    // can only change to new value is a vertex not already present
    //assert( fDecayVtx==0 );
    fIsAResonance = kTRUE;
  }
}

TVector3
RhoCandidate::Origin() const
{
  return Pos();
}


void RhoCandidate::PrintOn ( std::ostream& o ) const
{
  o <<"uid:"<<Uid() <<" trk:"<<GetTrackNumber() << " (" << P4().X() << "," << P4().Y() << "," << P4().Z() << ";" << P4().T() <<  ") " << Charge() <<"e" << " " << Mass() << " GeV/c2 ";
  o.setf ( ios::hex | ios::showbase );
  o << " ("
    << GetMarker ( 3 )
    << ","
    << GetMarker ( 2 )
    << ","
    << GetMarker ( 1 )
    << ","
    << GetMarker ( 0 )
    << ") ";
  o.unsetf ( ios::hex | ios::showbase );

  // print daughter links
  o << "daughters: "<< NDaughters() <<"(";
  for ( int i=0; i<NDaughters(); i++ ) { o << " " << fDaughters[i]->Uid(); }
  o <<")";
  //o << " dcy: "<<fDecayVtx;
  o << " pdg: " <<fPdgCode;
  o << " PID:";
  for ( int k=0; k<5; k++ ) { o << fPidLH[k] <<","; } // take the first 5 pid entries to check charged p,pi,e,mu,K
  o << "  mc truth pointer: " << fMcTruth << " ";
  FairMultiLinkedData_Interface::Print(o);
}


void RhoCandidate::SetMarker ( UInt_t n )
{

  fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;

  if ( n<32 ) {
    fMarker[0] = 1<<n;
  } else if ( n<64 ) {
    fMarker[1] = 1<< ( n%32 );
  } else if ( n<96 ) {
    fMarker[2] = 1<< ( n%64 );
  } else if ( n<128 ) {
    fMarker[3] = 1<< ( n%96 );
  } else {
    cout << "RhoCandidate warning: More than 128 tracks; overlap may occur" << endl;
  }
}

// Constructor from CandBase

// RhoCandidate::RhoCandidate ( TLorentzVector p4,
//                              RhoError& p4Err,
//                              RhoCandListIterator& iterDau,
//                              RhoVector3Err& theVertex,
//                              const TParticlePDG* hypo )
//   :
//   fFastMode ( kFALSE ),
//   fLocked ( kFALSE ),
//   fTheMother ( 0 ),
//   fDecayVtx ( 0 ),
//   fPdtEntry ( 0 ),
//   fPdgCode ( 0 ),
//   fIsAResonance ( kFALSE ),
// //        fTruth ( 0 ),
//   fMicroCand ( 0 ),
//   fTrackNumber ( -1 ),
//   //fDaugList ( 0 ),
//   fNCons ( 0 )
// {
//   fMarker[0] = fMarker[1] = fMarker[2] = fMarker[3] = 0;
//
//   // create the local candidate
//   //createLocalCand();
//
//   // first set the mother/daughter links
//   RhoCandidate* dau=0;
//   iterDau.Rewind();
//   Int_t nDau=0;
//   while ( dau=iterDau.Next() ) {
//     nDau++;
//     AddDaughterLink ( dau );
//   }
//
//   // a composite cand is not supposed not to have a vertex...
//   if ( nDau==0 ) {
//     cerr << "A composite cand is supposed to have daughters ! " << endl;
//   }
//
//   // set the trajector
//   SetTrajectory ( p4,p4Err, ( Int_t ) Charge(),hypo,&theVertex );
// }


// void RhoCandidate::SetTrajectory ( const TLorentzVector& p4, const RhoError& p4Err,
//                                    Int_t charge,const TParticlePDG* hypo,
//                                    RhoVector3Err dVtx )
// {
//   if ( dVtx ) {
//     SetP7 ( ((TVector3*)dVtx),p4 );
//     //SetCov7 ( dVtx->XXCov(),p4Err,dVtx->XPCov() );
//     SetCov7 ( dVtx.CovMatrix(),p4Err );
//     //BbrPointErr pos(dVtx->point(),dVtx->xxCov());
//     //_deferCompTrk = true;
//
//     //     HepSymMatrix ppc(3);
//     //     for(Int_t k=0;k<3;k++)
//     //       for (Int_t j=k;j<3;j++)
//     //  ppc[j][k]=p4Err[j][k];
//     //     BbrVectorErr mom(p4.vect(),ppc);
//
//     //     TrkCompTrk * compTrk=new TrkCompTrk( pos,mom,dVtx->xpCov(),charge,
//     //                   dVtx->chiSquared(),dVtx->nDof());
//     //     setTrkCompTrk(compTrk);
//
//     SetDecayVtx ( dVtx );
//   } else {
//     SetP4 ( p4 );
//     SetCovP4 ( p4Err );
//   }
//
//   if ( hypo ) { SetType ( hypo ); }
//   if ( Uid() ==0 ) { SetUid(); }
// }

void RhoCandidate::SetUid ( UInt_t uid )
{
  static UInt_t u = 10000;
  if ( uid!=0 ) {
    u = uid;
  } else {
    u++;
  }

  fUid = u;
  if ( ( fMarker[0]|fMarker[1]|fMarker[2]|fMarker[3] ) ==0 ) { SetMarker ( u%128 ); }
}

Bool_t
RhoCandidate::IsCloneOf ( const RhoCandidate& o, Bool_t checkType ) const
{
  // Original behaviour of RhoCandidate::IsCloneOf()
  if ( Uid() == o.Uid() && !checkType ) { return kTRUE; }

  if ( ( IsComposite() && !o.IsComposite() ) ||
       ( !IsComposite() && o.IsComposite() ) ) { return kFALSE; }

  // for single tracks and clusters, it is enough to compare
  // UIDs and PDT types
  if ( !IsComposite() && !o.IsComposite() ) {
    return ( Uid() == o.Uid() &&
             ( !checkType || PdtEntry() == o.PdtEntry() ) );
  }

  // if we got here, must be true
  //return kTRUE;
  return kFALSE;
}

void RhoCandidate::RemoveAssociations()
{
  // Clean up asociations to allow new w/ placement
  //if (fTheMother!=0) DropMotherLink(); fTheMother = 0;

  fTheMother=0; //make sure to drop associations only here.
  fNDaug=0;

  // ************************

  //if (fDecayVtx!=0) { delete fDecayVtx; fDecayVtx = 0; }
// for (int i=0;i<nCons;i++) { delete fConstraints[i]; fConstraints[i]=0; nCons=0;}
}

RhoCandidate* RhoCandidate::Combine ( RhoCandidate* c )
{
  RhoCandidate tmp ( P4() +c->P4(),Charge() +c->Charge() );
  RhoCandidate* cand = RhoFactory::Instance()->NewCandidate ( tmp );
  cand->SetMarker ( fMarker[0]|c->fMarker[0],0 );
  cand->SetMarker ( fMarker[1]|c->fMarker[1],1 );
  cand->SetMarker ( fMarker[2]|c->fMarker[2],2 );
  cand->SetMarker ( fMarker[3]|c->fMarker[3],3 );

  cand->SetCovP4 ( P4Cov() +c->P4Cov() );

  //Only one-way link because we're not sure where else the daughters are used (combinatorics)
  cand->AddDaughterLinkSimple(this);
  cand->AddDaughterLinkSimple(c);

  return cand;
}

//************** added Combine for more candidates K.Goetzen, 05/2008
RhoCandidate* RhoCandidate::Combine ( RhoCandidate* c1, RhoCandidate* c2 )
{
  RhoCandidate tmp ( P4() +c1->P4() +c2->P4(),Charge() +c1->Charge() +c2->Charge() );
  RhoCandidate* cand = RhoFactory::Instance()->NewCandidate ( tmp );
  cand->SetMarker ( fMarker[0]|c1->fMarker[0]|c2->fMarker[0],0 );
  cand->SetMarker ( fMarker[1]|c1->fMarker[1]|c2->fMarker[1],1 );
  cand->SetMarker ( fMarker[2]|c1->fMarker[2]|c2->fMarker[2],2 );
  cand->SetMarker ( fMarker[3]|c1->fMarker[3]|c2->fMarker[3],3 );

  cand->SetCovP4 ( P4Cov() +c1->P4Cov() +c2->P4Cov() );

  //Only one-way link because we're not sure where else the daughters are used (combinatorics)
  cand->AddDaughterLinkSimple(this);
  cand->AddDaughterLinkSimple(c1);
  cand->AddDaughterLinkSimple(c2);

  return cand;
}

RhoCandidate* RhoCandidate::Combine ( RhoCandidate* c1, RhoCandidate* c2, RhoCandidate* c3 )
{
  RhoCandidate tmp ( P4() +c1->P4() +c2->P4() +c3->P4(),Charge() +c1->Charge() +c2->Charge() +c3->Charge() );
  RhoCandidate* cand = RhoFactory::Instance()->NewCandidate ( tmp );
  cand->SetMarker ( fMarker[0]|c1->fMarker[0]|c2->fMarker[0]|c3->fMarker[0],0 );
  cand->SetMarker ( fMarker[1]|c1->fMarker[1]|c2->fMarker[1]|c3->fMarker[1],1 );
  cand->SetMarker ( fMarker[2]|c1->fMarker[2]|c2->fMarker[2]|c3->fMarker[2],2 );
  cand->SetMarker ( fMarker[3]|c1->fMarker[3]|c2->fMarker[3]|c3->fMarker[3],3 );

  cand->SetCovP4 ( P4Cov() +c1->P4Cov() +c2->P4Cov() +c3->P4Cov() );

  //Only one-way link because we're not sure where else the daughters are used (combinatorics)
  cand->AddDaughterLinkSimple(this);
  cand->AddDaughterLinkSimple(c1);
  cand->AddDaughterLinkSimple(c2);
  cand->AddDaughterLinkSimple(c3);

  return cand;
}

RhoCandidate* RhoCandidate::Combine ( RhoCandidate* c1, RhoCandidate* c2, RhoCandidate* c3, RhoCandidate* c4 )
{
  RhoCandidate tmp ( P4() +c1->P4() +c2->P4() +c3->P4()  + c4->P4(), Charge() +c1->Charge() +c2->Charge() +c3->Charge() +c4->Charge() );
  RhoCandidate* cand = RhoFactory::Instance()->NewCandidate ( tmp );
  cand->SetMarker ( fMarker[0]|c1->fMarker[0]|c2->fMarker[0]|c3->fMarker[0]|c4->fMarker[0],0 );
  cand->SetMarker ( fMarker[1]|c1->fMarker[1]|c2->fMarker[1]|c3->fMarker[1]|c4->fMarker[1],1 );
  cand->SetMarker ( fMarker[2]|c1->fMarker[2]|c2->fMarker[2]|c3->fMarker[2]|c4->fMarker[2],2 );
  cand->SetMarker ( fMarker[3]|c1->fMarker[3]|c2->fMarker[3]|c3->fMarker[3]|c4->fMarker[3],3 );

  cand->SetCovP4 ( P4Cov() +c1->P4Cov() +c2->P4Cov() +c3->P4Cov() +c4->P4Cov() );

  //Only one-way link because we're not sure where else the daughters are used (combinatorics)
  cand->AddDaughterLinkSimple(this);
  cand->AddDaughterLinkSimple(c1);
  cand->AddDaughterLinkSimple(c2);
  cand->AddDaughterLinkSimple(c3);
  cand->AddDaughterLinkSimple(c4);

  return cand;
}


void RhoCandidate::SetMarker ( UInt_t l,UInt_t m )
{
  if ( m<4 ) {
    fMarker[m]=l;
  } else {
    cout << "RhoCandidate: Trying to set non-existent marker " << m << endl;
  }
}


RhoError&
RhoCandidate::PosCov()   const
{
  static RhoError covPos ( 3 );
  covPos ( 0,0 ) = fErrP7[0];
  covPos ( 1,0 ) = covPos ( 0,1 ) = fErrP7[1];
  covPos ( 1,1 ) = fErrP7[2];
  covPos ( 2,0 ) = covPos ( 0,2 ) = fErrP7[3];
  covPos ( 2,1 ) = covPos ( 1,2 ) = fErrP7[4];
  covPos ( 2,2 ) = fErrP7[5];
  return covPos;
}

RhoVector3Err
RhoCandidate::PosWCov()   const
{
  return RhoVector3Err ( Pos(),PosCov() );
}


RhoError&
RhoCandidate::P4Cov()   const
{
  static RhoError covP4 ( 4 );
  covP4 ( 0,0 ) = fErrP7[18];
  covP4 ( 1,0 ) = covP4 ( 0,1 ) = fErrP7[19];
  covP4 ( 1,1 ) = fErrP7[20];
  covP4 ( 2,0 ) = covP4 ( 0,2 ) = fErrP7[21];
  covP4 ( 2,1 ) = covP4 ( 1,2 ) = fErrP7[22];
  covP4 ( 2,2 ) = fErrP7[23];
  covP4 ( 3,0 ) = covP4 ( 0,3 ) = fErrP7[24];
  covP4 ( 3,1 ) = covP4 ( 1,3 ) = fErrP7[25];
  covP4 ( 3,2 ) = covP4 ( 2,3 ) = fErrP7[26];
  covP4 ( 3,3 ) = fErrP7[27];
  return covP4;
}

RhoError&
RhoCandidate::P3Cov()   const
{
  static RhoError covP3 ( 3 );
  covP3 ( 0,0 ) = fErrP7[18];
  covP3 ( 1,0 ) = covP3 ( 0,1 ) = fErrP7[19];
  covP3 ( 1,1 ) = fErrP7[20];
  covP3 ( 2,0 ) = covP3 ( 0,2 ) = fErrP7[21];
  covP3 ( 2,1 ) = covP3 ( 1,2 ) = fErrP7[22];
  covP3 ( 2,2 ) = fErrP7[23];
  return covP3;
}

RhoVector3Err
RhoCandidate::P3WErr()   const
{
  return RhoVector3Err ( P3(),P3Cov() );
}


RhoLorentzVectorErr
RhoCandidate::P4WErr()   const
{
  return RhoLorentzVectorErr ( P4(),P4Cov() );
}


Double_t
RhoCandidate::EVar()   const
{
  return fErrP7[27];
}

Double_t
RhoCandidate::P()   const
{
  return TVector3 ( fXmomentum,fYmomentum,fZmomentum ).Mag();
}


Double_t
RhoCandidate::M()   const
{
  return TLorentzVector ( fXmomentum,fYmomentum,fZmomentum,fEnergy ).Mag();
}


TMatrixD&
RhoCandidate::Cov7()   const
{
  static TMatrixD cov ( 7,7 );

  // position error

  cov ( 0,0 ) = fErrP7[0];
  cov ( 1,0 ) = fErrP7[1];
  cov ( 1,1 ) = fErrP7[2];
  cov ( 2,0 ) = fErrP7[3];
  cov ( 2,1 ) = fErrP7[4];
  cov ( 2,2 ) = fErrP7[5];

  // position-momentum covariance

  cov ( 3,0 ) = fErrP7[6];
  cov ( 3,1 ) = fErrP7[7];
  cov ( 3,2 ) = fErrP7[8];
  cov ( 4,0 ) = fErrP7[9];
  cov ( 4,1 ) = fErrP7[10];
  cov ( 4,2 ) = fErrP7[11];
  cov ( 5,0 ) = fErrP7[12];
  cov ( 5,1 ) = fErrP7[13];
  cov ( 5,2 ) = fErrP7[14];
  cov ( 6,0 ) = fErrP7[15];
  cov ( 6,1 ) = fErrP7[16];
  cov ( 6,2 ) = fErrP7[17];

  // momentum error
  cov ( 3,3 ) = fErrP7[18];
  cov ( 4,3 ) = fErrP7[19];
  cov ( 4,4 ) = fErrP7[20];
  cov ( 5,3 ) = fErrP7[21];
  cov ( 5,4 ) = fErrP7[22];
  cov ( 5,5 ) = fErrP7[23];
  cov ( 6,3 ) = fErrP7[24];
  cov ( 6,4 ) = fErrP7[25];
  cov ( 6,5 ) = fErrP7[26];
  cov ( 6,6 ) = fErrP7[27];

  //latest modification :Dipak
  for ( int i=0; i<6; i++ )
    for ( int j=i+1; j<7; j++ ) {
      cov ( i,j ) =cov ( j,i );
    }

  return cov;
}

TMatrixD&
RhoCandidate::XPCov()   const
{
  static TMatrixD covPosP4 ( 4,3 );

  // position-momentum covariance

  covPosP4 ( 0,0 ) = fErrP7[6];
  covPosP4 ( 0,1 ) = fErrP7[7];
  covPosP4 ( 0,2 ) = fErrP7[8];
  covPosP4 ( 1,0 ) = fErrP7[9];
  covPosP4 ( 1,1 ) = fErrP7[10];
  covPosP4 ( 1,2 ) = fErrP7[11];
  covPosP4 ( 2,0 ) = fErrP7[12];
  covPosP4 ( 2,1 ) = fErrP7[13];
  covPosP4 ( 2,2 ) = fErrP7[14];
  covPosP4 ( 3,0 ) = fErrP7[15];
  covPosP4 ( 3,1 ) = fErrP7[16];
  covPosP4 ( 3,2 ) = fErrP7[17];

  return covPosP4;
}

Double_t
RhoCandidate::Correlation ( Int_t x1,Int_t x2,const TMatrixD& deriv,const TMatrixD& cov ) const
{
  const int nparm = cov.GetNcols();
  Double_t error = 0.;
  for ( int i=0; i<nparm; i++ ) {
    const double d1 = deriv ( x1,i );
    if ( d1==0.0 ) { continue; } // No need to accumulate 0
    for ( int j=0; j<nparm; j++ ) {
      const double d2 = deriv ( x2,j );
      if ( d2==0.0 ) { continue; } // No need to accumulate 0
      error+=d1*cov ( i,j ) *d2;
    }
  }
  return error;
}



//
// *************************** added for testing/use in pandaROOT, K.G. 11/2007
//

void RhoCandidate::SetPidInfo ( double* pidinfo )
{
  for ( int i=0; i<30; i++ )
    if ( pidinfo ) { fPidLH[i]=pidinfo[i]; }
    else { fPidLH[i]=0; }
}

void RhoCandidate::SetPidInfo ( int hypo, double value )
{
  if ( hypo>=0 && hypo<30 ) {
    fPidLH[hypo] = value;
  }
}

double RhoCandidate::GetPidInfo ( int hypo )
{
  if ( hypo>=0 && hypo<30 ) {
    return fPidLH[hypo];
  } else {
    return -999.0;
  }
}

const double* RhoCandidate::GetPidInfo() const
{
  return fPidLH;
}
