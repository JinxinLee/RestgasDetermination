int prod_aod(TString prefix="")
{
  if (prefix=="") 
  {
    cout << "USAGE:\n";
    cout << "prod_aod.C( <pref> )\n\n";
    cout << "   <pref>     : input/output file names prefix\n\n";    
    return;
  }
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  output         = "pid";
  TString  opt            = "";
  
  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput("dummy");
  fRun->SetOutput(output);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);
  if (opt!="") fRun->SetOptions(opt);

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddDigiTasks(kFALSE);
  fRun->AddRecoTasks(kFALSE);
  fRun->AddPidTasks();

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0);
  fRun->Finish();
  return 0;
}
