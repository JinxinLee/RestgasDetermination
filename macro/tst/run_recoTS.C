{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Input file (MC events)
  TString inFile = "test3.root";

  // Number of events to process
  Int_t nEvents = 10000;


  // Parameter file
  TString parFile = "testparams.root";

  // Output file
  TString outFile = "100100mu.reco.root";

  
  
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libTst");
  // ------------------------------------------------------------------------


  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  //FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  //TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  //stsDigiFile += "/parameters/sts/sts_digi_new_standard.par";
  //parInput2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
  //rtdb->setSecondInput(parInput2);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  // -----    TST hit producer   --------------------------------------------
  CbmTstHybridHitProducer* tstHitProd = new CbmTstHybridHitProducer(0.1,0.1, 0);
  //CbmTstHitProducerIdeal* tstHitProd = new CbmTstHitProducerIdeal();
  fRun->AddTask(tstHitProd);
 
  // =====                 End of HitProducers                           =====
  // =========================================================================
     
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
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
