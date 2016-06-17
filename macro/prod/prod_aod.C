void prod_aod(TString prefix="")
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
  fRun->Run(0);
  fRun->Finish();
}
