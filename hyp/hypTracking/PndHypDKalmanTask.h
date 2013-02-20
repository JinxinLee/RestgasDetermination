//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      GFKalman Filter Task
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PNDHYPDKALMANTASK_HH
#define PNDHYPDKALMANTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class GFRecoHitFactory;
class TH1D;
class TH2D;
class TGeoManager; 
class FairField;

class PndHypDKalmanTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndHypDKalmanTask();
  ~PndHypDKalmanTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {fTrackBranchName=name;}
  void AddHitBranch(unsigned int detId, const TString& m){fHitBranchMap[detId]=m;};
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}
  void SetSmooth(Bool_t opt=kTRUE){fSmooth=opt;}
  void UseMVD(bool mvd){fUseMVD=mvd;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);
  
  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TString fTrackBranchName;
  std::map<unsigned int,TString> fHitBranchMap;
  TClonesArray* fTrackArray;
  
  Bool_t fPersistence;

  GFRecoHitFactory* fTheRecoHitFactory;
  FairField* fField;

  TH1D* fPH; // momentum histo;
  TH1D* fChi2H; // chi2 histo;
  TH1D* fXresH;
  TH1D* fYresH;
  TH1D* fXresFitH;
  TH1D* fYresFitH;
  TH2D* fPEnd;

  int fTrackcount;
  Bool_t fSmooth;
  int fEvt; // event counter
  bool fUseMVD;

  // Private Methods -----------------

public:
  ClassDef(PndHypDKalmanTask,3)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
