#ifndef PNDSTTPATTERNRECOTASK2_HH
#define PNDSTTPATTERNRECOTASK2_HH

// Base Class Headers ----------------
#include "CbmTask.h"
#include<map>

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class TClonesArray;
class CbmGeanePro;
class CbmMCPoint;

class PndSttPatternRecoTask2 : public CbmTask {
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
  CbmGeanePro *fPro;
  // Private Methods -----------------

public:
  ClassDef(PndSttPatternRecoTask2,1)

};

#endif
