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
#include "PndLheCandidate.h"
#include "PndLheTrackCuts.h"

class PndLheHitsMaker : public FairTask {

    
protected:

  Int_t     fNHit;
  Int_t     fNTrack;
  Int_t     fNofEvents;

  TClonesArray  *fTpcInput;             //! pointers to TPC TCA
  TClonesArray  *fSttHelixInput;        //! pointers to STT Helix TCA 
  TClonesArray  *fSttPRHelixInput;      //! pointers to STT Helix PR TCA 
  TClonesArray  *fSttHitInput;          //! pointers to STT Hit TCA
  TClonesArray  *fSttMCArray;           //! pointers to SttPoint TCA
  TClonesArray  *fEmcInput;             //! pointers to EMC TCA
  TClonesArray  *fMvdStripHitArray;     //!
  TClonesArray  *fMvdPixelHitArray;     //!
  TClonesArray  *fMvdStripClusterArray; //!
  TClonesArray  *fMvdPixelClusterArray; //!
  TClonesArray  *fMvdStripDigiArray;    //!
  TClonesArray  *fMvdPixelDigiArray;    //!
  TClonesArray  *fMvdMCArray;           //!
  TClonesArray  *fGemMCArray;           //! pointers to GEMPoint TCA
  TClonesArray  *fGemInput;             //! pointers to GEM TCA
  
  TClonesArray  *fLheHits;      //! pointers to LHE hits

  TClonesArray  *fListMCtracks;  //! pointers to MC tracks
  TClonesArray  *fGeantTracks;   //! pointers to selected Geant tracks

  PndLheTrackCuts *fCuts;

  TString fOption;     //  options to choose particles

  Short_t fMvdMode;            // MVD Mode: 0 no MVD, (1) MvdPoint, 2 MvdHit
  Short_t fMvdSimMode;         // MVD Simulation Mode (ideal tracking)
  Short_t fTpcMode;            // TPC Mode: 0 no TPC, (1) TpcPoint, 2 TpcCluster
  Short_t fSttMode;            // STT Mode: (0) no STT, 1  SttPoint,  2 SttHit, 3 SttHelixHit, 4 SttHelixHit with MC position, 5 SttPRHelixHit
  Short_t fSttSimMode;         // STT Simulation Mode (ideal tracking)
  Short_t fEmcMode;            // EMC Mode: (0) no EMC, 2 EmcCluster, 3 EmcBumps
  Short_t fGemMode;            // GEM Mode: (0) no GEM, 1 GemPoint, 2 GemHit
  Short_t fGemSimMode;         // GEM Simulation Mode (ideal tracking)
  Bool_t fVerbose;             // Switch ON/OFF debug messages        (defaulf OFF)
  Bool_t fSimulation;          // Switch ON/OFF simulation mode       (default ON)
  Bool_t fPersistence;         // Persistence of LheHit/LheGeantTrack (default ON)

  Float_t fTpcResolution;      // Position smearing for TPC Points; (if negative -> no smearing)
  Float_t fMvdResolution;      // Position smearing for MVD Points; (if negative -> no smearing)
  Float_t fSttResolution;      // Position smearing for STT Points; (if negative -> no smearing)
  Float_t fGemResolution;      // Position smearing for GEM Points; (if negative -> no smearing)

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
  PndLheCandidate *AddTrack(Int_t mT);

  void GetMvdPoints();
  void GetMvdHits();
  void GetTpcPoints(); 
  void GetTpcClusters();
  void GetSttPoints();
  void GetSttHit();
  void GetSttHelixHit();
  void GetSttPRHelixHit();
  void GetSttHelixHitMC();
  void GetEmcClusters();
  void GetEmcBumps();
  void GetGemPoints();
  void GetGemHits();
  void SetOption(Option_t *option=" ") {fOption = option;  fOption.ToLower();}
  void SetTrack(PndLheHit *hit);
  void CheckTracks();
  void PrintTracks(Int_t n);
  //void GetStripPoints();  // not implemented
  
  Float_t GetTpcResolution()                       { return   fTpcResolution;}; 
  Float_t GetMvdResolution()                       { return   fMvdResolution;};

  void SetTpcMode(Short_t tpc, Float_t res = -1.)  {  fTpcMode       = tpc  ; fTpcResolution = res  ;};  
  void SetMvdMode(Short_t mvd, Float_t res = -1.)  {  fMvdMode       = mvd  ; fMvdResolution = res  ;};
  void SetSttMode(Short_t stt, Float_t res = -1.)  {  fSttMode       = stt  ; fSttResolution = res  ;};
  void SetEmcMode(Short_t emc)                     {  fEmcMode       = emc  ;}
  void SetGemMode(Short_t gem, Float_t res = -1.)  {  fGemMode       = gem  ; fGemResolution = res  ;};
  void SetVerbose(Bool_t verb)                     {  fVerbose       = verb ;}; 
  void SetSimulation(Bool_t sim)                   {  fSimulation    = sim  ;};
  void SetPersistence(Bool_t pers)                 {  fPersistence   = pers ;};
  void SetTpcResolution(Float_t res)               {  fTpcResolution = res  ;}; 
  void SetMvdResolution(Float_t res)               {  fMvdResolution = res  ;};
  void SetSttResolution(Float_t res)               {  fSttResolution = res  ;};

  ClassDef(PndLheHitsMaker,1)      // PndLheHitsMaker

};

#endif
