
#ifndef PNDBLACKBOXTASK_H
#define PNDBLACKBOXTASK_H

#include "PndBranchTask.h"

class TClonesArray;

class PndBlackBoxTask : public PndBranchTask
  {
  public:
    PndBlackBoxTask(const char* name);
    virtual ~PndBlackBoxTask();
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();
    virtual void Exec(Option_t* opt);
    void SetVerbose(Int_t iVerbose);
    
  private:
    
    ClassDef(PndBlackBoxTask,1);
    
  };

#endif
