// -------------------------------------------------------------------------
// -----          PndDchPrepareKalmanTracks2 header file               -----
// -----            Created 30.09.2008  by A. Wronska                  -----
// -----        based on the recotasks/demo code  by S.Neubert         -----
// -------------------------------------------------------------------------

/** \class PndDchPrepareKalmanTracks2
 *  \author A.Wronska <aleksandra.wronska@uj.edu.pl>
 *  \date 15.05.2008
 *  \brief PndDchPrepareKalmanTracks2 creates and initializes Tracks and is a pre-step for genfit
 **/
#ifndef PNDDCHPREPAREKALMANTRACKS2_HH
#define PNDDCHPREPAREKALMANTRACKS2_HH

// Pnd Headres
#include "CbmTask.h"

// c++ headers
#include <map>

class TClonesArray;
class RecoHitFactory;
class AbsBFieldIfc;
class CbmField;
class CbmGeanePro;

class PndDchPrepareKalmanTracks2 : public CbmTask {
public:

  /**  Constructor  **/
  PndDchPrepareKalmanTracks2();

  /**  Destructor **/
  ~PndDchPrepareKalmanTracks2();

  /** public method AddHitBranch(detID, hitBranchName) adds 
   more hit branches to be included in the Kalman analysis,
  here used as if private **/
  void AddHitBranch(unsigned int detId, const TString& m){fHitBranchNameMap[detId]=m;};

  /** public method SetPersistence(Bool_t opt) **/
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}

  /** public method SetField(CbmField* f) to keep pointer to the B field **/
  void SetField(CbmField* f){fField=f;}

  /** public method UseGeane(Bool_t f) to switch the use of Geane **/
  void UseGeane(Bool_t f=true){fUseGeane=f;}

  /** public method Init()
      fetches and creates all necessary arrays, creates Geane **/
  virtual InitStatus Init();

  /** public method Exec(Option_t* opt)
      creates TrackCands, find starting values for Kalman and creates Track objects **/
  virtual void Exec(Option_t* opt);


private:

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

  CbmField*    fField;    ///< pointer to field to be used by Geane 
  CbmGeanePro* fGeanePro; ///< pointer to Geane object


public:
  ClassDef(PndDchPrepareKalmanTracks2,1)

};

#endif
