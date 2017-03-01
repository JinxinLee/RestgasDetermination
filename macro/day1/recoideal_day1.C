// Macro for running Panda reconstruction tasks
// to run the macro:
// root  recoideal_day1.C  or in root session root>.x  reco_complete.C
// It uses an ideal pattern recognition both in the barrel and the forward part with a smearing of momentum and position vectors.
// After the ideal pattern reco the genfit kalman filter runs over the data set.

int recoideal_day1(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  options        = "day1+gem+fts1256";
  TString  input          = "evtday1"; 
  TString  output         = "recoideal";
  TString  friend1        = "digi";
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
  fRun->Setup();
  
  // -----   Add tasks   ----------------------------------------------------
  fRun->AddRecoIdealTasks();
  
  // -----   Intialise and run   --------------------------------------------
  PndEmcMapper::Init(1);
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();

  return 0;
}
