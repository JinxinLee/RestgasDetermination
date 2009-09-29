//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
// Attaches an event time to the event
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCEVTTIMEGENTASK_HH
#define TPCEVTTIMEGENTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class TClonesArray;
class TFile;
class TTree;
class TBranch;

class PndTpcEvtTimeGenTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcEvtTimeGenTask();
  ~PndTpcEvtTimeGenTask();

  // Operators
  
  // Accessors -----------------------
  Double_t MeanEvtSpacing() const {return fmeanEvtSpacing;}

  // Modifiers -----------------------
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
  void SetMeanEvtSpacing(Double_t deltaT) {fmeanEvtSpacing=deltaT;} // [ns]
  void SetEvtRate(Double_t evt_per_sec) {fmeanEvtSpacing=1/evt_per_sec*1.E9;}
  void SetT0(Double_t t0) {ft0=t0;}

  // Operations ----------------------
  virtual InitStatus Init();
  
  virtual void Exec(Option_t* opt);

private:


  TClonesArray* ftimeArray;

  Bool_t fpersistence;

  Double_t fmeanEvtSpacing;
  Double_t ft0;
  Double_t ftevent;
  unsigned int feventid;

  // Private Methods -----------------

public:
  ClassDef(PndTpcEvtTimeGenTask,1);

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
