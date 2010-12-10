#ifndef PNDSTTKALMANTASK2_HH
#define PNDSTTKALMANTASK2_HH

// Base Class Headers ----------------
#include "FairTask.h"
#include "PndGeoSttPar.h"
// Collaborating Class Headers -------
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class GFRecoHitFactory;
class TGeoManager; 

class PndSttKalmanTask2 : public FairTask {
public:

  // Constructors/Destructors ---------
  PndSttKalmanTask2();
  ~PndSttKalmanTask2();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);
  
  void AddHitBranch(unsigned int detId, const TString& m);
 void SetParContainers();
private:

  // Private Data Members ------------
  TString _trackBranchName;

  TClonesArray* fTrackArray;
  //  TClonesArray *fSttHelixHitArray;
  GFRecoHitFactory* _theRecoHitFactory;
  int counterevt;
 PndGeoSttPar *fSttParameters;  //  CHECK added

  // Private Methods -----------------

public:
  ClassDef(PndSttKalmanTask2,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
