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
  void SetMCTrackBranchName(const TString& name) {_mcTrackBranchName=name;}
  void SetTpcPointBranchName(const TString& name) {_tpcPointBranchName=name;}
  void SetBkgFileName(const TString& name) {_bkgFileName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetMeanEvtSpacing(Double_t deltaT) {_meanEvtSpacing=deltaT;} // [ns]
  void SetEvtRate(Double_t evt_per_sec) {_meanEvtSpacing=1/evt_per_sec*1.E9;}
  void SetNBkgEvts(Int_t n) {_nbkgEvts=n;}
  void SetMinP(Double_t p){_minp=p;}
  void SetVDrift(double v){_vdrift=v;} // cm/ns

  // Operations ----------------------
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);

private:

  // Private Data Members ------------
  TString _mcTrackBranchName;
  TString _tpcPointBranchName;
  TString _bkgFileName;
 

  TClonesArray* _mcTrackArray;
  TClonesArray* _mcPointArray;
  TClonesArray* _bkgTrackArray;
  TClonesArray* _bkgPointArray;
  TClonesArray* _timeArray;
  TClonesArray* _trackArray;

  TFile* _inFile;
  TTree* _bkgTree;
  TBranch* _bkgTrackBranch;
  TBranch* _bkgPointBranch;

  Bool_t _persistence;

  Double_t _meanEvtSpacing;
  Int_t _nbkgEvts;
  Double_t _minp; // lower p cut for reconstructed tracks
  Double_t _vdrift; // electron drift velocity

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
