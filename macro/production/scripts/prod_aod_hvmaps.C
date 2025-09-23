int prod_aod_hvmaps(TString prefix="")
{
  if (prefix=="") 
  {
    cout << "USAGE:\n";
    cout << "prod_aod.C( <pref> )\n\n";
    cout << "   <pref>     : input/output file names prefix\n\n";    
    return 0;
  }
  //-----User Settings:------------------------------------------------------
  //TString  parAsciiFile   = "all_hvmaps.par";
  TString  parAsciiFile   = "all_hvmaps.par";
  TString  output         = "pid";
  TString  opt            = "";
  //TString  opt            = "barreltrack";
  //TString  opt            = "pidnoswim";
  
  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput("dummy");
  fRun->SetOutput(output);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);
  if (opt!="") fRun->SetOptions(opt);

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddDigiTasks(/*kFALSE*/);
  fRun->AddRecoTasks(/*kFALSE*/);
  fRun->AddPidTasks();

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0);
  fRun->Finish();
  return 0;
}
