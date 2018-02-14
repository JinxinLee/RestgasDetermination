// Macro for running Panda digitization tasks
// to run the macro:
// root  digi_day1.C  or in root session root>.x  digi_complete.C
int digi_day1(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString options        = "day1+gem2+strip"; //with fts1234
  //TString options        = "day1+gem2+strip+fts1256";
  TString  input          = "evtday1";
  TString  output         = "digi";
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
  fRun->Setup();

  // -----   Add tasks   ----------------------------------------------------
  fRun->AddDigiTasks();

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();

  if (gROOT->GetVersionInt() >= 60602) {
    gGeoManager->GetListOfVolumes()->Delete();
    gGeoManager->GetListOfShapes()->Delete();
    delete gGeoManager;
  }
  return 0;
}
