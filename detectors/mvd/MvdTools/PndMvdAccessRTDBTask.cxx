#include "PndMvdAccessRTDBTask.h"

#include "FairRuntimeDb.h"
#include "FairRun.h"

ClassImp(PndMvdAccessRTDBTask);

void PndMvdAccessRTDBTask::SetParContainers()
{
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fPixelDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
  fStripDigiParRect = (PndSdsStripDigiPar*)(rtdb->getContainer("MVDStripDigiParRect"));
  fStripDigiParTrap = (PndSdsStripDigiPar*)(rtdb->getContainer("MVDStripDigiParTrap"));
}

InitStatus PndMvdAccessRTDBTask::Init()
{
  return kSUCCESS;
}

InitStatus PndMvdAccessRTDBTask::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

void
PndMvdAccessRTDBTask::Exec(Option_t*)
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
