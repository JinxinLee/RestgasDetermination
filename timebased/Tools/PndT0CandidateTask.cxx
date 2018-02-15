#include "PndT0CandidateTask.h"

#include "PndSolCorrTask.h"
#include "PndT0CandidateDetermination.h"


PndT0CandidateTask::PndT0CandidateTask() :
  FairTask("PndOnlineT0")
{
  fPersistence = kTRUE;
  fRunContinuous = kTRUE;
  
  PndSolCorrTask* SolCorrTask = new PndSolCorrTask();
  SolCorrTask->SetOutBranchName("TofCounter_SolCorr");
  SolCorrTask->AddInputBranch("SciTHit");
  SolCorrTask->AddInputBranch("FtofHit");
  this->Add(SolCorrTask);
  this->Add(new PndT0CandidateDetermination("TofCounter_SolCorr"));
  
  SetPersistence(fPersistence);
  RunContinuous(fRunContinuous);
  
}

PndT0CandidateTask::~PndT0CandidateTask()
{
}

void PndT0CandidateTask::SetPersistence(Bool_t val){

  fPersistence = val;
  TList* thistasks = this->GetListOfTasks();
  ((PndSolCorrTask*)thistasks->At(0))->SetPersistence(fPersistence);
  ((PndT0CandidateDetermination*)thistasks->At(1))->SetPersistence(fPersistence);
  return;

}

void PndT0CandidateTask::RunContinuous(Bool_t val)
{

  fRunContinuous = val;
  TList* thistasks = this->GetListOfTasks();
  ((PndSolCorrTask*)thistasks->At(0))->RunContinuous(fRunContinuous);
  ((PndT0CandidateDetermination*)thistasks->At(1))->RunContinuous(fRunContinuous);
}



// -----   Public method Init   --------------------------------------------

InitStatus PndT0CandidateTask::Init()
{

  return kSUCCESS;
}


ClassImp(PndT0CandidateTask);

