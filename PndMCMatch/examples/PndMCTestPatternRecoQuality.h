// -------------------------------------------------------------------------
// -----                 PNDMCMATCHSELECTORTASK header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCMATCHSELECTORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef PNDMCTESTPATTERNRECOQUALITY_H
#define PNDMCTESTPATTERNRECOQUALITY_H


// framework includes
#include "FairTask.h"
#include "PndMCMatch.h"
#include "PndMCResult.h"
#include "PndDetectorList.h"
#include "PndTrackCand.h"
#include "TH2.h"


#include <vector>
#include <map>

class TClonesArray;

class PndMCTestPatternRecoQuality : public FairTask
{
 public:

  /** Default constructor **/
	PndMCTestPatternRecoQuality(TString trackBranchName);

  /** Destructor **/
  virtual ~PndMCTestPatternRecoQuality();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();



  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();

  void SetTrackBranchName(TString name) {fTrackBranchName = name;}
  void AddHitsBranchName(TString name){ fBranchNames.push_back(name);}
//  void SetPossibleTrackParameter(TString name, Double_t value){fPossibleTrackParameter[name] = value;}

  void PrintTrackDataSummary(FairMultiLinkedData* trackData);
  void PrintTrackQualityMap();

 private:

  virtual void FillMapTrackQualifikation();
  virtual Bool_t PossibleTrack(FairMultiLinkedData* mcForward);
  virtual Int_t AnalyseData();
  virtual void CalcEfficiencies(Int_t mostProbableTrack);
  virtual void FillQualyHisto();
  virtual Int_t GetSumOfAllValidMCHits(FairMultiLinkedData* trackData);
  FairMultiLinkedData GetMCInfoForBranch(TString branchName, PndTrackCand& trackCand);

  std::vector<TString> fBranchNames;

  std::map<TString, FairMultiLinkedData> fMapLinkData;
//  std::map<TString, Double_t> fPossibleTrackParameter;
  std::map<TString, TH2*> fMapEfficiencies;

  PndMCResult fIdealTrackData;
  Int_t fNGhosts;

  PndMCMatch* fMCMatch;
  TClonesArray* fTrack;
  TClonesArray* fMCTrack;
  TClonesArray* fTrackCand;

  FairRootManager* ioman;

  std::map<Int_t, Int_t> fMapTrackQualifikation;

  TString fTrackBranchName;

  TH1* fPHisto;
  TH1* fPtHisto;
  TH1* fQualyHisto;

  Int_t fEventNr;

  void Register();

  void Reset();


  ClassDef(PndMCTestPatternRecoQuality,3);

};

#endif
