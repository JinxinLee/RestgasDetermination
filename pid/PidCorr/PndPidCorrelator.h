#ifndef PID_CORRELATOR_H
#define PID_CORRELATOR_H

// **************************************************************************
//  Author: Stefano Spataro e-mail: spataro@to.infn.it
//   
//  pid correlator
//
// Created: 03-07-09
// Modified:
//
// **************************************************************************

#include "TNtuple.h"
#include "TString.h"
#include "TClonesArray.h"
#include <map>

#include "FairTask.h"
#include "FairField.h"
#include "FairTrackParH.h"
#include "FairGeanePro.h"
#include "FairGeane.h"

#include "PndTrack.h"
#include "PndGeoHandling.h"
#include "PndRecoKalmanFit.h"
#include "PndPidCorrPar.h"
#include "PndPidCandidate.h"
#include "PndEmcXClMoments.h"


using std::map;

class TGeant3;
class PndPidCorrelator : public FairTask {

protected:

  TClonesArray* fTrack;             //! PndTrack TCA
  TClonesArray* fTrackID;           //! PndTrackID TCA
  TClonesArray* fPidChargedCand;    //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidNeutralCand;    //! PndPidCandidate TCA for neutral particles
  TClonesArray* fMdtTrack;          //! PndTrack TCA for MDT refit

  TClonesArray* fMvdHitsStrip;      //! PndSdsHit TCA for strip
  TClonesArray* fMvdHitsPixel;      //! PndSdsHit TCA for pixel
  TClonesArray* fTofHit;            //! PndTofHit TCA 
  TClonesArray* fTofPoint;          //! PndTofPoint TCA
  TClonesArray* fEmcCluster;        //! PndEmcCluster TCA 
  TClonesArray* fEmcBump;           //! PndEmcBump TCA
  TClonesArray* fMdtPoint;          //! PndMdtPoint TCA 
  TClonesArray* fMdtHit;            //! PndMdtHit TCA 
  TClonesArray* fMdtTrk;            //! PndMdtTrk TCA
  TClonesArray* fDrcPoint;          //! PndDrcBarPoint TCA
  TClonesArray* fDrcHit;            //! PndDrcHit TCA
  TClonesArray* fDskParticle;       //! PndDskParticle TCA  //need to change to PndDskHit in future
  TClonesArray* fSttHit;            //! PndSttHit/PndSttHelixHit TCA
 
  PndPidCorrPar* fCorrPar;          //! Correlation parameters
  
  Short_t fDebugMode;               // Fill ntuples for debug
  Short_t fMvdMode;                 // MVD Mode: 0 no MVD, 1 MvdPoint, (2) MvdHit
  Short_t fSttMode;                 // STT Mode: 0 no STT, 1 SttPoint,  2  SttHit, (3) SttHelixHit
  Short_t fTofMode;                 // TOF Mode: 0 no TOF, 1 -empty-,  (2) TofHit
  Short_t fEmcMode;                 // EMC Mode: 0 no EMC, 1 -empty-,  (2) EmcCluster, 3 EmcBumps
  Short_t fMdtMode;                 // MDT Mode: 0 no MDT, 1 -empty-,  (2) MdtHit 
  Short_t fDrcMode;                 // DRC Mode: 0 no DRC, 1 -empty-,  (2" DrcHit
  Short_t fDskMode;                 // DSK Mode: 0 no DSK, 1 -empty-,  (2" DskParticle
 
  Double_t fMvdELoss;               // Energy Loss in MVD 
  Double_t fMvdPath;                // MVD path crossed by the particle
  Int_t fMvdHitCount;               // Number of mvd hits
    
  map<Int_t, Bool_t> fClusterList;  // List of clusters correlated to tracks
  TString fTrackBranch;             //  options to choose PndTrack branches
  TString fTrackIDBranch;           //  options to choose PndTrackID branches
  TString fTrackOutBranch;          //  options to choose output branch
  Bool_t fVerbose;                  // Switch ON/OFF debug messages 
  Bool_t fSimulation;               // Switch simulation diagnostic
  Bool_t fGeanePro;                 // Use GEANE propagation 
  Bool_t fMdtRefit;                 // Use MDT Kalman refit propagation
  Bool_t fIdeal;                    // Ideal Correlation
  Int_t  fEventCounter;             // Event number
  Int_t fPidHyp;                    // particle hypothesis for propagation
  PndGeoHandling* fGeoH;             // Object to retrieve MVD geometry
  PndRecoKalmanFit *fFitter;         // Refitter for MDT tracks
  TFile *r;                          // File for debug ntuples
  TNtuple *tofCorr;                  // Debug ntuple for tof correlation
  TNtuple *emcCorr;                  // Debug ntuple for emc correlation 
  TNtuple *mdtCorr;                  // Debug ntuple for mdt correlation 
  TNtuple *drcCorr;                  // Debug ntuple for drc correlation
  TNtuple *dskCorr;                  // Debug ntuple for dsk correlation

  TString sDir;                      // Ntuple output directory
  TString sFile;                     // Ntuple output file
  
  void ConstructChargedCandidate();
  void ConstructNeutralCandidate();
   
  PndPidCandidate* AddChargedCandidate(PndPidCandidate* cand); 
  PndPidCandidate* AddNeutralCandidate(PndPidCandidate* cand); 
  PndTrack* AddMdtTrack(PndTrack* track);
 
  Bool_t GetTrackInfo(PndTrack* track, PndPidCandidate* pid); 
  Bool_t GetMvdInfo  (PndTrack* track, PndPidCandidate* pid); 
  Bool_t GetSttInfo  (PndTrack* track, PndPidCandidate* pid);  
  Bool_t GetTpcInfo  (PndTrack* track, PndPidCandidate* pid); 
  Bool_t GetTofInfo  (FairTrackParH* helix, PndPidCandidate* pid); 
  Bool_t GetEmcInfo  (FairTrackParH* helix, PndPidCandidate* pid); 
  Bool_t GetMdtInfo  (PndTrack* track, PndPidCandidate* pid);   
  Bool_t GetDrcInfo  (FairTrackParH* helix, PndPidCandidate* pid); 
  Bool_t GetDskInfo  (FairTrackParH* helix, PndPidCandidate* pid);
 
public:

  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
 
  void Register();
  void Reset();
   
  PndPidCorrelator(const char *name, const char *title="Pnd Task");
  PndPidCorrelator();
  virtual ~PndPidCorrelator();

  void SetOption(Option_t *option=" ")    {fOption = option;  fOption.ToLower();}
  void SetDebugMode(Bool_t debug)         { fDebugMode = debug; };
  void SetDebugFilename(TString filename) { sFile = filename; };
  void SetMdtRefit(Bool_t mdt)            { fMdtRefit = mdt; };
  void SetInputBranch(TString branch)     { fTrackBranch = branch; };	
  void SetInputIDBranch(TString branch)   { fTrackIDBranch = branch; };	 
  void SetOutputBranch(TString branch)    { fTrackOutBranch = branch; };
  void SetVerbose(Bool_t verb)            { fVerbose = verb  ;};
  void SetSimulation(Bool_t sim)          { fSimulation = sim; };
  void SetIdeal(Bool_t id)                { fIdeal = id; };
  void SetGeanePro(Bool_t gea = kTRUE)    { fGeanePro = gea; };
  void SetPidHyp(Int_t pid)               { fPidHyp = pid; };
  
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();

ClassDef(PndPidCorrelator,1)   // PndPidCorrelator

};

#endif
