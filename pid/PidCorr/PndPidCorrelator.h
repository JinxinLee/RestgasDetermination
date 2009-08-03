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

#include "FairTask.h"
#include "FairField.h"
#include "FairTrackParH.h"
#include "FairGeanePro.h"
#include "FairGeane.h"

#include "PndTrack.h"
#include "PndMvdGeoHandling.h"
#include "PndPidCorrPar.h"
#include "PndPidCandidate.h"

class TGeant3;
class PndPidCorrelator : public FairTask {

protected:

  TClonesArray* fTrack;             //! PndTrack TCA
  TClonesArray* fPidChargedCand;    //! PndPidCandidate TCA
  
  TClonesArray* fMvdHitsStrip;      //! PndMvdHit TCA for strip
  TClonesArray* fMvdHitsPixel;      //! PndMvdHit TCA for pixel
  TClonesArray* fTofHit;            //! PndTofHit TCA
  TClonesArray* fEmcCluster;        //! PndEmcCluster TCA
  TClonesArray* fMdtHit;            //! PndMdtHit TCA
  TClonesArray* fDrcHit;            //! PndDrcHit TCA
  TClonesArray* fSttHit;            //! PndSttHit/PndSttHelixHit TCA
 
  PndPidCorrPar* fCorrPar;          //! Correlation parameters
  
  Short_t fDebugMode;               // Fill ntuples for debug
  Short_t fMvdMode;                 // MVD Mode: 0 no MVD, 1 MvdPoint, (2) MvdHit
  Short_t fSttMode;                 // STT Mode: 0 no STT, 1 SttPoint,  2  SttHit, (3) SttHelixHit
  Short_t fTofMode;                 // TOF Mode: 0 no TOF, 1 -empty-,  (2) TofHit
  Short_t fEmcMode;                 // EMC Mode: 0 no EMC, 1 -empty-,  (2) EmcCluster, 3 EmcBumps
  Short_t fMdtMode;                 // MDT Mode: 0 no MDT, 1 -empty-,  (2) MdtHit 
  Short_t fDrcMode;                 // DRC Mode: 0 no DRC, 1 -empty-,  (2" DrcHit
 
  Double_t fMvdELoss;               // Energy Loss in MVD 
  Double_t fMvdPath;                // MVD path crossed by the particle
  Int_t fMvdHitCount;               // Number of mvd hits
    
  TString fTrackBranch;             //  options to choose branches
  Bool_t fVerbose;                  // Switch ON/OFF debug messages 
  Bool_t fSimulation;               // Switch simulation diagnostic
  Bool_t fGeanePro;                 // Use GEANE propagation

  FairGeanePro *fPro;     // Geane Propagator
  
  PndMvdGeoHandling* geoH;           // Object to retrieve MVD geometry
  
  TFile *r;                          // File for debug ntuples
  TNtuple *tofCorr;                  // Debug ntuple for tof correlation
  TNtuple *emcCorr;                  // Debug ntuple for emc correlation 
  TNtuple *mdtCorr;                  // Debug ntuple for mdt correlation 
  TNtuple *drcCorr;                  // Debug ntuple for drc correlation
  TString sDir;                      // Ntuple output directory
  TString sFile;                     // Ntuple output file
  
  static PndPidCorrelator* ftInstance;

public:

  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
 
  void Register();
  void Reset();
   
  PndPidCorrelator(const char *name, const char *title="Pnd Task");
  PndPidCorrelator();
  virtual ~PndPidCorrelator();

  PndPidCandidate* AddCandidate(PndPidCandidate* cand);
 
  void GetTrackInfo(PndTrack* track, PndPidCandidate* pid); 
  void GetMvdInfo  (PndTrack* track, PndPidCandidate* pid); 
  void GetSttInfo  (PndTrack* track, PndPidCandidate* pid); 
  void GetTofInfo  (FairTrackParH* helix, PndPidCandidate* pid); 
  void GetEmcInfo  (FairTrackParH* helix, PndPidCandidate* pid); 
  void GetMdtInfo  (FairTrackParH* helix, PndPidCandidate* pid);   
  void GetDrcInfo  (FairTrackParH* helix, PndPidCandidate* pid); 
   
  Float_t ExtrapolateToZ(FairTrackParH* helix, TVector3 *mom, TVector3 *vertex, const Float_t z = 0.); // extrapolate momentum and vertex at z=...
  Float_t ExtrapolateToR(FairTrackParH* helix, TVector3 *mom, TVector3 *vertex, const Float_t R);      // extrapolate momentum and vertex at sqrt(x*x+y*y)

  void SetOption(Option_t *option=" ")    {fOption = option;  fOption.ToLower();}
  void SetDebugMode(Bool_t debug)         { fDebugMode = debug; };
  void SetDebugFilename(TString filename) { sFile = filename; };
  void SetInputBranch(TString branch)     { fTrackBranch = branch; };	
  void SetVerbose(Bool_t verb)            { fVerbose = verb  ;};
  void SetSimulation(Bool_t sim)          { fSimulation = sim; };
  void SetGeanePro(Bool_t gea = kTRUE)    { fGeanePro = gea; };
	
  static PndPidCorrelator* Instance();

  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();

ClassDef(PndPidCorrelator,1)   // PndPidCorrelator

};

#endif
