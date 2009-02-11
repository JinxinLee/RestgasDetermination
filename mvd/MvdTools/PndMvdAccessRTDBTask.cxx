#include "PndMvdAccessRTDBTask.h"

#include "FairRuntimeDb.h"
#include "FairRun.h"

ClassImp(PndMvdAccessRTDBTask);

void PndMvdAccessRTDBTask::SetParContainers()
{
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fPixelDigiPar = (PndMvdPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
  fStripDigiParRect = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParRect"));
  fStripDigiParTrap = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParTrap"));
}

InitStatus PndMvdAccessRTDBTask::Init()
{

}

InitStatus PndMvdAccessRTDBTask::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

void
PndMvdAccessRTDBTask::Exec(Option_t* opt)
{
  return;
}

void
PndMvdAccessRTDBTask::Finish()
{
  fPixelDigiPar->Print();
  fStripDigiParRect->Print();
  fStripDigiParTrap->Print();
  return;
}
