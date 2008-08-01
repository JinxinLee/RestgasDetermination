#ifndef LHE_PID_MAKER_H
#define LHE_PID_MAKER_H

// **************************************************************************
//  Author: Stefano Spataro e-mail:stefano.spataro@exp2.physik.uni-giessen.de
//   
//  pid maker
//
// Created: 14-04-08
// Modified:
//
// **************************************************************************

#include "TNtuple.h"
#include "TString.h"

#include "CbmTask.h"
#include "CbmField.h"
#include "CbmTrackParH.h"

#include "PndTpcLheTrack.h"
#include "PndLhePidTrack.h"
#include "PndLheCorrPar.h"

class PndLhePidMaker : public CbmTask {

protected:

  TClonesArray* fTrackCand;         //! PndTpcLheTrack TCA
  TClonesArray* fPidTrackCand;      //! PndLhePidTrack TCA
 
  TClonesArray* fMvdHitsStrip;      //! PndMvdHit TCA for strip
  TClonesArray* fMvdHitsPixel;      //! PndMvdHit TCA for pixel
  TClonesArray* fTofHit;            //! PndTofHit TCA
  TClonesArray* fEmcCluster;        //! PndEmcCluster TCA
  TClonesArray* fMdtHit;            //! PndMdtHit TCA
  TClonesArray* fDrcHit;            //! PndDrcHit TCA
 
  PndLheCorrPar* fCorrPar;          //! Correlation parameters
  
  Short_t fDebugMode;               // Fill ntuples for debug
  Short_t fMvdMode;                 // MVD Mode: 0 no MVD, 1 MvdPoint, (2) MvdHit
  Short_t fTofMode;                 // TOF Mode: 0 no TOF, 1 -empty-,  (2) TofHit
  Short_t fEmcMode;                 // EMC Mode: 0 no EMC, 1 -empty-,  (2) EmcCluster, 3 EmcBumps
  Short_t fMdtMode;                 // MDT Mode: 0 no MDT, 1 -empty-,  (2) MdtHit 
  Short_t fDrcMode;                 // DRC Mode: (0) no DRC, 1 -empty-,  2 DrcHit
 
  Double_t fMvdELoss;               // Energy Loss in MVD 
  Double_t fMvdPath;                // MVD path crossed by the particle
  Int_t fMvdHitCount;               // Number of mvd hits
  
  TString fOption;          //  options to choose branches
  Bool_t fVerbose;          // Switch ON/OFF debug messages 
  Bool_t fSimulation;       // Switch simulation diagnostic

  TFile *r;                          // File for debug ntuples
  TNtuple *tofCorr;                  // Debug ntuple for tof correlation
  TNtuple *emcCorr;                  // Debug ntuple for emc correlation 
  TNtuple *mdtCorr;                  // Debug ntuple for mdt correlation 
  TNtuple *drcCorr;                  // Debug ntuple for drc correlation
  TString sDir;                      // Ntuple output directory
  TString sFile;                     // Ntuple output file
  
  static PndLhePidMaker* ftInstance;

public:

  virtual void Exec(Option_t * option);
  virtual InitStatus Init();                        //
 
  void Register();
  void Reset();
   
  PndLhePidMaker(const char *name, const char *title="Cbm Task");
  PndLhePidMaker();
  virtual ~PndLhePidMaker();

  PndLhePidTrack* AddTrack(PndLhePidTrack* track);
 
  void GetMvdInfo(const PndTpcLheHit* hit, const PndLhePidTrack* track); 
  void GetTofInfo(PndLhePidTrack* track); 
  void GetEmcInfo(PndLhePidTrack* track); 
  void GetMdtInfo(PndLhePidTrack* track);   
  void GetDrcInfo(PndLhePidTrack* track); 
   
  void SetOption(Option_t *option=" ") {fOption = option;  fOption.ToLower();}
  void SetDebugMode(Bool_t debug){ fDebugMode = debug; };
  void SetVerbose(Bool_t verb)   { fVerbose = verb  ;};
  void SetSimulation(Bool_t sim) { fSimulation = sim; };

  static PndLhePidMaker* Instance();

  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();

ClassDef(PndLhePidMaker,2)   // PndLhePidMaker

};

#endif
