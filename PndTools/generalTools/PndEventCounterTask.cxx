// -------------------------------------------------------------------------
// -----                   PndEventCounterTask source file             -----
// -----                  Created 24/09/10  by R. Kliemt               -----
// -------------------------------------------------------------------------

#include "PndEventCounterTask.h"
#include <iostream>

PndEventCounterTask::PndEventCounterTask(const char* name) :
FairTask(name)
{
  fEvtCounter=0;
  fEvtTalk=100;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndEventCounterTask::~PndEventCounterTask()
{
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndEventCounterTask::Exec(Option_t* opt)
{
  fEvtCounter++;
  if(fEvtCounter%fEvtTalk == 0 || fVerbose>1)
  {
    std::cout<<"Event number "<<fEvtCounter<<"."<<std::endl;
  }  
  return;
}

ClassImp(PndEventCounterTask);

