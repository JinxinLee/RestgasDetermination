// Macro for running Panda pid tasks
// to run the macro:
// root  pid_day1.C  or in root session root>.x  pid_complete.C
int pid_day1(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all_day1.par";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec"; 
  TString  output         = "pid";
  TString  friend1        = "digi";
  TString  friend2        = "reco";
  TString  friend3        = "";
  TString  friend4        = "";
  
  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetOptions("day1");
  fRun->SetInput(input);
  fRun->SetOutput(output);
  fRun->SetFriend1(friend1);
  fRun->SetFriend2(friend2);
  fRun->SetFriend3(friend3);
  fRun->SetFriend4(friend4);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup();
  
  // -----   Add tasks   ----------------------------------------------------
  fRun->AddPidTasks();
  
  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();

  return 0;
}
