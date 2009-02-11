#ifndef PNDSTTKALMANTASK_HH
#define PNDSTTKALMANTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class RecoHitFactory;
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
  
  void AddHitBranch(unsigned int detId, const TString& m);

private:

  // Private Data Members ------------
  TString _trackBranchName;
  std::map<unsigned int,TString> _hitBranchMap;
  TClonesArray* fTrackArray;
  RecoHitFactory* _theRecoHitFactory;
  


  // Private Methods -----------------

public:
  ClassDef(PndSttKalmanTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
