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
  void SetClusterBranchName(const TString& name) {fclusterBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
  void SetLaserRecoFile(const TString& recofile) {frecoFileName=recofile;}
  
  
  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();

private:

  // Private Data Members ------------
  TString fclusterBranchName;
  TString fbackupBranchName;
  TString frecoFileName;
  TFile* frecoFile;
  
  TClonesArray* fclusterArray;
  TClonesArray* fbackupArray;
  
  BiCubSpline* frecoMapR;
  BiCubSpline* frecoMapPerp;
  
  Bool_t fpersistence;

  double fzMin, fzMax, frMin, frMax;

  PndTpcDigiPar* fpar;
  
  
  // Private Methods -----------------

public:
  ClassDef(PndTpcLaserCorrectionTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
