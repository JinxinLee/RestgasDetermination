//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Task that mixes (copies) events from an other input file to the 
//      current data stream. Only PndTpcSignals are added for the moment
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCEVTMIXTASK_HH
#define TPCEVTMIXTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <vector>
#include <set>

#include "PndTpcDigi.h"
#include "PndTpcEvtTime.h"
// Collaborating Class Declarations --
class TClonesArray;
class TFile;
class TTree;
class TBranch;
class PndTpcPadPlane;
class PndTpcDigiPar;
class PndTpcFrontend;
class PndTpcGem;
class PndTpcGas;

class PndTpcEvtMixTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcEvtMixTask();
  ~PndTpcEvtMixTask();

  // Operators
  
  // Accessors -----------------------
  Double_t MeanEvtSpacing() const {return fmeanEvtSpacing;}

  // Modifiers -----------------------
  void SetInBranchName(const TString& name) {finBranchName=name;}
  void SetBkgBranchName(const TString& name) {fbkgBranchName=name;}
  void SetBkgFileName(const TString& name) {fbkgFileName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
  void SetNBkgEvts(Int_t n) {fnbkgEvts=n;}
  void AddSector(UInt_t id){fsectors.insert(id);}
  void SetMeanEvtSpacing(Double_t deltaT) {fmeanEvtSpacing=deltaT;fdoTimeSim=true;} // [ns]
  void SetEvtRate(Double_t evt_per_sec) {fmeanEvtSpacing=1/evt_per_sec*1.E9;fdoTimeSim=true;}
  void SetT0(Double_t t0) {ft0=t0;fdoTimeSim=true;}
  
  // Operations ----------------------
  virtual InitStatus Init();
  virtual void SetParContainers();
  virtual void Exec(Option_t* opt);
  virtual void FinishTask();

private:

  // Private Data Members ------------
  TString finBranchName;
  TString fbkgBranchName;
  TString fbkgFileName;
 

  TClonesArray* fsignalArray;
  TClonesArray* fOutArray;
  TClonesArray* fbkgArray;
  TClonesArray* ftimeArray;
  TClonesArray* ftimeOutArray;

  TFile* finFile;
  TTree* fbkgTree;
  
  TBranch* fbkgBranch;
  TBranch* ftimeBranch;

  std::vector<std::vector<PndTpcDigi>*>* fDigiVectors;
  std::vector<PndTpcEvtTime> fEvtTimes;

  Bool_t fpersistence;
  Bool_t fdoSignals;
  Int_t fnbkgEvts;
  Int_t fnAvailableBkgEvents;
  Bool_t fdoTimeSim;
  Double_t fmeanEvtSpacing;
  Double_t ft0;
  Int_t fevtCount;

  std::set<unsigned int> fsectors;
  PndTpcDigiPar* fpar;

   PndTpcPadPlane* fpadPlane;
  const PndTpcFrontend* ffrontend;
  const PndTpcGem* fgem;
  const PndTpcGas* fgas;
  double fzGem;
  // Private Methods -----------------

public:
  ClassDef(PndTpcEvtMixTask,1);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
