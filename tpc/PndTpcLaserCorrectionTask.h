//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Correction for drift deviations 
//      Requires external data from LaserFitTask
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer    TUM              (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCLASERCORRECTIONTASK_H
#define TPCLASERCORRECTIONTASK_H

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --

class BiCubSpline;
class PndTpcDigiPar;
class TFile;


class PndTpcLaserCorrectionTask : public FairTask {
 
 public:

  // Constructors/Destructors ---------
  PndTpcLaserCorrectionTask();
  ~PndTpcLaserCorrectionTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetClusterBranchName(const TString& name) {_clusterBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetLaserRecoFile(const TString& recofile) {_recoFileName=recofile;}
  
  
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

private:

  // Private Data Members ------------
  TString _clusterBranchName;
  TString _backupBranchName;
  TString _recoFileName;
  TFile* _recoFile;
  
  TClonesArray* _clusterArray;
  TClonesArray* _backupArray;
  
  BiCubSpline* _recoMapR;
  BiCubSpline* _recoMapPerp;
  
  Bool_t _persistence;

  double _zMin, _zMax, _rMin, _rMax;

  PndTpcDigiPar* _par;
  
  
  // Private Methods -----------------

public:
  ClassDef(PndTpcLaserCorrectionTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
