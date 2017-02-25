int prod_pid(TString prefix="")
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  output         = "pid";
  TString  friend1        = "digi";
  TString  friend2        = "reco";
  
  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput("dummy");
  fRun->SetOutput(output);
  fRun->SetFriend1(friend1);
  fRun->SetFriend2(friend2);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);
  
  // -----   Add tasks   ----------------------------------------------------
  fRun->AddPidTasks();
  
  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();

  return 0;
}
