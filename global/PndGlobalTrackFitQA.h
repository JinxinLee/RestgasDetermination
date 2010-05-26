// -------------------------------------------------------------------------
// -----                   PndGlobalTrackFitQA header file          -----
// -----                  Created 02.06.2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** \class PndGlobalTrackFitQA
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 19.03.2009
 *  \brief track finding quality assesment task
 * 
 *  Track finding QA, efficiency, mom. resolution and so on
 **/


#ifndef PNDGLOBALTRACKFITQA_H
#define PNDGLOBALTRACKFITQA_H

#include "TH1F.h"
#include "TH2F.h"

#include "FairMCPoint.h"
#include "FairTask.h"
#include "PndMCTrack.h"

#include <vector>

class PndGlobalTrackFitQA : public FairTask
{

 public:

  /** Default constructor **/
  PndGlobalTrackFitQA();

  /** Default constructor **/
  PndGlobalTrackFitQA(Int_t iVerbose);
  
  /** Destructor **/
  virtual ~PndGlobalTrackFitQA();
  
  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Public modifiers **/
  void SetVerbose(const Int_t& verbose) { fVerbose = verbose; };

  
 private:

  /** Arrays of MC information **/
  TClonesArray* fMCTrackArray;

  TClonesArray* fRecoTrackArray;     	    ///< Output array of PndGemTracks 

  TClonesArray* fFitTrackArray;

  /** Event counter **/
  Int_t fNofEvents;  ///< event counter
  Int_t fNofMCTracks;
  Int_t fNofRecoTracks;
  Int_t fNofFitTracks;
  
  TVector3 fTargetPos;
  std::vector<Int_t> fRecoTrackMCMatch;
  std::vector<Int_t> fFitTrackRecoMatch;

  TList* fHistoFList;

  TH2F* fhMomentumImprovementVsP;
  TH2F* fhMomentumImprovementVsPThetaBinned[20];
  TH2F* fhMomentumImprovementVsT;
  TH2F* fhMomentumImprovementVsTMomBinned[20];

  TH2F* fhThetaImprovementVsP;
  TH2F* fhThetaImprovementVsPThetaBinned[20];
  TH2F* fhThetaImprovementVsT;
  TH2F* fhThetaImprovementVsTMomBinned[20];

  TH2F* fhMomentumImprovementVsPDet;
  TH2F* fhMomentumImprovementVsTDet;

  TH2F* fhThetaImprovementVsPDet;
  TH2F* fhThetaImprovementVsTDet;

  TH1F* fhNofMCTracksPerEvent;
  TH1F *fhNofRecoTracksPerEvent;
  TH1F *fhNofFitTracksPerEvent;

  void CreateHistos();
  void PrepareMCTracks();
  void MatchRecoTracks();
  void MatchFitTracks();

  /** Get parameter containers **/
  virtual void SetParContainers();

  void DivideHistos(TH1* hist1, TH1* hist2, TH1* hist3);

  /** Finish **/
  virtual void Finish();

  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  ClassDef(PndGlobalTrackFitQA,1);

};

#endif
