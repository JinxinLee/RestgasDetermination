{
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  Int_t nEvents  = 20000;
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  // Input file (MC events)
  TString inFile = "../data/Lars/MvdDtsSim.root";
  // Parameter file
  TString parFile = "../data/Lars/MvdDtsParams.root";
  TString digiparFile = gSystem->Getenv("VMCWORKDIR");
  digiparFile += "/macro/mvd/Lars/mvd.dts.par";

  
  // In general, the following parts need not be touched
  // ========================================================================
  // Output file
  PndFileNameCreator creator(inFile.Data());
  TString outFile = creator.GetDigiFileName().c_str();
  std::cout << "DigiFileName: " << outFile.Data() << std::endl;

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);
//   Bool_t kParameterMerged=kTRUE;
//   FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
//   output->open(parOutFile);
//   rtdb->setOutput(output);
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  parInput2->open(digiparFile.Data(),"in");
  rtdb->setSecondInput(parInput2);

  fRun->LoadGeometry();


  // -----   Digitization   ---------------------------------------
  PndMvdStripHitProducer* mvdStripProd = new PndMvdStripHitProducer();
  mvdStripProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdStripProd);


//  PndMvdNoiseProducer* mvdNoiseMaker = new PndMvdNoiseProducer();
//  mvdNoiseMaker->SetVerbose(iVerbose);
//  fRun->AddTask(mvdNoiseMaker);


//   FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
//   output->open(parOutFile.Data());
//   rtdb->setOutput(output);
  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
//  PndMvdGeoPar* geoPar  = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar")); 
  
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
