// -------------------------------------------------------------------------
// -----                    PndMvdDigiTask source file                 -----
// -----                  Created 07/10/08  by R. Kliemt               -----
// -----                  Wrapper for Mvd digitizing tasks             -----
// -------------------------------------------------------------------------


#include "PndMvdDigiTask.h"
#include "PndMvdHybridHitProducer.h"
#include "PndMvdStripHitProducer.h"
#include "PndMvdNoiseProducer.h"

// -----   Default constructor   -------------------------------------------
PndMvdDigiTask::PndMvdDigiTask() :
  FairTask("MVD Digitization Task")
{
  this->Add(new PndMvdHybridHitProducer());
  this->Add(new PndMvdStripHitProducer());
  this->Add(new PndMvdNoiseProducer());

  TList* thistasks = this->GetListOfTasks();
  for(Int_t i=0;i<thistasks->GetEntries();i++)
  {
    ((FairTask*)thistasks->At(i))->SetVerbose(fVerbose);
  }
  SetPersistance(kTRUE);
}
// -------------------------------------------------------------------------

void PndMvdDigiTask::SetPersistance(Bool_t pers)
{
  //TODO: This is not recursive 
  fPersistance = pers;
  TList* thistasks = this->GetListOfTasks();
  ((PndMvdHybridHitProducer*)thistasks->At(0))->SetPersistance(fPersistance);
  ((PndMvdStripHitProducer*)thistasks->At(1))->SetPersistance(fPersistance);
  ((PndMvdNoiseProducer*)thistasks->At(2))->SetPersistance(fPersistance);
  return;
}

// -----   Destructor   ----------------------------------------------------
PndMvdDigiTask::~PndMvdDigiTask()
{
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdDigiTask::SetParContainers()
{
}

InitStatus PndMvdDigiTask::ReInit()
{
  return kERROR;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdDigiTask::Init()
{
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMvdDigiTask::Exec(Option_t* opt)
{
  return;
}


ClassImp(PndMvdDigiTask);
