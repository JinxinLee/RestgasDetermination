//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      GENFIT-based dEdx
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer, Christian Hoeppner   TUM
//
//
//-----------------------------------------------------------

#ifndef DEDXTASK_HH
#define DEDXTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class TH1D;
class PndTpcDigiPar;

class PndTpcdEdxTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcdEdxTask();
  ~PndTpcdEdxTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetTrackBranchName(const TString& name) {_trackBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetIdealdEdx(Bool_t opt=kTRUE) {_idealdEdx=opt;}
  void SetDXgrid(double dx) {_DX = dx;}
  

  // Operations ----------------------
  
  virtual InitStatus Init();
  
  virtual void SetParContainers();

  virtual void Exec(Option_t* opt);
  
  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TClonesArray* _trackArray;
  TClonesArray* _dEdxOutArray;
  TClonesArray* _dEdxMCOutArray;  
  TClonesArray* _mcTrackArray;
  TClonesArray* _pointArray;
  TClonesArray* _clusterArray;

  TString _trackBranchName;

  Bool_t _persistence;
  Bool_t _idealdEdx;
  double _DX;                       //dx slice size
  
  TH1D* _distHist;
  TH1D* _dirHist;

  PndTpcDigiPar* _par;

  

  // Private Methods -----------------

public:
  ClassDef(PndTpcdEdxTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
