// ---------------------------------------------------------------------------
// -----                   PndDchKalmanQATask2 header file               -----
// -----                 Created 30.10.2008  by A. Wronska               -----
// -----   based on the recotasks/demo/DemoTools by S.Neubert            -----
// ---------------------------------------------------------------------------


/** \class PndDchKalmanQATask
 *  \author A.Wronska <aleksandra.wronska@uj.edu.pl>
 *  \date 30.10.2008
 *  \brief PndDchKalmanQATask allows to judge performance of Kalman fitting 
 *         for forward tracks in dch
 **/

#ifndef PNDDCHKALMANQATASK_HH
#define PNDDCHKALMANQATASK_HH

// Panda Headers ----------------------
#include "CbmTask.h"

// ROOT Headers -----------------------
#include "TString.h"

class TClonesArray;
class TH1D;
class TH2D;
class TCanvas;

class PndDchKalmanQATask : public CbmTask {
public:

  /**  Constructor  **/
  PndDchKalmanQATask();

  /**  Destructor **/
  ~PndDchKalmanQATask();

  /** public method SetTrackBranchName(const TString& name) sets fTrackBranchName **/
  void SetTrackBranchName(const TString& name) {fTrackBranchName = name;}

  /** public method SetPersistence(Bool_t opt) **/
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}

  /** public method Init() **/
  virtual InitStatus Init();

  /** public method Exec(Option_t* opt) **/
  virtual void Exec(Option_t* opt);
  

  /** public method PlotHistograms()
      allows a fast glance at the results of fitting **/
  void PlotHistograms();

  /** public method Finish(Option_t* opt) **/
  virtual void Finish();
  

private:

  TString fTrackBranchName;         ///< name of a track array to be processed (fitted), default="Track"
  TClonesArray* fTrackArray;        ///< pointer to array of Track objects
  TClonesArray* fMCTrackArray;      ///< pointer to array of MCTracks
  TClonesArray* fDchTrackMatchArray;///< pointer to array of DchTrackMatches
  
  Bool_t fPersistence;   ///< persistence flag
  
  TH1D* fhP;             ///< momentum histogram;
  TH2D* fThetaH;         ///< theta histogram;
  TH2D* fPhiH;           ///< phi histogram;
  TH1D* fhChi2;          ///< chi2 histogram;
  TCanvas* fCanvas;      ///< pointer to a canvas for drawing fit results
  
  Int_t fTrackcount;       ///< track counter
  Int_t fEvt;              ///< event counter

  /** private method WriteHistograms()
      writes out histograms with results of fitting into a file **/
  Bool_t WriteHistograms();

public:
  ClassDef(PndDchKalmanQATask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
