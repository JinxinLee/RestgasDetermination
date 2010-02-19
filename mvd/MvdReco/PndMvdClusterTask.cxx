// -------------------------------------------------------------------------
// -----                     PndMvdClusterTask source file             -----
// -----                  Created 07/10/08  by R. Kliemt               -----
// -------------------------------------------------------------------------


#include "PndMvdClusterTask.h"
#include "PndMvdPixelClusterTask.h"
#include "PndMvdStripClusterTask.h"

// -----   Default constructor   -------------------------------------------
// PndMvdClusterTask::PndMvdClusterTask() :
//   FairTask("MVD Clustertisation Task")
// {
//
// }
// -------------------------------------------------------------------------

PndMvdClusterTask::PndMvdClusterTask() :
FairTask("MVD Clustertization Task")
{
  this->Add(new PndMvdPixelClusterTask());
  this->Add(new PndMvdStripClusterTask());
  TList* thistasks = this->GetListOfTasks();
  for(Int_t i=0;i<thistasks->GetEntries();i++)
  {
    ((FairTask*)thistasks->At(i))->SetVerbose(fVerbose);
  }
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMvdClusterTask::~PndMvdClusterTask()
{
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdClusterTask::SetParContainers()
{
}

InitStatus PndMvdClusterTask::ReInit()
{
  return kERROR;
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


// -----   Public method Init   --------------------------------------------
InitStatus PndMvdClusterTask::Init()
{
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMvdClusterTask::Exec(Option_t* opt)
{
  return;
}

ClassImp(PndMvdClusterTask);

