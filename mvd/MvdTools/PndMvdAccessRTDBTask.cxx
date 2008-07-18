#include "PndMvdAccessRTDBTask.h"

#include "CbmRuntimeDb.h"
#include "CbmRunAna.h"

ClassImp(PndMvdAccessRTDBTask);

void PndMvdAccessRTDBTask::SetParContainers()
{
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();
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
