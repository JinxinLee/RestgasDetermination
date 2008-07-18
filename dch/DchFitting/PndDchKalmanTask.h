
#ifndef PNDDCHKALMANTASK_HH
#define PNDDCHKALMANTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class RecoHitFactory;
class TH1D;
class TGeoManager; 

class PndDchKalmanTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndDchKalmanTask();
  ~PndDchKalmanTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {fTrackBranchName=name;}
  void AddHitBranch(unsigned int detId, const TString& m){fHitBranchMap[detId]=m;};
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}
  void SetSmooth(Bool_t opt=kTRUE){fSmooth=opt;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void Finish();
  
  void WriteHistograms();

private:

  // Private Data Members ------------
  TString fTrackBranchName;
  std::map<unsigned int,TString> fHitBranchMap;
  TClonesArray* fTrackArray;
  
  Bool_t fPersistence;

  RecoHitFactory* fTheRecoHitFactory;
  

  TH1D* fhP; // momentum histo;
  TH1D* fhChi2; // chi2 histo;
  
  Int_t fTrackCount;
  Int_t fEvtCount; // event counter
  Bool_t fSmooth;

  // Private Methods -----------------

public:
  ClassDef(PndDchKalmanTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
