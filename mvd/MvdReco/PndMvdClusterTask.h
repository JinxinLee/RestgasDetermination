
#ifndef PNDMVDCLUSTERTASK_H
#define PNDMVDCLUSTERTASK_H

#include "PndBlackBoxTask.h"

class TClonesArray;

class PndMvdClusterTask : public PndBlackBoxTask
  {
  public:
    
    /** Default constructor **/
    //     PndMvdClusterTask();
    
    PndMvdClusterTask();
    
    /** Destructor **/
    virtual ~PndMvdClusterTask();
    
    
    void SetPersistance(Bool_t p = kTRUE);
    Bool_t GetPersistance() {return fPersistance;};
    
  private:
    Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
    
    ClassDef(PndMvdClusterTask,2);
    
  };

#endif /* MVDCLUSTERTASK_H */

