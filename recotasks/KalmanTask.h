//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Kalman Filter Task
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef KALMANTASK_HH
#define KALMANTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class GFRecoHitFactory;
class TH1D;
class TGeoManager; 

class KalmanTask : public FairTask {
public:

  // Constructors/Destructors ---------
  KalmanTask();
  ~KalmanTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {_trackBranchName=name;}
  void SetOutBranchName(const TString& name) {_outBranchName=name;}
  void SetPixelBranchName(const TString& name) {fMVDPixelBranchName=name;}
  void SetStripBranchName(const TString& name) {fMVDStripBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetLazy(Bool_t opt){_lazy=opt;}
  void SetNumIterations(Int_t it){_numIt=it;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);
  
  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TClonesArray* _trackArray;
  TClonesArray* _trackOutArray;
  TString _trackBranchName;
  TString _outBranchName;
  TString fMVDPixelBranchName;
  TString fMVDStripBranchName;

  Bool_t _persistence;
  Bool_t _lazy;
  Int_t _numIt; // number of fitting iterations (back and forth)

  GFRecoHitFactory* _theRecoHitFactory;
  

  TH1D* _pH; // momentum histo;
  TH1D* _chi2H; // chi2 histo;
  TH1D* _massV0; // chi2 histo;
  TH1D* _massETAC; // chi2 histo;

  int _trackcount;

  // Private Methods -----------------

public:
  ClassDef(KalmanTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
