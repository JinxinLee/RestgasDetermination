// Macro for running Panda digitization tasks
// to run the macro:
// root  digi_complete.C  or in root session root>.x  digi_complete.C
int digi_complete(Int_t nEvents = 0, TString prefix="../data/evtcomplete")
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  output         = "digi";
  TString  friend1        = "";
  TString  friend2        = "";
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

  PndPersistencyTask *task;

  // -----   Add tasks   ----------------------------------------------------

  task = new PndMvdDigiTask();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndMvdClusterTask();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndSttHitProducerRealFast();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndGemDigitize("GEM Digitizer", 0);
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndGemFindHits("GEM Hit Finder", 0);
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndDrcHitProducerReal();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndSciTDigiTask();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndEmcHitsToWaveform();
  task->SetPersistency(kFALSE);
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndEmcWaveformToDigi();
  task->SetPersistency(kTRUE);
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndEmcMakeCluster();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndEmcMakeBump();
  task->SetVerbose(0);
  fRun->AddTask(task);

  PndMdtHitProducerIdeal* mdt = new PndMdtHitProducerIdeal();
  mdt->SetPositionSmearing(.3);
  mdt->SetVerbose(0);
  fRun->AddTask(mdt);

  task = new PndMdtTrkProducer();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndFtsHitProducerRealFast();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndFtofHitProducerIdeal();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndRichHitProducer();
  task->SetVerbose(0);
  fRun->AddTask(task);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  fRun->Finish();
  return 0;
}
