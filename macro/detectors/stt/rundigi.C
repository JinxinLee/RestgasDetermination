{
  
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Input file (MC events)
  TString inFile = "testrun.root";

  // Number of events to process
  Int_t nEvents = 0;

  // Parameter file
  TString parFile = "testparams.root";

  // Output file
  TString outFile = "testdigi.root";

  // In general, the following parts need not be touched
  // ========================================================================

//   // ----  Load libraries   -------------------------------------------------
//   gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
//   rootlogon();
//   // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun = new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);

  // ------------------------------------------------------------------------

  // -----   STT analysis tasks   --------------------------------------------
  // digitize ....

  //PndSttHitProducerIdeal* sttHitProducer = new PndSttHitProducerIdeal();
  PndSttHitProducerRealFast* sttHitProducer = new PndSttHitProducerRealFast();
  fRun->AddTask(sttHitProducer);
  // ------------------------------------------------------------------------

  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
  // PndGeoSttPar *sttPar = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
  

  // -----   Initialize and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
}
