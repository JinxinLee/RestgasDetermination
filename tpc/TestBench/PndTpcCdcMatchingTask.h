//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      TPC-CDC Matching Routine
//      
//
// Environment:
//      Software developed for the Prototype Detector at FOPI
//
// Author List:
//      Robert Münzer          TUM
//
//-----------------------------------------------------------

#ifndef TPCCDCMATTASK_H
#define TPCCDCMATTASK_H

//Base class
#include "FairTask.h"

#include "TClonesArray.h"
#include "TString.h"
#include "TVector3.h"
#include "FairGeanePro.h"
class PndTpcDigiPar;

// Collaborating Class Declarations --------------------------------------

class PndTpcCdcMatchingTask : public FairTask {

public:

// Constructor ----------------------------------------------

  PndTpcCdcMatchingTask();			//default constructor
  ~PndTpcCdcMatchingTask();
  
  virtual InitStatus Init();	 
  virtual void Exec(Option_t* opt);
  
  virtual void SetParContainers();
  
// Modifiers --------------------------------------------------
  
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;} 
  void SetTpcClusterBranchName(TString cln) {fTpcClusterBranchName=cln;}
  void SetTpcTrackBranchName(TString tn) {fTpcTrackBranchName=tn;}
  void SetCdcHitBranchName(TString cln) {fCdcHitBranchName=cln;}
  void SetCdcTrackBranchName(TString tn) {fCdcTrackBranchName=tn;}
  void SetSecondarySuppression(Bool_t opt=kTRUE) {fSecondarySupp=opt;}
  void SetNumberOfTrackReps(unsigned int n) {fNumReps=n;}
  void SetMaxMatDistance(double max) {fmaxmatdistance=max;}
  void SetMaxMatPhi(double max) {fmaxmatphi=max;}
  void SetMinHitsperLength(double mhpl) {fminhitsperlength=mhpl;}

private:

 
 Bool_t fPersistence;
 Bool_t fSecondarySupp;

 TString fTpcClusterBranchName;
 TString fTpcTrackBranchName;
 TString fCdcTrackBranchName;
 TString fCdcHitBranchName;
 TString fCdcEvBranchName;

 TClonesArray* fTpcClusterArray;
 TClonesArray* fTpcTrackArray;
 TClonesArray* fCdcTrackArray; 
 TClonesArray* fCdcHitArray;
 TClonesArray* fCdcEventArray;
 TClonesArray* fCdcTpcMatArray;
 TClonesArray* fCdcTpcMatPreFitOutArray;
 TClonesArray* fCdcTpcMatArrayOutArray;

 TClonesArray* fFitStatArray;

 Int_t fmatablecdctracks;
 Int_t fmatchedcdctracks;

 Double_t fmaxmatdistance;
 Double_t fmaxmatphi;
 Double_t fminhitsperlength;
 
 PndTpcDigiPar* fPar;

 FairGeanePro* gPro;

 std::map<unsigned int, TClonesArray*> fOutArrayMap;
 unsigned int fNumReps;

public:
  ClassDef(PndTpcCdcMatchingTask,3)
	
};

#endif
