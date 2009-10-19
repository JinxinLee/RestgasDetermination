// -------------------------------------------------------------------------
// -----          PndGemPrepareKalmanTracks  header file               -----
// -----            Created 30.09.2008  by A. Wronska                  -----
// -----        based on the recotasks/demo code  by S.Neubert         -----
// -------------------------------------------------------------------------

/** \class PndGemPrepareKalmanTracks2
 *  \author A.Wronska <aleksandra.wronska@uj.edu.pl>
 *  \date 15.05.2008
 *  \modified by Radoslaw Karabowicz <r.karabowicz@gsi.de> for GEM on 03/04/2009
 *  \brief PndGemPrepareKalmanTracks2 creates and initializes Tracks and is a pre-step for genfit
 **/
#ifndef PNDGEMPREPAREKALMANTRACKS_H
#define PNDGEMPREPAREKALMANTRACKS_H

// Pnd Headres
#include "FairTask.h"

// c++ headers
#include <map>

class TClonesArray;
class RecoHitFactory;
class AbsBFieldIfc;
class FairField;
class FairGeanePro;

class PndGemPrepareKalmanTracks : public FairTask {
public:

  /**  Constructor  **/
  PndGemPrepareKalmanTracks();

  /**  Destructor **/
  virtual ~PndGemPrepareKalmanTracks();

  /** public method AddHitBranch(detID, hitBranchName) adds 
   more hit branches to be included in the Kalman analysis,
  here used as if private **/
  void AddHitBranch(unsigned int detId, const TString& m){fHitBranchNameMap[detId]=m;};

  /** public method SetPersistence(Bool_t opt) **/
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}

  /** public method SetField(FairField* f) to keep pointer to the B field **/
  void SetField(FairField* f){fField=f;}

  /** public method UseGeane(Bool_t f) to switch on the use of Geane **/
  void UseGeane(Bool_t f=kTRUE){fUseGeane=f;}

  /** public method UseMC(Bool_t f) to switch on init from mc or to 0 if not needed**/
  void UseMC(Bool_t f=kTRUE){fUseMC=f;}

  /** public method SetMinNuOfHits(Int_t nu) sets lower limit 
      for number of hits in a processable track **/
  void SetMinNofHits(Int_t nu=6){fMinNofHits=nu;}

  /** public method SetPDG(Int_t f) if useMC == kTRUE it is ignored **/
  void SetPDG(Int_t f=11){fPDG=f;}

  /** public method Init()
      fetches and creates all necessary arrays, creates Geane **/
  virtual InitStatus Init();

  /** public method Exec(Option_t* opt)
      creates TrackCands, find starting values for Kalman and creates Track objects **/
  virtual void Exec(Option_t* opt);


private:

  TClonesArray* fTrackArray;          ///< output array of genfit Tracks
  TClonesArray* fGemPointArray;       ///< input array of PndGemPoints (needed for initialization of parameters)
  TClonesArray* fGemTrackArray;       ///< input array of PndGemTracks to be processed by this task
  //  TClonesArray* fGemTrackMatchArray;  ///<input array of PndGemTrackMatches (needed for initialization) 
  TClonesArray* fMcArray;             ///<input array of MCTracks (needed for initialization) 
  TClonesArray* fHitArray;            ///<input array of PndGemHits

  std::map<unsigned int,TString> fHitBranchNameMap;   ///< map of relevant <detID,name of hit branch>
  std::map<unsigned int,TClonesArray*> fHitBranchMap; ///< map of relevant <detID, hit array>
  Int_t fMinNofHits;    ///< minimum number of hits to further process a track
  Bool_t fPersistence;    ///< persistance flag
  Bool_t fUseGeane;       ///< flag switching on/off use of Geane
  Bool_t fUseMC;          ///< flag switching on/off initialisation of track params from MC
  Int_t fPDG;  ///< hipothesis of PDG value 
    //It  (shouldn't be here)!!!!

  FairField*    fField;    ///< pointer to field to be used by Geane 
  FairGeanePro* fGeanePro; ///< pointer to Geane object


  ClassDef(PndGemPrepareKalmanTracks,1)

};

#endif
