
#ifndef PNDEVENTCOUNTERTASK_H
#define PNDEVENTCOUNTERTASK_H

#include "FairTask.h"

class TClonesArray;

class PndEventCounterTask : public FairTask
  {
  public:
    PndEventCounterTask(const char* name);
    virtual ~PndEventCounterTask();
    virtual void SetParContainers() {return;}
    virtual InitStatus Init(){return kSUCCESS;}
    virtual InitStatus ReInit(){return kERROR;}
    virtual void Exec(Option_t* opt);
    void SetVerbose(Int_t iVerbose) {fVerbose=iVerbose;}
    void SetAnnounce(Int_t iann) {if(iann>0)fEvtTalk=iann;}
    Int_t GetEventCount() const {return fEvtCounter;}
    
  private:
    Int_t fEvtCounter;
    Int_t fEvtTalk;
    ClassDef(PndEventCounterTask,1);
    
  };

#endif
