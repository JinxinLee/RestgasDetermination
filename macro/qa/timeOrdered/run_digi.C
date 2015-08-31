void run_digi()
{
  // Macro created 20/09/2006 by S.Spataro
  // It loads a simulation file and digitize hits 

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
  // Input file (MC events)
  TString inFile = "sim_complete.root";
  Int_t nEvents = 0;
  
  PndFileNameCreator creator(inFile.Data());
  TString parFile = creator.GetParFileName().c_str();
  TString outFile = creator.GetDigiFileName("timebased").c_str();
  std::cout << "DigiFileName: " << outFile.Data() << std::endl;
  // Digitisation file (ascii)
  TString digiFile = "all.par";
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->SetGenerateRunInfo(kFALSE);
  fRun->SetEventMeanTime(50);
  //fRun->SetBeamTime(400, 2000);
  fRun->SetUseFairLinks(kTRUE); 
  // -----  Parameter database   --------------------------------------------
  TString allDigiFile = gSystem->Getenv("VMCWORKDIR");
  allDigiFile += "/macro/params/";
  allDigiFile += digiFile;
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  
  // -----   STT digi producers   ---------------------------------
  PndSttHitProducerRealFull* sttHitProducer = new PndSttHitProducerRealFull();
  sttHitProducer->RunTimeBased();
  fRun->AddTask(sttHitProducer);
  
  PndSttHitSorterTask* sttSorter = new PndSttHitSorterTask(5000, 50, "STTHit", "STTSortedHits", "PndSTT");
  fRun->AddTask(sttSorter);

  // -----   MDV digi producers   ---------------------------------
  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  mvddigi->RunTimeBased();
  mvddigi->SetVerbose(iVerbose);
  fRun->AddTask(mvddigi);

  // -----   EMC hit producers   ---------------------------------
  //wf simulation
    Bool_t storeWfDataClass = kTRUE;
    Bool_t storeWf = kTRUE;
    PndEmcFWEndcapTimebasedWaveforms* wfTask = new PndEmcFWEndcapTimebasedWaveforms(iVerbose);
    wfTask->StoreDataClass(storeWfDataClass);
    wfTask->SetStorageOfData(storeWf);
    wfTask->RunTimebased();

    Bool_t storeUnsortedDigis = kTRUE;
    PndEmcFWEndcapDigi* digiTask = new PndEmcFWEndcapDigi(iVerbose, storeUnsortedDigis);

    PndEmcDigiSorterTask* sorterTask = new PndEmcDigiSorterTask(10000, 1, "EmcDigi", "EmcDigiSorted", "Emc");
    sorterTask->SetVerbose(iVerbose);

    fRun->AddTask(wfTask);
    fRun->AddTask(digiTask);
    fRun->AddTask(sorterTask);

  //PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  //fRun->AddTask(emcHdrFiller); // ECM header

  // -----   SciT hit producers   ---------------------------
  Double_t SciTDeadtime = 1000;
  Double_t SciTdt = 0.1;

  PndSciTDigiTask* SciTDigi = new PndSciTDigiTask();
  SciTDigi->SetVerbose(iVerbose);
  SciTDigi->SetDeadTime(SciTDeadtime);
  SciTDigi->SetTimeResolution(SciTdt);
  SciTDigi->RunTimeBased();
  fRun->AddTask(SciTDigi);

  // -----   MDT hit producers   ---------------------------------
  PndMdtHitProducerIdeal* mdtHitProd = new PndMdtHitProducerIdeal();
  mdtHitProd->SetPositionSmearing(.3); // position smearing [cm]
  fRun->AddTask(mdtHitProd);

  // -----   DRC hit producers   ---------------------------------
  PndDrcHitProducerReal* drchit = new PndDrcHitProducerReal();
  drchit->SetVerbose(iVerbose);
  fRun->AddTask(drchit);

  // -----   GEM hit producers   ---------------------------------
  Int_t verboseLevel = 0;
  PndGemDigitize* gemDigitize = new PndGemDigitize("GEM Digitizer", verboseLevel);
  fRun->AddTask(gemDigitize);


  // -----   FTS hit producers   ---------------------------------
  PndFtsHitProducerRealFull* ftsHitProducer = new PndFtsHitProducerRealFull();
  ftsHitProducer->RunTimeBased();
  // fRun->AddTask(ftsHitProducer);

  PndFtsHitSorterTask* ftsSorter = new PndFtsHitSorterTask(5000, 50, "FTSHit", "FTSSortedHits", "PndFTS");
  //  fRun->AddTask(ftsSorter);

  // -----   Ftof hit producers   ---------------------------
  PndFtofHitProducerIdeal* ftofhit = new PndFtofHitProducerIdeal();
  ftofhit->SetVerbose(iVerbose);
  fRun->AddTask(ftofhit);

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  timer.Start();
  fRun->Run(0, nEvents);

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
  cout << " Test passed" << endl;
  cout << " All ok " << endl;

  exit(0);
}
