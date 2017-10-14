//-*- Mode: C++ -*-
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDFTSCAMCTRACK_H
#define PNDFTSCAMCTRACK_H

#include "PndFTSCADef.h"
#include "TMath.h"

#include <iostream>
using std::ostream;
using std::istream;

class TParticle;
class PndMCTrack;

/**
 * @class PndFTSCAMCTrack
 * store MC track information for PndFTSCAPerformance
 */
class PndFTSCAMCTrack
{
 public:

  PndFTSCAMCTrack();
  PndFTSCAMCTrack( const TParticle *part );

  void SetMCTrack(const PndMCTrack* ttt, Double_t q, unsigned int Nmcpoints, unsigned int FirstmcpointId);
  PndFTSCAMCTrack& operator =(const PndFTSCAMCTrack &rhs);
  
  // void SetTPCPar( float X, float Y, float Z, float Px, float Py, float Pz );

  int MotherId()       const { return fMotherId; }
  int    PDG()         const { return fPDG;}
  float Par( int i )    const { return fPar[i]; }
  float TPCPar( int i ) const { return fTPCPar[i]; }

  float X()           const { return fPar[0]; }
  float Y()           const { return fPar[1]; }
  float Z()           const { return fPar[2]; }
  float Px()          const { return fPar[3]/fPar[6]; }
  float Py()          const { return fPar[4]/fPar[6]; }
  float Pz()          const { return fPar[5]/fPar[6]; }
  float P()           const { return fP; }
  float Pt()          const { return fPt; }
  const float *Par()            const { return fPar; }
  const float *TPCPar()         const { return fTPCPar; }

  float DzDs()        const { return Pz()/TMath::Sqrt( Px()*Px() + Py()*Py() ); }
  
  int     NHits()          const { return fNHits;}
  int     NMCPoints()      const { return fNMCPoints;}
  int     FirstMCPointID() const { return fFirstMCPointID;}
  int     NReconstructed() const { return fNReconstructed; }
  int     Set()            const { return fSet; }
  int     NTurns()         const { return fNTurns; }

  int     NMCRows()         const { return fNMCRows; }
  int     NHitRows()         const { return fNHitRows; }
  int     NMCContRows()         const { return fNMCContRows; }
  int     NHitContRows()         const { return fNHitContRows; }
  
  void SetMotherId( int v )          { fMotherId = v; }
  void SetP ( float v )          { fP = v; }
  void SetPt( float v )          { fPt = v; }
  void SetPDG( int v )         { fPDG = v; }
  void SetPar( int i, float v )             { fPar[i] = v; }
  void SetTPCPar( int i, float v )          { fTPCPar[i] = v; }
  void SetNHits( int v )         { fNHits = v; }
  void SetNMCPoints( int v )      { fNMCPoints = v; }
  void SetFirstMCPointID( int v ) { fFirstMCPointID = v;}
  void SetNReconstructed( int v ) { fNReconstructed = v; }
  void SetSet( int v )           { fSet = v; }
  void SetNTurns( int v )        { fNTurns = v; }

  void SetNMCRows( int v )        { fNMCRows = v; }
  void SetNHitRows( int v )        { fNHitRows = v; }
  void SetNMCContRows( int v )        { fNMCContRows = v; }
  void SetNHitContRows( int v )        { fNHitContRows = v; }
  
  bool IsForwardTrack() const { return fIsForwardTrack; }
  void SetIsForwardTrack( bool v) { fIsForwardTrack = v; }
  


  friend ostream& operator<<(ostream& out, const PndFTSCAMCTrack &a);
  friend istream& operator>>(istream& in, PndFTSCAMCTrack &a);

 protected:

  bool  fIsForwardTrack; //* throw out tracks which don't correspond to a forward model

  int    fMotherId;      //* index of mother track in tracks array. -1 for primary tracks. -2 if a mother track is not in the acceptance
  int    fPDG;           //* particle pdg code
  float fPar[7];         //* x,y,z,ex,ey,ez,q/p
  float fTPCPar[7];      //* x,y,z,ex,ey,ez,q/p at TPC entrance (x=y=0 means no information)
  float fP, fPt;         //* momentum and transverse momentum
  int    fNHits;          //* N TPC clusters
  int    fNMCPoints;      //* N MC points
  int    fFirstMCPointID; //* id of the first MC point in the points array
  int    fNReconstructed; //* how many times is reconstructed
  int    fSet;            //* set of tracks 0-OutSet, 1-ExtraSet, 2-RefSet, 3-ExtraSecSet, 4-ExtraPrimSet, 5-RefSecSet, 6-RefPrimSet, 7-LongRefPrimSet
  int    fNTurns;         //* N of turns in the current sector

  int    fNMCRows; // N rows with MC Points. Calculated after reading all MC info.
  int    fNHitRows;
  int    fNMCContRows; // N rows with MC Points. Calculated after reading all MC info.
  int    fNHitContRows;
};

#endif
