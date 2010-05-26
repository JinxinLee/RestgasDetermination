// -------------------------------------------------------------------------
// -----                   PndGlobalTrackMergerQA header file          -----
// -----                  Created 02.06.2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** \class PndGlobalTrackMergerQA
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 19.03.2009
 *  \brief track finding quality assesment task
 * 
 *  Track finding QA, efficiency, mom. resolution and so on
 **/


#ifndef PNDGLOBALTRACKMERGERQA_H
#define PNDGLOBALTRACKMERGERQA_H

#include "TH1F.h"
#include "TH2F.h"

#include "FairMCPoint.h"
#include "FairTask.h"
#include "PndMCTrack.h"

#include <vector>

class PndGlobalTrackMergerQA : public FairTask
{

 public:

  /** Default constructor **/
  PndGlobalTrackMergerQA();

  /** Default constructor **/
  PndGlobalTrackMergerQA(Int_t iVerbose);
  
  /** Destructor **/
  virtual ~PndGlobalTrackMergerQA();
  
  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Public modifiers **/
  void SetVerbose(const Int_t& verbose) { fVerbose = verbose; };

  void SetRecoTrackBranchName(TString tempStr) { fRecoTrackBranchName=tempStr; };
  
 private:

  /** Arrays of MC information **/
  TClonesArray* fMCTrackArray;

  TClonesArray* fRecoTrackArray;     	    ///< Output array of PndGemTracks 

  /** Event counter **/
  Int_t fNofEvents;  ///< event counter
  
  TVector3 fTargetPos;
  std::vector<Int_t> fRecoTrackMCMatch;

  TString fRecoTrackBranchName;

  Int_t fNofMCAll;
  Int_t fNofMCAcc;
  Int_t fNofMCPrim;
  Int_t fNofMCSec;
  Int_t fNofMCRef; // primary, mom.mag > 0.5GeV/c

  Int_t fNofRecoAcc;
  Int_t fNofRecoPrim;
  Int_t fNofRecoSec;
  Int_t fNofRecoRef; // primary, mom.mag > 0.5GeV/c

  Int_t fNofRecoGhosts;
  Int_t fNofRecoClones;

  TList* fHistoList;
  // number of mc tracks, reco tracks, efficiency as function of MOMENTUM
  TH1F* fhMCAllVsP, *fhMCAccVsP, *fhMCPrimVsP, *fhMCSecVsP, *fhMCRefVsP;
  TH1F* fhRecoAccVsP, *fhRecoPrimVsP, *fhRecoSecVsP, *fhRecoRefVsP;
  TH1F* fhEffAccVsP, *fhEffPrimVsP, *fhEffSecVsP, *fhEffRefVsP;
  // number of mc tracks, reco tracks, efficiency as function of THETA
  TH1F* fhMCAllVsT, *fhMCAccVsT, *fhMCPrimVsT, *fhMCSecVsT, *fhMCRefVsT;
  TH1F* fhRecoAccVsT, *fhRecoPrimVsT, *fhRecoSecVsT, *fhRecoRefVsT;
  TH1F* fhEffAccVsT, *fhEffPrimVsT, *fhEffSecVsT, *fhEffRefVsT;
  // number of mc tracks, reco tracks, efficiency as function of PHI
  TH1F* fhMCAllVsA, *fhMCAccVsA, *fhMCPrimVsA, *fhMCSecVsA, *fhMCRefVsA;
  TH1F* fhRecoAccVsA, *fhRecoPrimVsA, *fhRecoSecVsA, *fhRecoRefVsA;
  TH1F* fhEffAccVsA, *fhEffPrimVsA, *fhEffSecVsA, *fhEffRefVsA;

  // MOMENTUM, THETA and (PHI)ANGLE reco tracks distribution
  TH1F* fhRecoAllP, *fhRecoPrimP, *fhRecoSecP;
  TH1F* fhRecoAllT, *fhRecoPrimT, *fhRecoSecT;
  TH1F* fhRecoAllA, *fhRecoPrimA, *fhRecoSecA;

  // momentum resolution vs MOMENTUM
  TH2F* fhMomResAccVsP, *fhMomResPrimVsP, *fhMomResSecVsP, *fhMomResRefVsP;
  // momentum resolution vs THETA
  TH2F* fhMomResAccVsT, *fhMomResPrimVsT, *fhMomResSecVsT, *fhMomResRefVsT;
  // momentum resolution vs THETA
  TH2F* fhMomResAccVsA, *fhMomResPrimVsA, *fhMomResSecVsA, *fhMomResRefVsA;

  TH1F* fhNofMCTracksPerEvent, *fhNofRecoTracksPerEvent;

  void CreateHistos();
  void PrepareMCTracks();
  void MatchRecoTracks();

  /** Get parameter containers **/
  virtual void SetParContainers();

  void DivideHistos(TH1* hist1, TH1* hist2, TH1* hist3);

  /** Finish **/
  virtual void Finish();

  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  ClassDef(PndGlobalTrackMergerQA,1);

};

#endif
