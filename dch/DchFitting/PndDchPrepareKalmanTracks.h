// -------------------------------------------------------------------------
// -----          PndDchPrepareKalmanTracks header file                -----
// -----            Created 15.05.2008  by A. Wronska                  -----
// -----        based on the recotasks/demo code  by S.Neubert         -----
// -------------------------------------------------------------------------

/** \class PndDchPrepareKalmanTracks
 *  \author A.Wronska <aleksandra.wronska@uj.edu.pl>
 *  \date 15.05.2008
 *  \brief PndDchPrepareKalmanTracks creates and initializes Tracks and is a pre-step for genfit
 **/
#ifndef PNDDCHPREPAREKALMANTRACKS_HH
#define PNDDCHPREPAREKALMANTRACKS_HH

// Pnd Headres
#include "FairTask.h"

// c++ headers
#include <map>

class TClonesArray;
class GFRecoHitFactory;
class AbsBFieldIfc;
class FairField;
class FairGeanePro;

class PndDchPrepareKalmanTracks : public FairTask {
public:

  /**  Constructor  **/
  PndDchPrepareKalmanTracks();

  /**  Destructor **/
  ~PndDchPrepareKalmanTracks();

  /** public method AddHitBranch(detID, hitBranchName) adds 
   more hit branches to be included in the Kalman analysis,
  here used as if private**/
  void AddHitBranch(unsigned int detId, const TString& m){fHitBranchNameMap[detId]=m;};

  /** public method SetPersistence(Bool_t opt) **/
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}

  /** public method SetField(FairField* f) to keep pointer to the B field **/
  void SetField(FairField* f){fField=f;}

  /** public method UseGeane(Bool_t f) to switch the use of Geane **/
  void UseGeane(Bool_t f=true){fUseGeane=f;}

  /** public method Init()
      fetches and creates all necessary arrays, creates Geane **/
  virtual InitStatus Init();

  /** public method Exec(Option_t* opt)
      creates GFTrackCands, find starting values for Kalman and creates Track objects **/
  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  TClonesArray* fTrackArray;          ///< output array of genfit Tracks
  TClonesArray* fDchPointArray;       ///< input array of PndDchPoints (needed for initialization of parameters)
  TClonesArray* fDchTrackArray;       ///< input array of PndDchTracks to be processed by this task
  TClonesArray* fDchTrackMatchArray;  ///<input array of PndDchTrackMatches (needed for initialization) 
  TClonesArray* fMcArray;             ///<input array of MCTracks (needed for initialization) 
  TClonesArray* fCHitArray;           ///<input array of PndDchCylinderHits

  std::map<unsigned int,TString> fHitBranchNameMap;   ///< map of relevant <detID,name of hit branch>
  std::map<unsigned int,TClonesArray*> fHitBranchMap; ///< map of relevant <detID, hit array>
  
  Bool_t fPersistence;    ///< persistance flag
  Bool_t fUseGeane;       ///< flag switching on/off use of Geane

  FairField*    fField;    ///< pointer to field to be used by Geane 
  FairGeanePro* fGeanePro; ///< pointer to Geane object

  // Private Methods -----------------

public:
  ClassDef(PndDchPrepareKalmanTracks,1)

};

#endif
