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

PndMvdClusterTask::PndMvdClusterTask(Double_t pixelRadius, Int_t stripChargecut, TString geoFile) :
  FairTask("MVD Clustertization Task")
{
	Int_t meanalgo=0; //0: built-in CoG, 1: algo class, CoG
	Int_t clustermod=0; //0: simple, 1:"normal"
  this->Add(new PndMvdPixelClusterTask(pixelRadius,geoFile));
  this->Add(new PndMvdStripClusterTask(stripChargecut,geoFile,meanalgo,clustermod));

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

