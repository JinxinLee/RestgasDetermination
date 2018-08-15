int run_digi()
{
  // Macro created 20/09/2006 by S.Spataro
  // It loads a simulation file and digitize hits 

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  Bool_t timeBased = false;
  // Input file (MC events)
  TString inFile = "dpm_simulation.root";
  Int_t nEvents = 0;
  
  PndFileNameCreator creator(inFile.Data());
  TString parFile = creator.GetParFileName().c_str();
  TString outFile;
  if (timeBased)
	  outFile= creator.GetDigiFileName("timebased").c_str();
  else
	  outFile= creator.GetDigiFileName().c_str();
  std::cout << "DigiFileName: " << outFile.Data() << std::endl;
  // Digitisation file (ascii)
  TString digiFile = "all.par";

  FairLogger *logger = FairLogger::GetLogger();
  logger->SetLogFileName("MyLog.log");
  logger->SetLogToScreen(kTRUE);
//  logger->SetLogToFile(kTRUE);
  logger->SetLogVerbosityLevel("HIGH");
//  logger->SetLogFileLevel("DEBUG4");
  logger->SetLogScreenLevel("DEBUG");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  FairFileSource *fFileSource = new FairFileSource(inFile);
//  fFileSource->ReadEventTimeFromFile("EventTimes.dat");
  fFileSource->SetEventMeanTime(50);
  fFileSource->SetBeamTime(1600, 400);
  fRun->SetSource(fFileSource);

  fRun->SetOutputFile(outFile);
  fRun->SetGenerateRunInfo(kFALSE);
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
  if (timeBased)
	  sttHitProducer->RunTimeBased();
  fRun->AddTask(sttHitProducer);
  
  if (timeBased) {
	  PndSttHitSorterTask* sttSorter = new PndSttHitSorterTask(5000, 50, "STTHit", "STTSortedHits", "PndSTT");
  	  fRun->AddTask(sttSorter);
  }

  // -----   MVD digi producers   ---------------------------------
  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  if (timeBased)
	  mvddigi->RunTimeBased();
  mvddigi->SetVerbose(iVerbose);
  fRun->AddTask(mvddigi);

  // -----   EMC hit producers   ---------------------------------
  //wf simulation
  if (timeBased){
    Bool_t storeWfDataClass = kTRUE;
    Bool_t storeWf = kTRUE;
    PndEmcFWEndcapTimebasedWaveforms* wfTask = new PndEmcFWEndcapTimebasedWaveforms(iVerbose);
    wfTask->StoreDataClass(storeWfDataClass);
    wfTask->SetStorageOfData(storeWf);
    if (timeBased)
    	wfTask->RunTimebased();

    Bool_t storeUnsortedDigis = kTRUE;
    PndEmcFWEndcapDigi* digiTask = new PndEmcFWEndcapDigi(iVerbose, storeUnsortedDigis);

    PndEmcDigiSorterTask* sorterTask = new PndEmcDigiSorterTask(10000, 1, "EmcDigi", "EmcDigiSorted", "Emc");
    sorterTask->SetVerbose(iVerbose);


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

  //PndEmcHdrFiller* emcHdrFiller = new PndEmcHdrFiller();
  //fRun->AddTask(emcHdrFiller); // ECM header

  // -----   SciT hit producers   ---------------------------
  Double_t SciTDeadtime = 1000;
  Double_t SciTdt = 0.1;

  PndSciTDigiTask* SciTDigi = new PndSciTDigiTask();
  SciTDigi->SetVerbose(iVerbose);
  SciTDigi->SetDeadTime(SciTDeadtime);
  SciTDigi->SetTimeResolution(SciTdt);
  if (timeBased)
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
  if (timeBased)
	  ftsHitProducer->RunTimeBased();
  fRun->AddTask(ftsHitProducer);

  if (timeBased){
	  PndFtsHitSorterTask* ftsSorter = new PndFtsHitSorterTask(5000, 50, "FTSHit", "FTSSortedHits", "PndFTS");
	  fRun->AddTask(ftsSorter);
  }

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

  return 0;
}
