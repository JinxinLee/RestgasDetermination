#ifndef PNDSTTPATTERNRECOTASK_HH
#define PNDSTTPATTERNRECOTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"
#include<map>

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class TClonesArray;
class CbmGeanePro;
class CbmMCPoint;

class PndSttPatternRecoTask : public CbmTask {
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
  CbmGeanePro *fPro;
  // Private Methods -----------------

public:
  ClassDef(PndSttPatternRecoTask,1)

};

#endif
