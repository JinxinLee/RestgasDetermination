// Macro for running Panda digitization, reconstruction and pid tasks
// to run the macro:
// root  full_day1.C  or in root session root>.x  full_complete.C
void full_day1(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  options        = "day1+gem+fts1256";
  TString  prefix         = "evtday1";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec"; 
  TString  output         = "pid";
  TString  friend1        = "";
  TString  friend2        = "";
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
  fRun->Setup(prefix);

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddDigiTasks();
  fRun->AddRecoTasks();
  //fRun->AddPidTasks();

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
}
