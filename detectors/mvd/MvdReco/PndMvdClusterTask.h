
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
    
    void SetPersistance(Bool_t pers);

  private:
    
    ClassDef(PndMvdClusterTask,2);
    
  };

#endif /* MVDCLUSTERTASK_H */

