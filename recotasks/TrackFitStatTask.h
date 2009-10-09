//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      example task for the usage of the genfit tracking farmework
//      inside FAIRRoot
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TRACKFITSTATTASK_HH
#define TRACKFITSTATTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include "GFTrack.h"

#include "TApplication.h"

// Collaborating Class Declarations --
class TClonesArray;
class TGeoManager;
class TH1D;

class TrackFitStatTask : public FairTask {
public:

  // Constructors/Destructors ---------
  TrackFitStatTask();
  virtual ~TrackFitStatTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void SetTrackBranchName(const TString& name) {_trackBranchName=name;}
  void SetMCBranchName(const TString& name){_mcBranchName=name;}
  void SetMCPCut(double dp_p){_mcPCut=dp_p;}
  void SetMCCuts(Double_t pmin, Double_t pmax, 
		 Double_t thetamin, Double_t thetamax,
		 Int_t minTPChits);
  void SetPdgSelection(Int_t pdgcode){_pdgId=pdgcode;_pdgselect=true;}
  void DoResiduals(Bool_t opt=kTRUE) {_doRes=opt;}
  void SetPrecoTol(Double_t precotol)	{ _precotol=precotol;}	
  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  //TApplication app;
  TString _trackBranchName;
  TString _mcBranchName;
  TString _mcPndTpcBranchName;
  TClonesArray* _trackArray;
  TClonesArray* _mcTrackArray;
  TClonesArray* _mcPndTpcHitArray;
  TClonesArray* _fitstatArray;
  TClonesArray* _mcAnnexArray;

  Bool_t _persistence;
  Bool_t _doRes;
  // mc-truth cuts (for efficiency definition)
  Double_t _mcPCut; // in sigma of (parameterized) dp/p resolution
  Double_t _pmin;
  Double_t _pmax;
  Double_t _thetamin;
  Double_t _thetamax;
  Int_t _minPndTpcHits;
  Int_t _pdgId;
  bool _pdgselect;
  Double_t _precotol;

  TH1D* _efficiency;
  TH1D* _trklengthtpc;
  // Private Methods -----------------

public:
  ClassDef(TrackFitStatTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
