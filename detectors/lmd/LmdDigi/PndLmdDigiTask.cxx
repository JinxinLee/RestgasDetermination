// -------------------------------------------------------------------------
// -----                    PndLmdDigiTask source file                 -----
// -----                  Created 04/02/10  by M. Michel               -----
// -----                Based on PndMvdDigiTask by R. Kliemt           -----
// -----                  Wrapper for Lmd digitizing tasks             -----
// -------------------------------------------------------------------------


#include "PndLmdDigiTask.h"
#include "PndLmdStripHitProducer.h"
//#include "PndLmdNoiseProducer.h"

// -----   Default constructor   -------------------------------------------
PndLmdDigiTask::PndLmdDigiTask() :
  FairTask("LMD Digitization Task")
{
  this->Add(new PndLmdStripHitProducer());
//  this->Add(new PndLmdNoiseProducer());

  TList* thistasks = this->GetListOfTasks();
  for(Int_t i=0;i<thistasks->GetEntries();i++)
  {
    ((FairTask*)thistasks->At(i))->SetVerbose(fVerbose);
  }
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndLmdDigiTask::~PndLmdDigiTask()
{
}
// -------------------------------------------------------------------------
/*
// -----   Initialization  of Parameter Containers -------------------------
void PndLmdDigiTask::SetParContainers()
{
}

InitStatus PndLmdDigiTask::ReInit()
{
  return kERROR;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndLmdDigiTask::Init()
{
  return kSUCCESS;
}

// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndLmdDigiTask::Exec(Option_t*)
{
  return;
}

*/
ClassImp(PndLmdDigiTask);
