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

  //! Sets the branch name of the track data which should be analyzed.
  void SetTrackBranchName(TString name) {fTrackBranchName = name;}

  //! Adds branch names of detector data which should be taken into account in the analysis
  void AddHitsBranchName(TString name){ fBranchNames.push_back(name);}

  //! Prints how many hits of one detector are in trackData.
  void PrintTrackDataSummary(FairMultiLinkedData& trackData);

  /** Prints the information of the track quality map.
   * First output value is the MC track ID. Second output the quality indicator:
   * -1 : Track with at least one hit in a tracking detector but of type 0
   * 0  : Track with sufficient hits in tracking detectors for track finding but not found
   * 1  : Not assigned
   * 2  : All hits of the MC track were found and no additional once
   * 3  : Some hits of the MC track were found and no additional once
   * 4  : 70 % of all hits found belong to this MC track
   */
  void PrintTrackQualityMap();

 private:

  virtual void FillMapTrackQualifikation();

/** Checks if a MC track could be found by the tracking algorithm.
  * \param mcForward a FairMultiLinkedData pointer containing the hits from a track found by ideal track finding
  * \return kTRUE if a track could be found, kFALSE if not
  *  At the moment this method checks if there are at least 4 hit points in the MVD or more than 5 hits in the STT.
  *  For a more sophisticated analysis this method has to be overwritten in a derived class
  */
  virtual Bool_t PossibleTrack(FairMultiLinkedData& mcForward);

  /** Analyses the track data and assigns quality indicator to track.
   * Quality indicators:
   * 1  : Not assigned
   * 2  : All hits of the MC track were found and no additional once
   * 3  : Some hits of the MC track were found and no additional once
   * 4  : 70 % of all hits found belong to this MC track
   */
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
