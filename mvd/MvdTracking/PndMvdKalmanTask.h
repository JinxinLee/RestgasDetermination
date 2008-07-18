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
//      Ralf Kliemt, TU Dresden             (Copied for MVD use)
//
//-----------------------------------------------------------

#ifndef PNDMVDKALMANTASK_HH
#define PNDMVDKALMANTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"
#include "CbmGeanePro.h"

// Collaborating Class Declarations --
class TClonesArray;
class RecoHitFactory;
class TH1D;
class TGeoManager; 

class PndMvdKalmanTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndMvdKalmanTask();
  ~PndMvdKalmanTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {fTrackBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}
  

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);
  
  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TClonesArray* fTrackArray;
  TString fTrackBranchName;

  Bool_t fPersistence;

  RecoHitFactory* fTheRecoHitFactory;
  

  TH1D* fPH; // momentum histo;
  TH1D* fChi2H; // chi2 histo;
  TH1D* fMassV0; // chi2 histo;
  TH1D* fMassETAC; // chi2 histo;
  TH1D* fMasses; // all particle histo;

  Int_t fTrackcount;

  Int_t fEvent;
  TGeant3 *gMC3;
  CbmGeanePro *fPro;

  // Private Methods -----------------

public:
  ClassDef(PndMvdKalmanTask,1);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
