// -------------------------------------------------------------------------
// -----                 PNDMCMATCHSELECTORTASK header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCMATCHSELECTORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef PndTrackingQualityTask_H
#define PndTrackingQualityTask_H


// framework includes
#include "FairTask.h"
#include "PndMCMatch.h"
#include "PndMCResult.h"
#include "PndDetectorList.h"
#include "PndTrackCand.h"
#include "TH2.h"
#include "THStack.h"
#include "RhoHistogram/RhoTuple.h"


#include <vector>
#include <map>

class TClonesArray;

class PndTrackingQualityTask : public FairTask
{
 public:

  /** Default constructor **/
	PndTrackingQualityTask(TString trackBranchName, Bool_t pndTrackData = kTRUE);

  /** Destructor **/
  virtual ~PndTrackingQualityTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();



  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();

  //! Sets the branch name of the track data which should be analyzed.
  void SetTrackBranchName(TString name, Bool_t pndTrackData = kTRUE) {fPndTrackOrTrackCand = pndTrackData; fTrackBranchName = name;} //alternatively PndTrackCandData can be analysed. Therefore pndTrackData has to be set to kFALSE

  //! Adds branch names of detector data which should be taken into account in the analysis
  void AddHitsBranchName(TString name){ fBranchNames.push_back(name);}


 private:
//
//  virtual void FillMapTrackQualifikation();


  /** Analyses the track data and assigns quality indicator to track.
   * Quality indicators:
   * 1  : Not assigned
   * 2  : All hits of the MC track were found and no additional once
   * 3  : Some hits of the MC track were found and no additional once
   * 4  : 70 % of all hits found belong to this MC track
   */


  virtual void FillQualyHisto(std::map<Int_t, Int_t> trackQualifikation, Int_t nGhosts);
  virtual void FillMCStatus(std::map<Int_t, Int_t> trackMCStatus);
  virtual void FillEfficiencies(std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t > > > efficiencies);
  virtual void MapToHist(std::map<Int_t, Double_t>, TH1*);

  virtual Int_t GetSumOfAllValidMCHits(FairMultiLinkedData* trackData);

  void InitializeHistograms();
  void LabelQualyHistogram(TH1 *);
  void ColorHistogram();
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
  Bool_t fPndTrackOrTrackCand; //kTRUE if track and kFALSE if track cand

  RhoTuple * fTuple;

  TH1* fPHisto;
  TH1* fPRelHisto;
  TH1* fPtHisto;
  TH1* fPtRelHisto;
  TH1* fQualyHisto;
  THStack * fQualyStack;
  TH1 * fQualyHisto_mc;
  TH1 * fQualyHisto_neg;
  TH1 * fQualyHisto_pos;
  TH1 * fQualyHisto_all;

  Int_t fEventNr;

  void Register();

  void Reset();


  ClassDef(PndTrackingQualityTask,1);

};

#endif
