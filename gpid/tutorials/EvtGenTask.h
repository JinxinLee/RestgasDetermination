#ifndef EVTGENTASK_H
#define EVTFENTASK_H 1


#include "CbmTask.h"
#include "TVector3.h"
#include <algorithm>
#include <vector>
#include "Event.h" 
#include "TRandom3.h"

class TClonesArray;
class TTree;
class TFile;


class EvtGenTask : public CbmTask
{
 public :
    EvtGenTask();
    virtual ~EvtGenTask();
    virtual InitStatus Init();
    virtual void Exec(Option_t* opt) ; 	

 private :
    TClonesArray* fEvtArray;  
    Event* fEvt; 
    TRandom3 fRan;
    ClassDef(EvtGenTask,1);
};

#endif 
