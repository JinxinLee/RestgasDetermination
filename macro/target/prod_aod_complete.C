int prod_aod_complete(TString prefix="")
{
  if (prefix=="") 
  {
    cout << "USAGE:\n";
    cout << "prod_aod.C( <pref> )\n\n";
    cout << "   <pref>     : input/output file names prefix\n\n";    
    return 0;
  }
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  //TString  parAsciiFile   = "all_hvmaps.par";
  TString  output         = "pid_poca";
  TString  opt            = "genfit2";
  TString friend1 = "sim";
  TString friend2 = "digi";
  //TString  opt            = "barreltrack";
  
  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput("dummy");
  fRun->SetOutput(output);
  fRun->AddFriend(friend1);
  fRun->AddFriend(friend2);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);
  if (opt!="") fRun->SetOptions(opt);

  // -----   Add tasks   ----------------------------------------------------
  //fRun->AddDigiTasks(/*kFALSE*/);
  fRun->AddRecoTasks(/*kFALSE*/);
  fRun->AddPidTasks();

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0);
  fRun->Finish();
  return 0;
}
