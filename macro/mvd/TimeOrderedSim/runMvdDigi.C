// Macro to emulate the MVD digitization in pandaroot
// Updated 30.11.2009
// Ralf Kliemt
runMvdDigi(Int_t nEvents=500)
{
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  TString inFile = "Mvd_Sim_Dpm_500.root";
  TString parFile = "Mvd_Sim_Dpm_500_params.root";
  TString digiparFile = gSystem->Getenv("VMCWORKDIR");
  digiparFile += "/macro/params/all.par";

  // Output file
  PndFileNameCreator creator(inFile.Data());
  TString outFile = creator.GetDigiFileName().c_str(); 
  std::cout << "DigiFileName: " << outFile.Data() << std::endl;

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  fRun->SetEventMeanTime(50);

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput2 = new FairParRootFileIo(kTRUE);
  parInput2->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput2);


  FairParAsciiFileIo* parInput1 = new FairParAsciiFileIo();
  parInput1->open(digiparFile.Data(),"in");
  rtdb->setSecondInput(parInput1);


  
  PndSttHitProducerRealFull* sttHitProducer = new PndSttHitProducerRealFull();
  sttHitProducer->RunTimeBased();
  fRun->AddTask(sttHitProducer);

  PndSttHitSorterTask* sttSorter = new PndSttHitSorterTask(5000, 50, "STTHit", "STTSortedHits", "PndSTT");
  fRun->AddTask(sttSorter);



  // ======================================================================
  //    1) Default Task
  //    2) Separate pixels/strips/noise
  //    Set parameters in pandroot/macro/params/mvd.digi.par  

  // -----   Default Digitization   ---------------------------------------
  PndMvdDigiTask* mvddigi = new PndMvdDigiTask();
  mvddigi->SetVerbose(iVerbose);
  mvddigi->RunTimeBased();
  fRun->AddTask(mvddigi);


  // -----   OR custom settings to switch off something   -----------------
//  PndMvdStripHitProducer* mvdStripProd = new PndMvdStripHitProducer();
//  mvdStripProd->SetVerbose(iVerbose);
//  mvdStripProd->RunTimeBased();
//  fRun->AddTask(mvdStripProd);
//  PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer();
//  mvdPixProd->SetVerbose(0);
//  mvdPixProd->RunTimeBased();
//  fRun->AddTask(mvdPixProd);

//  PndSdsDigiPixelSorterTask* sort = new PndSdsDigiPixelSorterTask(4000, 20, "MVDPixelDigis", "MVDSortedPixelDigis", "PndMvd");
//  fRun->AddTask(sort);
//  PndSdsDigiStripSorterTask* sortstrip = new PndSdsDigiStripSorterTask(1000, 10, "MVDStripDigis", "MVDSortedStripDigis", "PndMvd");
//  fRun->AddTask(sortstrip);

//  PndMvdNoiseProducer* mvdNoiseMaker = new PndMvdNoiseProducer();
//  mvdNoiseMaker->RunTimeBased();
//  mvdNoiseMaker->SetVerbose(iVerbose);
//  fRun->AddTask(mvdNoiseMaker);
//  PndMvdPixelDigiSorterTask* mvdPixelSorter = new PndMvdPixelDigiSorterTask();
//  fRun->AddTask(mvdPixelSorter);

  rtdb->setOutput(parInput2);
  rtdb->print();
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,nEvents);

  rtdb->saveOutput();
  rtdb->print();

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s\a\a" << endl;
  cout << endl;

}
