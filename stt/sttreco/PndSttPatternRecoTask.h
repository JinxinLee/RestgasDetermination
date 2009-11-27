#ifndef PNDSTTPATTERNRECOTASK_HH
#define PNDSTTPATTERNRECOTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"
#include<map>

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class TClonesArray;
class FairGeanePro;
class FairMCPoint;

class PndSttPatternRecoTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndSttPatternRecoTask();
  ~PndSttPatternRecoTask();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------

  // Operations ----------------------
  
  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);


private:

  // Private Data Members ------------
 
  TClonesArray* fTrackArray;
  TClonesArray* fMCTrackArray;
  TClonesArray* fMatchArray;
  TClonesArray* fSttTrackArray;
  TClonesArray* fSttHitArray;
  FairGeanePro *fPro;
  /** object persistence **/
  Bool_t fPersistence;

  // Private Methods -----------------

public:

  void SetPersistence(Bool_t persistence) { fPersistence = persistence; } 

  ClassDef(PndSttPatternRecoTask,1)

};

#endif
