#ifndef LHE_HITS_MAKER_H
#define LHE_HITS_MAKER_H

// *************************************************************************
//  Author: Oleg Rogachevsky             e-mail: rogach@sunhe.jinr.ru
//   
//  collect all hits from tracking detectors in form used for 
//  finding and fitting in LHE program 
//
// Created: 1-07-07
// Modified: Stefano Spataro
//
// *************************************************************************

#include "TClonesArray.h"

#include "FairTask.h"
#include "FairDetector.h"

#include "PndLheHit.h"
#include "PndTpcLheTrack.h"
#include "PndTpcLheTrackCuts.h"

class PndLheHitsMaker : public FairTask {

    
protected:

  Int_t     fNHit;
  Int_t     fNTrack;
  Int_t     fNofEvents;

  TClonesArray  *fTpcInput;     //! pointers to TPC TCA
  TClonesArray  *fSttInput;     //! pointers to STT TCA
  TClonesArray  *fMvdInput;     //! pointers to MVD TCA 
  TClonesArray  *fMvdInput2;    //! pointers to MVD TCA (2nd TCA for hits)
  TClonesArray  *fEmcInput;     //! pointers to EMC TCA
 
  TClonesArray  *fLheHits;      //! pointers to LHE hits

  TClonesArray  *fListMCtracks;  //! pointers to MC tracks
  TClonesArray  *fGeantTracks;   //! pointers to selected Geant tracks

  PndTpcLheTrackCuts *fCuts;

  TString fOption;     //  options to choose particles

  Short_t fMvdMode;            // MVD Mode: 0 no MVD, (1) MvdPoint, 2 MvdHit
  Short_t fTpcMode;            // TPC Mode: 0 no TPC, (1) TpcPoint, 2 TpcCluster
  Short_t fSttMode;            // STT Mode: (0) no STT, 1  SttPoint,  2 SttHit, 3 SttHelixHit
  Short_t fEmcMode;            // EMC Mode: (0) no EMC, 2 EmcCluster, 3 EmcBumps
  Bool_t fVerbose;             // Switch ON/OFF debug messages        (defaulf OFF)
  Bool_t fSimulation;          // Switch ON/OFF simulation mode       (default ON)
  Bool_t fPersistence;         // Persistence of LheHit/LheGeantTrack (default ON)

  Float_t fTpcResolution;      // Position smearing for TPC Points; (if negative -> no smearing)
  Float_t fMvdResolution;      // Position smearing for MVD Points; (if negative -> no smearing)
  Float_t fSttResolution;      // Position smearing for STT Points; (if negative -> no smearing)

public:    

  virtual InitStatus Init();              // 
  virtual InitStatus ReInit();
  virtual void Exec(Option_t * option);   //
  virtual void Finish();                  // 

  void Reset();
  void Register();
   
  PndLheHitsMaker(const char *name, const char *title="PND Task");
  PndLheHitsMaker();
  virtual ~PndLheHitsMaker();

  PndLheHit *AddHit();
  PndTpcLheTrack *AddTrack(Int_t mT);

  void GetMvdPoints();
  void GetMvdHits();
  void GetTpcPoints(); 
  void GetTpcClusters();
  void GetSttPoints();
  void GetSttHit();
  void GetSttHelixHit();
  void GetEmcClusters();
  void GetEmcBumps();
  void SetOption(Option_t *option=" ") {fOption = option;  fOption.ToLower();}
  void SetTrack(PndLheHit *hit);
  void CheckTracks();
  void PrintTracks(Int_t n);
  void GetStripPoints();
  
  Float_t GetTpcResolution()                       { return   fTpcResolution;}; 
  Float_t GetMvdResolution()                       { return   fMvdResolution;};

  void SetTpcMode(Short_t tpc, Float_t res = -1.)  {  fTpcMode       = tpc  ; fTpcResolution = res  ;};  
  void SetMvdMode(Short_t mvd, Float_t res = -1.)  {  fMvdMode       = mvd  ; fMvdResolution = res  ;};
  void SetSttMode(Short_t stt, Float_t res = -1.)  {  fSttMode       = stt  ; fSttResolution = res  ;};
  void SetEmcMode(Short_t emc)                     {  fEmcMode       = emc  ;}
  void SetVerbose(Bool_t verb)                     {  fVerbose       = verb ;}; 
  void SetSimulation(Bool_t sim)                   {  fSimulation    = sim  ;};
  void SetPersistence(Bool_t pers)                 {  fPersistence   = pers ;};
  void SetTpcResolution(Float_t res)               {  fTpcResolution = res  ;}; 
  void SetMvdResolution(Float_t res)               {  fMvdResolution = res  ;};
  void SetSttResolution(Float_t res)               {  fSttResolution = res  ;};

  ClassDef(PndLheHitsMaker,1)      // PndLheHitsMaker

};

#endif
