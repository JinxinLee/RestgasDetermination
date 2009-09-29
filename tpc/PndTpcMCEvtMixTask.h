//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Task that mixes (copies) events from an other input file to the 
//      current data stream.
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCMCEVTMIXTASK_HH
#define TPCMCEVTMIXTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class TClonesArray;
class TFile;
class TTree;
class TBranch;

class PndTpcMCEvtMixTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcMCEvtMixTask();
  ~PndTpcMCEvtMixTask();

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetMCTrackBranchName(const TString& name) {fmcTrackBranchName=name;}
  void SetTpcPointBranchName(const TString& name) {ftpcPointBranchName=name;}
  void SetBkgFileName(const TString& name) {fbkgFileName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
  void SetMeanEvtSpacing(Double_t deltaT) {fmeanEvtSpacing=deltaT;} // [ns]
  void SetEvtRate(Double_t evt_per_sec) {fmeanEvtSpacing=1/evt_per_sec*1.E9;}
  void SetNBkgEvts(Int_t n) {fnbkgEvts=n;}
  void SetMinP(Double_t p){fminp=p;}
  void SetVDrift(double v){fvdrift=v;} // cm/ns

  // Operations ----------------------
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);

private:

  // Private Data Members ------------
  TString fmcTrackBranchName;
  TString ftpcPointBranchName;
  TString fbkgFileName;
 

  TClonesArray* fmcTrackArray;
  TClonesArray* fmcPointArray;
  TClonesArray* fbkgTrackArray;
  TClonesArray* fbkgPointArray;
  TClonesArray* ftimeArray;
  TClonesArray* ftrackArray;

  TFile* finFile;
  TTree* fbkgTree;
  TBranch* fbkgTrackBranch;
  TBranch* fbkgPointBranch;

  Bool_t fpersistence;

  Double_t fmeanEvtSpacing;
  Int_t fnbkgEvts;
  Double_t fminp; // lower p cut for reconstructed tracks
  Double_t fvdrift; // electron drift velocity

  // Private Methods -----------------
  void buildTracks(TClonesArray* tpcpoints, TClonesArray* mctracks,
	      Double_t evttime, Int_t evtId);


public:
  ClassDef(PndTpcMCEvtMixTask,1);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
