#ifndef STTKALMANTASK_HH
#define STTKALMANTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class RecoHitFactory;
class TGeoManager; 

class SttKalmanTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  SttKalmanTask();
  ~SttKalmanTask();

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
  ClassDef(SttKalmanTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
