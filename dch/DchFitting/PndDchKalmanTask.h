// -------------------------------------------------------------------------
// -----                   PndDchKalmanTask header file                -----
// -----                 Created 15.05.2008  by A. Wronska             -----
// -----   based on the recotasks/demo/DemoKalmanTask by S.Neubert     -----
// -------------------------------------------------------------------------

/** \class PndDchKalmanTask
 *  \author A.Wronska <aleksandra.wronska@uj.edu.pl>
 *  \date 15.05.2008
 *  \brief PndDchKalmanTask does Kalman fitting for forward tracks in dch
 **/

#ifndef PNDDCHKALMANTASK_HH
#define PNDDCHKALMANTASK_HH

// Pnd Headres
#include "FairTask.h"

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

class PndDchKalmanTask : public FairTask {
 public:
  
  /**  Constructor  **/
  PndDchKalmanTask();

  /**  Destructor **/
  ~PndDchKalmanTask();

  /** public method SetTrackBranchName(const TString& name) sets fTrackBranchName **/
  void SetTrackBranchName(const TString& name) {fTrackBranchName=name;}


  /** public method AddHitBranch(detID, hitBranchName) adds 
   more hit branches to be included in the Kalman analysis **/
  void AddHitBranch(unsigned int detId, const TString& m){fHitBranchMap[detId]=m;};

  /** public method SetPersistence(Bool_t opt) **/
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}

  /** public method SetNumIterations(Int_t i) **/
  void SetNumIterations(Int_t i){fNumIt=i;}
  
  /** public method Init() **/
  virtual InitStatus Init();

  /** public method Exec(Option_t* opt) **/
  virtual void Exec(Option_t* opt);

  /** public method Finish(Option_t* opt) **/
  virtual void Finish();
  
  /** public method WriteHistograms()
      writes out histograms with results of fitting into a file **/
  void WriteHistograms();

  /** public method PlotHistograms()
      allows a fast glance at the results of fitting **/
  void PlotHistograms();


private:

  TString fTrackBranchName;    ///< name of a track array to be processed (fitted), default="Track"
    std::map<unsigned int,TString> fHitBranchMap;  ///< map of <detID, name of corresponding hit array>
  TClonesArray* fTrackArray;   ///< pointer to array of 
  TClonesArray* fMCTrackArray; ///< pointer to array of MCTracks
  
  RecoHitFactory* fTheRecoHitFactory; ///< factory producing PndDchRecoHits of PndDchCylinderHits
  
  TH1D* fhP;             ///< momentum histogram;
  TH2D* fThetaH;         ///< theta histogram;
  TH2D* fPhiH;           ///< phi histogram;
  TH1D* fhChi2;          ///< chi2 histogram;
  TH2D* fChi2HitversusZ; ///< 2d histogram of chi2 versus z-coord.
  TCanvas* fCanvas;      ///< pointer to a canvas for drawing fit results
  
  Int_t fTrackCount;     ///< track counter
  Int_t fEvtCount;       ///< event counter
  Bool_t fPersistence;   ///< persistence flag
  Int_t fNumIt;          ///< number of iterations


public:
  ClassDef(PndDchKalmanTask,1)

};

#endif
