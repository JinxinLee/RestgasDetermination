// Macro for running Panda digitization tasks
// to run the macro:
// root  digi_complete.C  or in root session root>.x  digi_complete.C
int digiOnly_complete(Int_t nEvents = 0)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  prefix         = "evtcomplete_1G5";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  output         = "digi";
  TString  friend1        = "digionly";
  TString  friend2        = "";
  TString  friend3        = "";
  TString  friend4        = "";

  // -----   Initial Settings   --------------------------------------------
  PndMasterRunAna *fRun= new PndMasterRunAna();
  fRun->SetInput(input);
  fRun->SetOutput(output);
  fRun->SetFriend1(friend1);
  fRun->SetFriend2(friend2);
  fRun->SetFriend3(friend3);
  fRun->SetFriend4(friend4);
  fRun->SetParamAsciiFile(parAsciiFile);
  fRun->Setup(prefix);

  PndPersistencyTask *task;

  // -----   Add tasks   ----------------------------------------------------
  task = new PndSttHitProducerRealFast();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndMvdDigiTask();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndEmcHitsToWaveform();
  task->SetPersistency(kFALSE);
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndEmcWaveformToDigi();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndSciTDigiTask();
  task->SetVerbose(0);
  fRun->AddTask(task);

  PndMdtHitProducerIdeal* mdt = new PndMdtHitProducerIdeal();
  mdt->SetPositionSmearing(.3);
  mdt->SetVerbose(0);
  fRun->AddTask(mdt);

  task = new PndDrcHitProducerReal();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndGemDigitize("GEM Digitizer", 0);
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
