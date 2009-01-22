#pragma once
#ifndef EVTGENTASK_H
#define EVTFENTASK_H

//C++ headers
#include <algorithm>
#include <vector>

//ROOT & PANDA headers
#include "CbmTask.h"
#include "TVector3.h"
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
