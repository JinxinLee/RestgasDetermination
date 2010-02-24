//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Residual calculation for straight line tracks
//      
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer           TUM      (original author)  
//
//-----------------------------------------------------------

#ifndef TPCSLRESTASK_H
#define TPCSLRESTASK_H

//Base class
#include "FairTask.h"

#include "TClonesArray.h"
#include "TString.h"


// Collaborating Class Declarations --------------------------------------

class PndTpcCluster;
class PndTpcDigiPar;

class PndTpcSLResidualTask : public FairTask {

public:

// Constructor ----------------------------------------------

  PndTpcSLResidualTask();			//default constructor
  ~PndTpcSLResidualTask();
  
  virtual InitStatus Init();	 
  virtual void Exec(Option_t* opt);
  
  virtual void SetParContainers();
  
// Setters --------------------------------------------------
  
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;} 
  void SetClusterBranchName(TString cln) {fClusterBranchName=cln;}
  void SetTrackBranchName(TString tn) {fTrackBranchName=tn;}

  void SetSecondarySuppression(Bool_t opt=kTRUE) {fSecondarySupp=opt;}
  

private:

 
 Bool_t fPersistence;
 Bool_t fSecondarySupp;

 TString fClusterBranchName;
 TString fTrackBranchName;

 TClonesArray* fClusterArray;
 TClonesArray* fTrackArray;
 TClonesArray* fResArray;

 TClonesArray* fFitStatArray;
 
 PndTpcDigiPar* fPar;

public:
  ClassDef(PndTpcSLResidualTask,1)
	
};

#endif
