// Macro for running Panda digitization tasks
// to run the macro:
// root  digi_complete.C  or in root session root>.x  digi_complete.C
int eventBuilderAna(Int_t nEvents = 0, TString prefix="evtcomplete", Bool_t timeBased = kTRUE)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  output         = "eventana_500";
  TString  friend1        = "event_500";
  TString  friend2        = "sim";
  TString  friend3        = "";
  TString  friend4        = "";

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


  // -----   Add tasks   ----------------------------------------------------

  PndEventBuilderAnaTask* task = new PndEventBuilderAnaTask();
  task->SetBranchName("MVDSortedPixelDigis_event");
  task->AddAdditionalBranches("MVDSortedStripDigis_event");
  task->AddAdditionalBranches("STTSortedHits_event");
//  task->AddAdditionalBranches("GEMSortedDigi_event");
  task->AddAdditionalBranches("SciTSortedHit_event");
//  task->AddAdditionalBranches("FtsSortedHit_event");
//  task->AddAdditionalBranches("EmcDigiSorted_event");
  task->SetVerbose(2);
  fRun->AddTask(task);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
