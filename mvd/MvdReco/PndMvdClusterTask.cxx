// -------------------------------------------------------------------------
// -----                     PndMvdClusterTask source file             -----
// -----                  Created 07/10/08  by R. Kliemt               -----
// -------------------------------------------------------------------------


#include "PndMvdClusterTask.h"
#include "PndMvdPixelClusterTask.h"
#include "PndMvdStripClusterTask.h"


PndMvdClusterTask::PndMvdClusterTask() :
PndBlackBoxTask("Mvd Hit Reconstruction BlackBox Task")
{
  fPersistance = kTRUE;
  this->Add(new PndMvdPixelClusterTask());
  this->Add(new PndMvdStripClusterTask());
  SetPersistance(fPersistance);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMvdClusterTask::~PndMvdClusterTask()
{
}
// -------------------------------------------------------------------------

void PndMvdClusterTask::SetPersistance(Bool_t pers)
{
  //TODO: This is not recursive 
  fPersistance = pers;
  TList* thistasks = this->GetListOfTasks();
  ((PndMvdPixelClusterTask*)thistasks->At(0))->SetPersistance(fPersistance);
  ((PndMvdStripClusterTask*)thistasks->At(1))->SetPersistance(fPersistance);
  return;
}

ClassImp(PndMvdClusterTask);

