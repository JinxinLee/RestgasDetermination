// *************************************************************************
// Author: Ralf Kliemt ralf.kliemt(at)hiskp(dot)uni-bonn(dot)de
//   
// ideal tracker using MC id & track info
// gaussian momentum smearing & fake efficiency included
// z-momentum & z-coordinate of vertex is smeared with doubled sigma (just a guess)
//
// Created: 28.01.2011
// Modified:
//
// *************************************************************************

#ifndef FTSTRACKERIDEALH
#define FTSTRACKERIDEALH

//#include "TObject.h"
//#include "TObjArray.h"
//#include "TArrayI.h"
//#include "TBenchmark.h"
#include "TVector3.h"
#include "TDatabasePDG.h"
//#include "Riostream.h"
#include "FairTask.h"
#include <cmath>

class PndFtsTrackerIdeal : public FairTask {
  
public:
  
  PndFtsTrackerIdeal();
  virtual ~PndFtsTrackerIdeal();  
  
  virtual void Exec(Option_t * option);
  virtual InitStatus Init();              
  virtual void Finish(); 
  
  void Reset();                    
  void Register();                 
  
  void SetTrackOutput(TString name = "FTSTrkIdeal") { fTracksArrayName = name; };
  
  void SetMomentumSmearing(Double_t sigmax = -1.,Double_t sigmay = -1.,Double_t sigmaz = -1.) { fMomSigma.SetXYZ(fabs(sigmax),fabs(sigmay),fabs(sigmaz)); fRelative=kFALSE; }; // in GeV
  void SetRelativeMomentumSmearing(Double_t dpop = -1.) { fDPoP=fabs(dpop); fRelative=kTRUE;}; // in GeV
  void SetVertexSmearing(Double_t sigmax = -1.,Double_t sigmay = -1.,Double_t sigmaz = -1.) { fVtxSigma.SetXYZ(fabs(sigmax),fabs(sigmay),fabs(sigmaz)); }; // in cm
  void SetTrackingEfficiency(Double_t eff = 1.) { fEfficiency=eff; };
  
protected:
  
  void SmearFWD(TVector3 &vec, const TVector3 &sigma); // smearing with doubled sigma in z direction
  
  TClonesArray*  fMCTracks;     // Array of PndMCTrack
  TClonesArray*  fMCPoints[4];  // Array of event's points
  TClonesArray*  fHits[4];      // Array of event's hits
  Int_t          fBranchIDs[4];  // Array of Branch IDs
  
  TClonesArray  *fTrackCands;   // Array of found track candidates
  TClonesArray  *fTracks;       // Array of found tracks
  TClonesArray  *fTrackIds;     // Array of track IDs (Links)

  // Parameters for fake tracking
  TVector3 fMomSigma;          // Momentum smearing sigma [GeV]
  Double_t fDPoP;              // Relative momentum Smearing
  Bool_t fRelative;            // falg
  TVector3 fVtxSigma;          // Vertex smearing sigma [cm]
  Double_t fEfficiency;        // Tracking efficiency - if (0 <= e < 1), some tracks will be discarded
  
  TString fTracksArrayName;     // Branch name where to store the Track candidates
  TDatabasePDG *pdg;            // Particle DB
  
  PndFtsTrackerIdeal(const  PndFtsTrackerIdeal& L);
  PndFtsTrackerIdeal& operator= (const  PndFtsTrackerIdeal&) {return *this;}
  
  ClassDef(PndFtsTrackerIdeal,1);
  
};
#endif
