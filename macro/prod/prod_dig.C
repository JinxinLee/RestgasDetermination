void prod_dig(TString prefix="")
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  output         = "digi";
  
  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput("dummy");
  fRun->SetOutput(output);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddDigiTasks();

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();

}
