// *************************************************************************
// Author: Ralf Kliemt r.kliemt(at)gsi(dot)de
//   
// ideal tracker using MC id & track info
// gaussian momentum smearing & fake efficiency included
//
// Created: 20.03.2013
// Modified:
//
// *************************************************************************

#ifndef STTMVDGEMTRACKINGIDEAL
#define STTMVDGEMTRACKINGIDEAL

#include "TVector3.h"
#include "TDatabasePDG.h"
#include "FairTask.h"
#include <cmath>

class PndSttMvdGemTrackingIdeal : public FairTask {
  
public:
  
  PndSttMvdGemTrackingIdeal();
  virtual ~PndSttMvdGemTrackingIdeal();  
  PndSttMvdGemTrackingIdeal(const PndSttMvdGemTrackingIdeal& o) :
    fMCTracks(o.fMCTracks),
    fMCPoints(),
    fHits(),
    fBranchIDs(),
    fBranchActive(),
    fTrackCands(o.fTrackCands),
    fTracks(o.fTracks),
    fTrackIds(o.fTrackIds),
    fMomSigma(o.fMomSigma),
    fDPoP(o.fDPoP),
    fRelative(o.fRelative),
    fVtxSigma(o.fVtxSigma),
    fEfficiency(o.fEfficiency),
    fPersistence(o.fPersistence),
    fTracksArrayName(o.fTracksArrayName),
    pdg(o.pdg)
  { 
    for(int i=0;i<4;i++){
      fMCPoints[i]=o.fMCPoints[i];
      fHits[i]=o.fHits[i];
      fBranchIDs[i]=o.fBranchIDs[i];
      fBranchActive[i]=o.fBranchActive[i];
    }
};
  PndSttMvdGemTrackingIdeal& operator=(const PndSttMvdGemTrackingIdeal& o)
  {
    fMCTracks=o.fMCTracks;
    for(int i=0;i<4;i++){
      fMCPoints[i]=o.fMCPoints[i];
      fHits[i]=o.fHits[i];
      fBranchIDs[i]=o.fBranchIDs[i];
      fBranchActive[i]=o.fBranchActive[i];
    }
    fTrackCands=o.fTrackCands;
    fTracks=o.fTracks;
    fTrackIds=o.fTrackIds;
    fMomSigma=o.fMomSigma;
    fDPoP=o.fDPoP;
    fRelative=o.fRelative;
    fVtxSigma=o.fVtxSigma;
    fEfficiency=o.fEfficiency;
    fPersistence=o.fPersistence;
    fTracksArrayName=o.fTracksArrayName;
    pdg=o.pdg;
  };
  
  virtual void Exec(Option_t * option);
  virtual InitStatus Init();              
  virtual void Finish(); 
  
  void Reset();                    
  void Register();                 
  
  void SetTrackOutput(TString name = "STTTrkIdeal") { fTracksArrayName = name; };
  
  void SetMomentumSmearing(Double_t sigmax = -1.,Double_t sigmay = -1.,Double_t sigmaz = -1.) { fMomSigma.SetXYZ(fabs(sigmax),fabs(sigmay),fabs(sigmaz)); fRelative=kFALSE; }; // in GeV
  void SetRelativeMomentumSmearing(Double_t dpop = -1.) { fDPoP=fabs(dpop); fRelative=kTRUE;}; // in GeV
  void SetVertexSmearing(Double_t sigmax = -1.,Double_t sigmay = -1.,Double_t sigmaz = -1.) { fVtxSigma.SetXYZ(fabs(sigmax),fabs(sigmay),fabs(sigmaz)); }; // in cm
  void SetTrackingEfficiency(Double_t eff = 1.) { fEfficiency=eff; };
  
  void SetSttActivity(Bool_t act=kTRUE){fBranchActive[0]=act;}
  void SetGemActivity(Bool_t act=kTRUE){fBranchActive[1]=act;}
  void SetMvdActivity(Bool_t act=kTRUE){fBranchActive[2]=act;fBranchActive[3]=act;}
  
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

protected:
  
  void SmearVector(TVector3 &vec, const TVector3 &sigma); // smearing with doubled sigma in z direction
  
  TClonesArray*  fMCTracks;         //! Array of PndMCTrack
  TClonesArray*  fMCPoints[4];      //! Array of event's points
  TClonesArray*  fHits[4];          //! Array of event's hits
  Int_t          fBranchIDs[4];     //! Array of Branch IDs
  Bool_t         fBranchActive[4];  //! Array of Branch Activeness
  
  TClonesArray  *fTrackCands;   //! Array of found track candidates
  TClonesArray  *fTracks;       //! Array of found tracks
  TClonesArray  *fTrackIds;     //! Array of track IDs (Links)

  // Parameters for fake tracking
  TVector3 fMomSigma;          // Momentum smearing sigma [GeV]
  Double_t fDPoP;              // Relative momentum Smearing
  Bool_t fRelative;            // falg
  TVector3 fVtxSigma;          // Vertex smearing sigma [cm]
  Double_t fEfficiency;        // Tracking efficiency - if (0 <= e < 1), some tracks will be discarded
  Bool_t  fPersistence;        // Persistence of tracking TCAs

  TString fTracksArrayName;     // Branch name where to store the Track candidates
  TDatabasePDG *pdg;            //! Particle DB
  
  //PndSttMvdGemTrackingIdeal(const  PndSttMvdGemTrackingIdeal& L);
  //PndSttMvdGemTrackingIdeal& operator= (const  PndSttMvdGemTrackingIdeal&) {return *this;}
  
  ClassDef(PndSttMvdGemTrackingIdeal,1);
  
};
#endif
