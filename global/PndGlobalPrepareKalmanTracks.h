// -------------------------------------------------------------------------
// -----          PndGlobalPrepareKalmanTracks  header file            -----
// -----            Created 13.01.2010  by R. Karabowicz               -----
// -------------------------------------------------------------------------

/** \class PndGlobalPrepareKalmanTracks
 *  \author Radoslaw Karabowicz <r.karabowicz@gsi.de> for Global on 13.01.2010
 *  \brief PndGlobalPrepareKalmanTracks creates and initializes Tracks and is a pre-step for genfit
 **/
#ifndef PNDGLOBALPREPAREKALMANTRACKS_H
#define PNDGLOBALPREPAREKALMANTRACKS_H

// Pnd Headres
#include "FairTask.h"

// c++ headers
#include <map>

class TClonesArray;
class RecoHitFactory;
class AbsBFieldIfc;
class FairField;
class FairGeanePro;

class PndGlobalPrepareKalmanTracks : public FairTask {
public:

  /**  Constructor  **/
  PndGlobalPrepareKalmanTracks();

  /**  Destructor **/
  ~PndGlobalPrepareKalmanTracks();

  /** public method SetPersistence(Bool_t opt) **/
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}

  /** public method SetField(FairField* f) to keep pointer to the B field **/
  void SetField(FairField* f){fField=f;}

  /** public method UseGeane(Bool_t f) to switch on the use of Geane **/
  void UseGeane(Bool_t f=kTRUE){fUseGeane=f;}

  /** public method UseMC(Bool_t f) to switch on init from mc or to 0 if not needed**/
  void UseMC(Bool_t f=kTRUE){fUseMC=f;}

  /** public method SetMinNofHits(Int_t nu) sets lower limit 
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

  TClonesArray* fOutputTrackArray;          ///< output array of genfit Tracks

  TClonesArray* fGlobalTrackArray;    ///< input array of global PndTrackCands to be processed by this task

  Int_t fMinNofHits;    ///< minimum number of hits to further process a track

  TClonesArray* fMcArray;             ///<input array of MCTracks (needed for initialization) 
  Bool_t        fUseMC;         ///< whether to use MC information on particle code

  Bool_t fPersistence;    ///< persistance flag
  Bool_t fUseGeane;       ///< flag switching on/off use of Geane
  Int_t  fPDG;            ///< hipothesis of PDG value 

  FairField*    fField;    ///< pointer to field to be used by Geane 
  FairGeanePro* fGeanePro; ///< pointer to Geane object


public:
  ClassDef(PndGlobalPrepareKalmanTracks,1)

};

#endif
