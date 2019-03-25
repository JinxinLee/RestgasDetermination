// Macro for running Panda mvd analysis
// to run the macro:
// root  ana_mvd.C"(\"WORKDIR\")"
int ana_mvd(TString workDir=".", Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  prefix         = "mvdqa";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  output         = "qaana";
  TString  friend1        = "sim";
  TString  friend2        = "rec";
  TString  friend3        = "";
  TString  friend4        = "";
  TString  fOptions       = "";

  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput(input);
  fRun->SetOutput(output);
  fRun->AddFriend(friend1);
  fRun->AddFriend(friend2);
  fRun->AddFriend(friend3);
  fRun->AddFriend(friend4);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);

  PndMvdQATask* qaTask = new PndMvdQATask();
  qaTask->SetVerbose(0);
  qaTask->SetWorkdir(workDir);
  fRun->AddTask(qaTask);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
