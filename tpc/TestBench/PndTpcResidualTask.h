//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Residual calculation for curved tracks
//      
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer           TUM      (original author)  
//      Johannes Rauch          TUM
//
//-----------------------------------------------------------

#ifndef TPCRESTASK_H
#define TPCRESTASK_H

//Base class
#include "FairTask.h"

#include "TClonesArray.h"
#include "TString.h"


// Collaborating Class Declarations --------------------------------------

class PndTpcCluster;
class PndTpcDigiPar;

class PndTpcResidualTask : public FairTask {

public:

// Constructor ----------------------------------------------

  PndTpcResidualTask();			//default constructor
  ~PndTpcResidualTask();
  
  virtual InitStatus Init();	 
  virtual void Exec(Option_t* opt);
  
  virtual void SetParContainers();
  
// Setters --------------------------------------------------
  
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;} 
  void SetClusterBranchName(TString cln) {fClusterBranchName=cln;}
  void SetTrackBranchName(TString tn) {fTrackBranchName=tn;}

  void SetSecondarySuppression(Bool_t opt=kTRUE) {fSecondarySupp=opt;}
  void SetNumberOfTrackReps(unsigned int n) {fNumReps=n;}
  

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

 std::map<unsigned int, TClonesArray*> fOutArrayMap;
 unsigned int fNumReps;

public:
  ClassDef(PndTpcResidualTask,1)
	
};

#endif
