// Macro for running Panda pid tasks
// to run the macro:
// root  pid_day1.C  or in root session root>.x  pid_complete.C
void pid_day1(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  options        = "day1+gem+fts1256";
  TString  input          = "evtday1"; 
  TString  output         = "pid";
  TString  friend1        = "digi";
  TString  friend2        = "reco";
  TString  friend3        = "";
  TString  friend4        = "";
  
  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetOptions(options);
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

  exit(0); 
}
