// Macro for running Panda digitization tasks
// to run the macro:
// root  eventBuilder.C  or in root session root>.x  eventBuilder.C
int eventBuilder(Int_t nEvents = 0, TString prefix="evtcomplete")
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  output         = "event";
  TString  friend1        = "digi_timebased";
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

  PndGapEventBuilderTask* task = new PndGapEventBuilderTask();
  task->SetBranchName("MVDSortedPixelDigis");
  task->SetTimeGap(20);
  task->AddAdditionalBranches("MVDSortedStripDigis", 10);
  task->AddAdditionalBranches("STTSortedHits", 250);
  task->AddAdditionalBranches("GEMSortedDigi", 20);
  task->AddAdditionalBranches("SciTSortedHit", 20);
  task->AddAdditionalBranches("EmcDigiSorted", 20);
//  task->AddAdditionalBranches("FTSSortedHit", 300);
  task->SetVerbose(2);
  fRun->AddTask(task);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
