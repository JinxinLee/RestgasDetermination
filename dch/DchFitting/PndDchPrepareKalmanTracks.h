
#ifndef PNDDCHPREPAREKALMANTRACKS_HH
#define PNDDCHPREPAREKALMANTRACKS_HH

// Base Class Headers ----------------
#include "CbmTask.h"
#include <map>

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class RecoHitFactory;
class AbsBFieldIfc;
class CbmField;
class CbmGeanePro;

class PndDchPrepareKalmanTracks : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndDchPrepareKalmanTracks();
  ~PndDchPrepareKalmanTracks();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void AddHitBranch(unsigned int detId, const TString& m){fHitBranchNameMap[detId]=m;};
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}
  void SetField(CbmField* f){fField=f;}
  void UseGeane(bool f=true){fUseGeane=f;}

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
  TClonesArray* fTrackArray;
  TClonesArray* fDchTrackArray;
  TClonesArray* fDchTrackMatchArray;
  TClonesArray* fMcArray;
  TClonesArray* fCHitArray;

  std::map<unsigned int,TString> fHitBranchNameMap;
  std::map<unsigned int,TClonesArray*> fHitBranchMap;
  
  Bool_t fPersistence;
  Bool_t fUseGeane;

  CbmField*    fField;
  CbmGeanePro* fGeanePro;

  // Private Methods -----------------

public:
  ClassDef(PndDchPrepareKalmanTracks,1)

};

#endif
