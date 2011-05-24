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
  PndBlackBoxTask("MVD Digitization BlacBox Task")
{
  this->Add(new PndMvdHybridHitProducer());
  this->Add(new PndMvdStripHitProducer());
  //this->Add(new PndMvdNoiseProducer());
   
  fPersistance = kTRUE;
  SetPersistance(fPersistance);
}
// -------------------------------------------------------------------------

void PndMvdDigiTask::SetPersistance(Bool_t pers)
{
  fPersistance = pers;
  TList* thistasks = this->GetListOfTasks();
  ((PndMvdHybridHitProducer*)thistasks->At(0))->SetPersistance(fPersistance);
  ((PndMvdStripHitProducer*)thistasks->At(1))->SetPersistance(fPersistance);
  //((PndMvdNoiseProducer*)thistasks->At(2))->SetPersistance(fPersistance);
  return;
}

// -----   Destructor   ----------------------------------------------------
PndMvdDigiTask::~PndMvdDigiTask()
{
}
// -------------------------------------------------------------------------


ClassImp(PndMvdDigiTask);
