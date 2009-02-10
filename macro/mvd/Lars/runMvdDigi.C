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
  PndMvdFileNameCreator creator(inFile.Data());
  TString outFile = creator.GetDigiFileName().c_str();
  std::cout << "DigiFileName: " << outFile.Data() << std::endl;

  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);


  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);
//   Bool_t kParameterMerged=kTRUE;
//   CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
//   output->open(parOutFile);
//   rtdb->setOutput(output);
  CbmParAsciiFileIo* parInput2 = new CbmParAsciiFileIo();
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


//   CbmParRootFileIo* output=new CbmParRootFileIo(kTRUE);
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
