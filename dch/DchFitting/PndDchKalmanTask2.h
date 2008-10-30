// -------------------------------------------------------------------------
// -----                   PndDchKalmanTask2 header file               -----
// -----                 Created 15.05.2008  by A. Wronska             -----
// -----   based on the recotasks/demo/DemoKalmanTask by S.Neubert     -----
// -------------------------------------------------------------------------

/** \class PndDchKalmanTask2
 *  \author A.Wronska <aleksandra.wronska@uj.edu.pl>
 *  \date 15.05.2008
 *  \brief PndDchKalmanTask2 does Kalman fitting for forward tracks in dch
 **/

#ifndef PNDDCHKALMANTASK2_HH
#define PNDDCHKALMANTASK2_HH

// Pnd Headres
#include "CbmTask.h"

// ROOT Headers
#include "TString.h"

// c++ headers
#include <map>

class TClonesArray;
class RecoHitFactory;
class TH1D;
class TH2D;
class TCanvas;
class TGeoManager; 

class PndDchKalmanTask2 : public CbmTask {

 public:
  
  /**  Constructor  **/
  PndDchKalmanTask2();

  /**  Destructor **/
  ~PndDchKalmanTask2();

  /** public method SetTrackBranchName(const TString& name) sets fTrackBranchName **/
  void SetTrackBranchName(const TString& name) {fTrackBranchName=name;}

  /** public method AddHitBranch(detID, hitBranchName) adds 
   more hit branches to be included in the Kalman analysis **/
  void AddHitBranch(unsigned int detId, const TString& m){fHitBranchMap[detId]=m;};

  /** public method SetPersistence(Bool_t opt) **/
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}

  /** public method SetSmooth(Bool_t opt=kTRUE) to set the smoothing on/off **/
  void SetSmooth(Bool_t opt=kTRUE){fSmooth=opt;}

  /** public method SetNumIterations(Int_t i) **/
  void SetNumIterations(Int_t i){fNumIt=i;}
  
  /** public method Init() **/
  virtual InitStatus Init();

  /** public method Exec(Option_t* opt) **/
  virtual void Exec(Option_t* opt);

private:

  TString fTrackBranchName;    ///< name of a track array to be processed (fitted), default="Track"
  std::map<unsigned int,TString> fHitBranchMap;  ///< map of <detID, name of corresponding hit array>
  TClonesArray* fTrackArray;   ///< pointer to array of Track objects
  TClonesArray* fMCTrackArray; ///< pointer to array of MCTracks
  RecoHitFactory* fTheRecoHitFactory; ///< factory producing PndDchRecoHits of PndDchCylinderHits
  Int_t fTrackCount;     ///< track counter
  Int_t fEvtCount;       ///< event counter
  Bool_t fSmooth;        ///< smoothing flag
  Bool_t fPersistence;   ///< persistence flag
  Int_t fNumIt;          ///< number of iterations


public:
  ClassDef(PndDchKalmanTask2,1)

};

#endif
