//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Correction of cluster positions based on fit of 
//      aquired data
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCCLUSTERCORRTASK_H
#define TPCCLUSTERCORRTASK_H

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "PndTpcCluster.h"
class TClonesArray;
class PndTpcDigiPar;
class PndTpcPadPlane;
class TF1;

class PndTpcClusterCorrectionTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcClusterCorrectionTask();
  ~PndTpcClusterCorrectionTask();

  // Operators
  
  // Accessors -----------------------


  // Modifiers -----------------------
  void SetClusterBranchName(const TString& name) {_clusterBranchName=name;}
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void GetParameters(std::vector<double>) const;
  void SetParameters(double* parArr);

 
  // Operations ----------------------
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();
  

  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  TString _clusterBranchName;
  TString _clusterOutName;
  TClonesArray* _clusterArray;
  TClonesArray* _outArray;
  
  Bool_t _persistence;
  bool _parSet;  //flag if corrFunc's pars have been set
  
  std::vector<double> _pars;  //coefficients of 5th order polynomial 
                              //used fpr cluster position correction

  PndTpcDigiPar* fpar;
  PndTpcPadPlane* fpadplane;

  TF1* corrFunc;
  
  // Private Methods -----------------

public:
  ClassDef(PndTpcClusterCorrectionTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
