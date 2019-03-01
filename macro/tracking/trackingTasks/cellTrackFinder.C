// Macro for running Panda digitization tasks
// to run the macro:
// root  digi_complete.C  or in root session root>.x  digi_complete.C
int cellTrackFinder(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  prefix         = "evtcomplete";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  output         = "cell";
  TString  friend1        = "sim";
  TString  friend2        = "digi";
  TString  friend3        = "";
  TString  friend4        = "";
  TString  fOptions       = ""; // "gf2" for genfit 2

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

//  FairGeane *Geane = new FairGeane();
//    fRun->AddTask(Geane);

    PndSttCellTrackFinderTask *cellTrackFinder = new PndSttCellTrackFinderTask();
    cellTrackFinder->SetPersistence(kTRUE);
    cellTrackFinder->AddHitBranch("STTHit");
    cellTrackFinder->SetAnalyseSteps(kFALSE);
    cellTrackFinder->SetVerbose(0);
    cellTrackFinder->SetRunTimeBased(kFALSE);
//      cellTrackFinder->SetClusterTime(300);
      fRun->AddTask(cellTrackFinder);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
