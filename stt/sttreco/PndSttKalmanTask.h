#ifndef PNDSTTKALMANTASK_HH
#define PNDSTTKALMANTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"
#include "PndGeoSttPar.h"
// Collaborating Class Headers -------
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class GFRecoHitFactory;
class TGeoManager; 

class PndSttKalmanTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndSttKalmanTask();
  ~PndSttKalmanTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);
  
 void SetParContainers();
private:

  // Private Data Members ------------
  TString _trackBranchName;

  TClonesArray* fTrackArray;
  GFRecoHitFactory* _theRecoHitFactory;
  
 PndGeoSttPar *fSttParameters;  //  CHECK added

  // Private Methods -----------------

public:
  ClassDef(PndSttKalmanTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
