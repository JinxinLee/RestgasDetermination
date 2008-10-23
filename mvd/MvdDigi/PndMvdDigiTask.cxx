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
  CbmTask("MVD Digitization Task")
{
  this->Add(new PndMvdHybridHitProducer());
  this->Add(new PndMvdStripHitProducer());
  this->Add(new PndMvdNoiseProducer());

  TList* thistasks = this->GetListOfTasks();
  for(Int_t i=0;i<thistasks->GetEntries();i++)
  {
    ((CbmTask*)thistasks->At(i))->SetVerbose(fVerbose);
  }
}
// -------------------------------------------------------------------------

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
