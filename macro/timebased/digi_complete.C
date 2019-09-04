// Macro for running Panda digitization tasks
// to run the macro:
// root  digi_complete.C  or in root session root>.x  digi_complete.C
int digi_complete(Int_t nEvents = 0, TString prefix="../data/evtcomplete", Bool_t timeBased = kTRUE)
{
  //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";
  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  output         = "digi_timebased";
  TString  friend1        = "sim";
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
  FairFileSource* source = (FairFileSource*)FairRootManager::Instance()->GetSource();
  source->SetEventMeanTime(500);
  source->SetBeamTime(1600,400);

  PndPersistencyTask *task;

  // -----   Add tasks   ----------------------------------------------------
  if (timeBased) {
     std::cout << "Running TIMEBASED" << std::endl;
  } else {
     std::cout << "Running EVENTBASED" << std::endl;
  }

  task = new PndMvdDigiTask();
  task->SetVerbose(0);
  if (timeBased)
      ((PndMvdDigiTask*)task)->RunTimeBased();
  fRun->AddTask(task);

  task = new PndSttHitProducerRealFull();
  if (timeBased)
      ((PndSttHitProducerRealFull*)task)->RunTimeBased();
  task->SetVerbose(0);
  fRun->AddTask(task);

  if (timeBased)
  {
    PndSttHitSorterTask* stttask = new PndSttHitSorterTask(5000, 50, "STTHit", "STTSortedHits", "PndSTT");
    fRun->AddTask(stttask);
  }

  task = new PndGemDigitize("GEM Digitizer", 0);
  if (timeBased)
        ((PndGemDigitize*)task)->RunTimeBased();
  task->SetVerbose(0);
  fRun->AddTask(task);

  if (timeBased)
  {
    PndGemDigiSorterTask* gemtask = new PndGemDigiSorterTask(5000, 50, "GEMDigi", "GEMSortedDigi", "PndGEM");
    fRun->AddTask(gemtask);
  }


  task = new PndDrcHitProducerReal();
  task->SetVerbose(0);
  fRun->AddTask(task);

  Double_t SciTDeadtime = 1000;
  Double_t SciTdt = 0.1;

  PndSciTDigiTask* SciTDigi = new PndSciTDigiTask();
  SciTDigi->SetVerbose(0);
  SciTDigi->SetDeadTime(SciTDeadtime);
  SciTDigi->SetTimeResolution(SciTdt);
  if (timeBased)
     SciTDigi->RunTimeBased();
  fRun->AddTask(SciTDigi);

  if (timeBased){
      PndSciTHitSorterTask* task = new PndSciTHitSorterTask(5000, 1, "SciTHit", "SciTSortedHit", "PndSciT");
      fRun->AddTask(task);
  }

  if (timeBased){
    Bool_t storeWfDataClass = kTRUE;
    Bool_t storeWf = kTRUE;
    PndEmcFWEndcapTimebasedWaveforms* wfTask = new PndEmcFWEndcapTimebasedWaveforms(0);
    wfTask->StoreDataClass(storeWfDataClass);
    wfTask->SetStorageOfData(storeWf);
    if (timeBased)
        wfTask->RunTimebased();

    Bool_t storeUnsortedDigis = kTRUE;
    PndEmcFWEndcapDigi* digiTask = new PndEmcFWEndcapDigi(0, storeUnsortedDigis);

    PndEmcDigiSorterTask* sorterTask = new PndEmcDigiSorterTask(10000, 1, "EmcDigi", "EmcDigiSorted", "Emc");
    sorterTask->SetVerbose(0);


    fRun->AddTask(wfTask);
    fRun->AddTask(digiTask);
    fRun->AddTask(sorterTask);
  }
  else {
      PndEmcHitsToWaveform* emcwf = new PndEmcHitsToWaveform();
      PndEmcWaveformToDigi* emcdigi = new PndEmcWaveformToDigi();

      fRun->AddTask(emcwf);
      fRun->AddTask(emcdigi);

  }

  PndMdtHitProducerIdeal* mdt = new PndMdtHitProducerIdeal();
  mdt->SetPositionSmearing(.3);
  mdt->SetVerbose(0);
  fRun->AddTask(mdt);

  task = new PndMdtTrkProducer();
  task->SetVerbose(0);
  fRun->AddTask(task);

  task = new PndFtsHitProducerRealFull();
  if (timeBased){
      ((PndFtsHitProducerRealFull*)task)->RunTimeBased();
  }
  task->SetVerbose(0);
  fRun->AddTask(task);

  if(timeBased){
      PndFtsHitSorterTask* ftstask = new PndFtsHitSorterTask(5000,50,"FTSHit", "FTSSortedHit","PndFTS");
      fRun->AddTask(ftstask);
  }

  task = new PndFtofHitProducerIdeal();
//  if (timeBased){
//      (PndFtofHitProducerIdeal*)task->RunTimebased();
//  }
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
