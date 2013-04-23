// -------------------------------------------------------------------------
// -----                   PndBarrelTrackFinderQA header file       -----
// -----                  Created 23.12.2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** \class PndBarrelTrackFinderQA
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 08.12.2010
 *  \brief QA for track finding class
 * 
 **/


#ifndef PNDBARRELTRACKFINDERQA_H
#define PNDBARRELTRACKFINDERQA_H

#include "TH1F.h"
#include "TH2F.h"

#include "FairMCPoint.h"
#include "FairTask.h"
#include "PndMCTrack.h"

#include "PndTrackCand.h"

#include <vector>

class PndBarrelTrackFinderQA : public FairTask
{

 public:

  /** Default constructor **/
  PndBarrelTrackFinderQA();

  /** Default constructor **/
  PndBarrelTrackFinderQA(Int_t iVerbose);
  
  /** Destructor **/
  virtual ~PndBarrelTrackFinderQA();
  
  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Public modifiers **/
  void SetVerbose(const Int_t& verbose) { fVerbose = verbose; };

  void UseMvdSttGem(const Bool_t useMvd, const Bool_t useStt, const Bool_t useGem) {
    fIncludeDet[0] = useMvd;
    fIncludeDet[1] = useStt;
    fIncludeDet[2] = useGem;
  };

 private:

  /** Arrays of MC information **/
  TClonesArray*       fMCTrackArray;
  TClonesArray*       fMCPointArray[3];
  TClonesArray*       fBarrelTrackArray;
  Bool_t              fIncludeDet[3];

  TString fDetName[3];

  /** Event counter **/
  Int_t fTNofEvents;  ///< event counter

  TList* fHistoList;
  // mc momentum vs reco momentum, all combinations
  TH2F* fhMomMagCompAll2D;
  TH2F* fhMomPhiCompAll2D;
  TH2F* fhMomTheCompAll2D;
  TH1F* fhMomMagCompAll1D;
  TH1F* fhMomResCompAll1D;
  TH1F* fhMomPhiCompAll1D;
  TH1F* fhMomTheCompAll1D;

  TH1F* fhMomMagSimAllHist;
  TH1F* fhMomMagRecAllHist;
  TH1F* fhMomMagEffAllHist;
  TH1F* fhMomPhiSimAllHist;
  TH1F* fhMomPhiRecAllHist;
  TH1F* fhMomPhiEffAllHist;
  TH1F* fhMomTheSimAllHist;
  TH1F* fhMomTheRecAllHist;
  TH1F* fhMomTheEffAllHist;

  TH1F* fhMomMagSimHist;
  TH1F* fhMomMagRecHist;
  TH1F* fhMomMagEffHist;
  TH1F* fhMomPhiSimHist;
  TH1F* fhMomPhiRecHist;
  TH1F* fhMomPhiEffHist;
  TH1F* fhMomTheSimHist;
  TH1F* fhMomTheRecHist;
  TH1F* fhMomTheEffHist;

  TH1F* fhMomMagAllRecoHist;
  TH1F* fhMomPhiAllRecoHist;
  TH1F* fhMomTheAllRecoHist;

  TH1F* fhMomMagCloneHist;
  TH1F* fhMomPhiCloneHist;
  TH1F* fhMomTheCloneHist;
  TH1F* fhMomMagGhostHist;
  TH1F* fhMomPhiGhostHist;
  TH1F* fhMomTheGhostHist;

  TH1F* fhRecoMomMag;
  TH1F* fhRecoMomPhi;
  TH1F* fhRecoMomThe;

  TH2F* fhMomResVsMomMag;
  TH2F* fhMomResVsMomPhi;
  TH2F* fhMomResVsMomThe;
  TH2F* fhPhiResVsMomMag;
  TH2F* fhPhiResVsMomPhi;
  TH2F* fhPhiResVsMomThe;
  TH2F* fhTheResVsMomMag;
  TH2F* fhTheResVsMomPhi;
  TH2F* fhTheResVsMomThe;

  TH2F* fhNofDetPntsVsMagFoundHist[3];
  TH2F* fhNofDetPntsVsPhiFoundHist[3];
  TH2F* fhNofDetPntsVsTheFoundHist[3];
  TH2F* fhNofDetPntsVsMagNotFdHist[3];
  TH2F* fhNofDetPntsVsPhiNotFdHist[3];
  TH2F* fhNofDetPntsVsTheNotFdHist[3];

  TH2F* fhNofMvdPixsVsThetaHist;
  TH2F* fhNofMvdStrsVsThetaHist;
  TH2F* fhNofSttHitsVsThetaHist;
  TH2F* fhNofGemHitsVsThetaHist;

  void CreateHistos();

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Finish **/
  virtual void Finish();

  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  ClassDef(PndBarrelTrackFinderQA,1);

};

#endif
