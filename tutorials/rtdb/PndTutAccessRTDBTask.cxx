#include "PndTutAccessRTDBTask.h"

#include "FairRuntimeDb.h"
#include "FairRunAna.h"

ClassImp(PndTutAccessRTDBTask);

void PndTutAccessRTDBTask::SetParContainers()
{
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fTutParDef = (PndTutPar*)(rtdb->getContainer("TUTParDefault"));
  fTutParAlt = (PndTutPar*)(rtdb->getContainer("TUTParAlternative"));
}

InitStatus PndTutAccessRTDBTask::Init()
{
  if (!fTutParDef){
     std::cout<<"-E- PndTutAccessRTDBTask: fTutParDef Container does not exist!"<<std::endl;
     return kERROR;
  }
  if (!fTutParAlt){
     std::cout<<"-E- PndTutAccessRTDBTask: fTutParAlt Container does not exist!"<<std::endl;
     return kERROR;
  }
    return kSUCCESS;

}

InitStatus PndTutAccessRTDBTask::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

void
PndTutAccessRTDBTask::Exec(Option_t*)
{
  return;
}

void
PndTutAccessRTDBTask::Finish()
{
  fTutParDef->Print();
  fTutParAlt->Print();
  return;
}
