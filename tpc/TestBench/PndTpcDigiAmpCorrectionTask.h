//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      pad-wise Digi amplitude correction 
//      based on gain calibration measurement
//      
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer           TUM      (original author)  
//
//-----------------------------------------------------------

#ifndef TPCAMPCORRECTION_H
#define TPCAMPCORRECTION_H

//Base class
#include "FairTask.h"

#include "TClonesArray.h"
#include "TString.h"

#include <map>

// Collaborating Class Declarations --------------------------------------

class PndTpcDigi;
class PndTpcDigiPar;

class PndTpcDigiAmpCorrectionTask : public FairTask {

public:

// Constructor ----------------------------------------------

  PndTpcDigiAmpCorrectionTask();			//default constructor
  ~PndTpcDigiAmpCorrectionTask() {;}
  
  virtual InitStatus Init();	 
  virtual void Exec(Option_t* opt);
  
  virtual void SetParContainers();
  
// Setters --------------------------------------------------
  
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;} 
  void SetDigiBranchName(TString d) {fDigiBranchName=d;}
  void SetExternalInput(TString file) {fAsciiInput=file; fInputSet=true; readFile();}
   

private:

  void readFile();

  Bool_t fPersistence;
  Bool_t fInputSet;
  
  TString fDigiBranchName;
  TString fAsciiInput;
  
  TClonesArray* fDigiArr;
  
  PndTpcDigiPar* fPar;
  
  std::map<unsigned int, double> fCorrMap;  // < padID, correction factor >


public:
  ClassDef(PndTpcDigiAmpCorrectionTask,1)
	
};

#endif
