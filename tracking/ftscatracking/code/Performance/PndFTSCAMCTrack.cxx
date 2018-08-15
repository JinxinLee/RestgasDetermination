// $Id: PndFTSCAMCTrack.cxx,v 1.3 2011/01/31 17:18:28 fisyak Exp $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Maksym Zyzak <M.Zyzak@gsi.de>                            *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************


#include "PndFTSCAMCTrack.h"
#include "PndFTSCAMath.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
#include "PndMCTrack.h"

PndFTSCAMCTrack::PndFTSCAMCTrack()
    : fPDG( 0 ), fP( 0 ), fPt( 0 ), fNHits( 0 ), fNMCPoints( 0 ), fFirstMCPointID( 0 ), fNReconstructed( 0 ), fSet( 0 ), fNTurns( 0 ), fIsForwardTrack(true)
{
  //* Default constructor
}


PndFTSCAMCTrack::PndFTSCAMCTrack( const TParticle *part )
    : fPDG( 0 ), fP( 0 ), fPt( 0 ), fNHits( 0 ), fNMCPoints( 0 ), fFirstMCPointID( 0 ), fNReconstructed( 0 ), fSet( 0 ), fNTurns( 0 ), fIsForwardTrack(true)
{
  //* Constructor from TParticle

  for ( int i = 0; i < 7; i++ ) fPar[i] = 0;
  for ( int i = 0; i < 7; i++ ) fTPCPar[i] = 0;
  fP = 0;
  fPt = 0;

  if ( !part ) return;
  TLorentzVector mom, vtx;
  part->ProductionVertex( vtx );
  part->Momentum( mom );
  fPar[0] = part->Vx();
  fPar[1] = part->Vy();
  fPar[2] = part->Vz();
  fP = part->P();
  fPt = part->Pt();
  float pi = ( fP > 1.e-4 ) ? 1. / fP : 0;
  fPar[3] = part->Px() * pi;
  fPar[4] = part->Py() * pi;
  fPar[5] = part->Pz() * pi;
  fPar[6] = 0;
  fPDG  = part->GetPdgCode();
  if ( CAMath::Abs( fPDG ) < 100000 ) {
    TParticlePDG *pPDG = TDatabasePDG::Instance()->GetParticle( fPDG );
    if ( pPDG ) fPar[6] = pPDG->Charge() / 3.0 * pi;
  }
}

// void PndFTSCAMCTrack::SetTPCPar( float X, float Y, float Z,
//                                     float Px, float Py, float Pz )
// {
//   //* Set parameters at TPC entrance

//   for ( int i = 0; i < 7; i++ ) fTPCPar[i] = 0;

//   fTPCPar[0] = X;
//   fTPCPar[1] = Y;
//   fTPCPar[2] = Z;
//   float p = CAMath::Sqrt( Px * Px + Py * Py + Pz * Pz );
//   float pi = ( p > 1.e-4 ) ? 1. / p : 0;
//   fTPCPar[3] = Px * pi;
//   fTPCPar[4] = Py * pi;
//   fTPCPar[5] = Pz * pi;
//   fTPCPar[6] = 0;
//   if ( CAMath::Abs( fPDG ) < 100000 ) {
//     TParticlePDG *pPDG = TDatabasePDG::Instance()->GetParticle( fPDG );
//     if ( pPDG ) fTPCPar[6] = pPDG->Charge() / 3.0 * pi;
//   }
// }

void PndFTSCAMCTrack::SetMCTrack(const PndMCTrack* ttt, Double_t q, unsigned int Nmcpoints, unsigned int FirstmcpointId)
{
    if (Nmcpoints!=0){
    Double_t px = ttt->GetMomentum().X();
    Double_t py = ttt->GetMomentum().Y();
    Double_t pz = ttt->GetMomentum().Z();
    Double_t p = sqrt( px*px + py*py + pz*pz );
    
    fMotherId = ttt->GetMotherID();
    fPDG = ttt->GetPdgCode();
    
    fPar[0] = ttt->GetStartVertex().X();
    fPar[1] = ttt->GetStartVertex().Y();
    fPar[2] = ttt->GetStartVertex().Z();
    fPar[3] = px/fabs(p);
    fPar[4] = py/fabs(p);
    fPar[5] = pz/fabs(p);
    fPar[6] = q/p;
    fTPCPar[0] = 0;
    fTPCPar[1] = 0;
    fTPCPar[2] = 0;
    fTPCPar[3] = 0;
    fTPCPar[4] = 0;
    fTPCPar[5] = 0;
    fTPCPar[6] = 0;
    fP = p;
    fPt = sqrt( px*px + py*py );
    fNHits = Nmcpoints;
    fNMCPoints = Nmcpoints;
    fFirstMCPointID = FirstmcpointId;
    fNReconstructed = 0;
    fSet = 0;
    fNTurns = 1;
    fIsForwardTrack = true;
    }
    else{
    fMotherId = -1;
    fPDG = -1;
    fPar[0] = 0;
    fPar[1] = 0;
    fPar[2] = 0;
    fPar[3] = 0;
    fPar[4] = 0;
    fPar[5] = 0;
    fPar[6] = 0;
    fTPCPar[0] = 0;
    fTPCPar[1] = 0;
    fTPCPar[2] = 0;
    fTPCPar[3] = 0;
    fTPCPar[4] = 0;
    fTPCPar[5] = 0;
    fTPCPar[6] = 0;
    fP = 0;
    fPt = 0;
    fNHits = 0;
    fNMCPoints = 0;
    fFirstMCPointID = 0;
    fNReconstructed = 0;
    fSet = 0;
    fNTurns = 1;
    fIsForwardTrack = true;
    }
}

PndFTSCAMCTrack& PndFTSCAMCTrack::operator =(const PndFTSCAMCTrack &rhs)
{
    if (rhs.NMCPoints()!=0){
    Double_t px = rhs.Px();
    Double_t py = rhs.Py();
    Double_t pz = rhs.Pz();
    Double_t p = sqrt( px*px + py*py + pz*pz );
    
    fMotherId = rhs.MotherId();
    fPDG = rhs.PDG();
    
    fPar[0] = rhs.Par(0);
    fPar[1] = rhs.Par(1);
    fPar[2] = rhs.Par(2);
    fPar[3] = rhs.Par(3);
    fPar[4] = rhs.Par(4);
    fPar[5] = rhs.Par(5);
    fPar[6] = rhs.Par(6);
    fTPCPar[0] = 0;
    fTPCPar[1] = 0;
    fTPCPar[2] = 0;
    fTPCPar[3] = 0;
    fTPCPar[4] = 0;
    fTPCPar[5] = 0;
    fTPCPar[6] = 0;
    fP = p;
    fPt = sqrt( px*px + py*py );
    fNHits = rhs.NMCPoints();
    fNMCPoints = rhs.NMCPoints();
    fFirstMCPointID = rhs.FirstMCPointID();
    fNReconstructed = 0;
    fSet = 0;
    fNTurns = 1;
    fIsForwardTrack = rhs.IsForwardTrack();
    }
    else{
    fMotherId = -1;
    fPDG = -1;
    fPar[0] = 0;
    fPar[1] = 0;
    fPar[2] = 0;
    fPar[3] = 0;
    fPar[4] = 0;
    fPar[5] = 0;
    fPar[6] = 0;
    fTPCPar[0] = 0;
    fTPCPar[1] = 0;
    fTPCPar[2] = 0;
    fTPCPar[3] = 0;
    fTPCPar[4] = 0;
    fTPCPar[5] = 0;
    fTPCPar[6] = 0;
    fP = 0;
    fPt = 0;
    fNHits = 0;
    fNMCPoints = 0;
    fFirstMCPointID = 0;
    fNReconstructed = 0;
    fSet = 0;
    fNTurns = 1;
    }
    return *this;
}


ostream& operator<<(ostream& out, const PndFTSCAMCTrack &a)
{
  out << a.fMotherId << " " << a.fPDG << std::endl;
  for (int i = 0; i < 7; i++) out << a.fPar[i] << " ";
  out << std::endl;
  for (int i = 0; i < 7; i++) out << a.fTPCPar[i] << " ";
  out << std::endl;
  out << a.fP << " " << a.fPt << std::endl;
  out << a.fNHits << " " << a.fNMCPoints << " " << a.fFirstMCPointID << std::endl;
  return out << a.fNReconstructed << " " << a.fSet << " " << a.fNTurns << std::endl;
}


istream& operator>>(istream& in, PndFTSCAMCTrack &a)
{
  in >> a.fMotherId;
  in >> a.fPDG;
  for (int i = 0; i < 7; i++) in >> a.fPar[i];
  for (int i = 0; i < 7; i++) in >> a.fTPCPar[i];
  in >> a.fP >> a.fPt;
  in >> a.fNHits >> a.fNMCPoints >> a.fFirstMCPointID;
  return in >> a.fNReconstructed >> a.fSet >> a.fNTurns;
}

