int prod_aod_hvmaps(TString prefix="", Bool_t use_mvd_hvmaps = false)
{
  if (prefix=="") 
  {
    cout << "USAGE:\n";
    cout << "prod_aod_hvmaps.C( <pref>, <use_mvd_hvmaps> )\n\n";
    cout << "   <pref>           : input/output file names prefix\n";
    cout << "   <use_mvd_hvmaps> : boolean (true/false) to select all_hvmaps.par\n\n";
    return 0;
  }
  //-----User Settings:------------------------------------------------------
  TString parAsciiFile = use_mvd_hvmaps ? "all_hvmaps.par" : "all.par";
  TString  output         = "digi";
  TString  opt            = "genfit2";
  //TString  opt            = "barreltrack";
  
  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput("dummy");
  fRun->SetOutput(output);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);
  if (opt!="") fRun->SetOptions(opt);

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddDigiTasks(/*kFALSE*/);
//  fRun->AddRecoTasks(/*kFALSE*/);
//  fRun->AddPidTasks();

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0);
  fRun->Finish();
  return 0;
}
