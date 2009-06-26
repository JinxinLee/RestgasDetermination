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
#include "FairTask.h"

// ROOT Headers -----------------------
#include "TString.h"

class TClonesArray;
class TH1D;
class TH1F;
class TH2D;
class TCanvas;

class PndDchKalmanQATask : public FairTask {
public:

  /**  Constructor  **/
  PndDchKalmanQATask();

  /**  Destructor **/
  ~PndDchKalmanQATask();

  /** public method SetTrackBranchName(const TString& name) sets fTrackBranchName **/
  void SetTrackBranchName(const TString& name) {fTrackBranchName = name;}

  /** public method SetPersistence(Bool_t opt) **/
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}

  /** public method SetPersistence(Bool_t opt) **/
  void SetApproach(Int_t approach) {fApproach=approach;}

  /** public method ApplySignPatch(...) sets a flag deciding whether
      or not to revert wrongly reconstructed sign of momentum components **/
  void ApplySignPatch(Bool_t patch){fSignPatch=patch;}

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
  TClonesArray* fPointArray;      ///< pointer to array of PndDchPoints
  TClonesArray* fDchTrackMatchArray;///< pointer to array of DchTrackMatches
  
  Bool_t fPersistence;   ///< persistence flag
    
    TH1D* fhP;             ///< momentum pull distribution;
    TH1D* fhPx;             ///< x-momentum pull distribution;
    TH1D* fhPy;             ///< y-momentum pull distribution;
    TH1D* fhPz;             ///< z-momentum pull distribution;
    TH2D* fThetaH;         ///< theta pull distribution;
    TH2D* fPhiH;           ///< phi pull distribution;
    TH1D* fhChi2;          ///< chi2 pull distribution;
    TH1F* fHrp;///< momentum residual;
    TH1F* fHrpx; ///< residual of px;
    TH1F* fHrpy; ///< residual of py;
    TH1F* fHrpz; ///< residual of pz;
    TH1F* fHrtv; ///< residual of txz (tv);
    TH1F* fHrtw; ///< residual of tyz (tw);
  TCanvas* fCanvas;      ///< pointer to a canvas for drawing fit results;
  
  Int_t fTrackcount;       ///< track counter;
  Int_t fEvt;              ///< event counter;
  Int_t fApproach;       ///< choice of where to compare track parameters with MC;
  Bool_t fSignPatch;     ///< flag for correcting wrongly reconstructed sign of p components;

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
