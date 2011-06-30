//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Performs Eventdeconvoluiton of Mixed TPC data  
//      based on 
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCEVTDECONVTASK_HH
#define TPCEVTDECONVTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "PndTpcRiemannTrack.h"


class TClonesArray;
class TH1D;


class PndTpcEvtDeconvTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcEvtDeconvTask();
  virtual ~PndTpcEvtDeconvTask();

  // Operators
  
  // Accessors -----------------------
  
  
  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {fTrackBranchName=name;}
  void SetOutTrackBranchName(const TString& name) {fOutTrackBranchName=name;}
  
    
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}
  void SetCuts(Double_t rcut, Double_t zcut){fRCut=rcut;fZCut=zcut;}

  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  void WriteHistograms(const TString& fname) const;
  
  
 private:

   // Private Data Members ------------
  TString fTrackBranchName;
  TString fOutTrackBranchName;
   
  Bool_t fPersistence;
  Double_t fRCut;
  Double_t fZCut;
   
  TClonesArray* fTrackArray;
  TClonesArray* fOutTrackArray;



  ClassDef(PndTpcEvtDeconvTask,1);
  
};

#endif

