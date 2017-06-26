// Macro for running Panda digitization, reconstruction and pid tasks
// to run the macro:
//
// root -l -b -q tut_aod.C
// 
// to run with different input prefix (0 = all events)
//
// root -l -b -q 'tut_aud.C(0,"mydata")'

#include "auxtut.C"

void tut_aod(Int_t nEvents = 0, TString prefix = "signal")
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  output         = "pid";
  
  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput("dummy");
  fRun->SetOutput(output);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddDigiTasks(kFALSE);
  fRun->AddRecoTasks(kFALSE);
  fRun->AddPidTasks();

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  
  // *** temporaty fix to avoid error on macro exit
  RemoveGeoManager();
}
