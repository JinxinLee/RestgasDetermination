#ifndef PNDSTTPATTERNRECOTASK2_HH
#define PNDSTTPATTERNRECOTASK2_HH

// Base Class Headers ----------------
#include "FairTask.h"
#include<map>

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class TClonesArray;
class FairGeanePro;
class FairMCPoint;

class PndSttPatternRecoTask2 : public FairTask {
public:

  // Constructors/Destructors ---------
  PndSttPatternRecoTask2();
  ~PndSttPatternRecoTask2();

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
  TClonesArray* fSttHelixHitArray;
  FairGeanePro *fPro;
  /** object persistence **/
  Bool_t fPersistence;
  // Private Methods -----------------

public:

  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

  ClassDef(PndSttPatternRecoTask2,1)

};

#endif
