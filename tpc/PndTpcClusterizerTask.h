//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      PndTpc Clusterizer reads in MCPoints and produces primary electrons
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCCLUSTERIZERTASK_HH
#define TPCCLUSTERIZERTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class PndTpcGas;
class PndTpcDigiPar;

class PndTpcClusterizerTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcClusterizerTask();
  ~PndTpcClusterizerTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void SetPointBranchName(const TString& name) {_pointBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  
    /**
  	if activeated there will be no Interpolation between hits
	This has to be set
	if the ALICE Monte Carlo is activeated in PndTpcDetector
  */
  void SetMereChargeConversion(Bool_t opt=kTRUE)	{_mereChargeConversion=opt;}


  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();
  
  void ChargeConversion();


private:

  // Private Data Members ------------
  
  
  TString _pointBranchName;
  TClonesArray* _pointArray;
  TClonesArray* _primArray;
  
  const PndTpcGas* _gas;
  PndTpcDigiPar* _par;

  Bool_t _persistence;
  Bool_t _mereChargeConversion;

  // Private Methods -----------------

public:
  ClassDef(PndTpcClusterizerTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
