void produceSplitSignals(TString input){

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Input file (MC events)

TString inFile = input; inFile.Append(".mc.root");
TString outFile = input; outFile.Append(".raw.root");
//  TString inFile = "data/pions.mc.root";
//TString inFile = "data/etacphiphi.mc.root";
//TString inFile = "$PANDAMC/FAIRRoot/lambdas.mc.root";
  // Output file
  //TString outFile = "$PANDAMC/FAIRRoot/lambdas.noDt.raw.root";
//TString outFile = "data/pions.raw.root";
  //TString outFile = "data/test.raw.root";

  // Number of events to process
  Int_t nEvents = 1000;


  // Parameter file
  TString parFile = input + ".param.root";


  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libtpc");
  gSystem->Load("libgenfit");
  // ------------------------------------------------------------------------


  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Digitization run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  //CbmParAsciiFileIo* parInput2 = new CbmParAsciiFileIo();
  //TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  //stsDigiFile += "/parameters/sts/sts_digi_new_standard.par";
  //parInput2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
  //rtdb->setSecondInput(parInput2);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  
//  GenfitTask *Genfit= new GenfitTask();
//  fRun->AddTask(Genfit);

// ----- PndTpc Event Mixer: Mixes background tracks to events -------------

    // -----    Digi Sequence  --------------------------------------------
  PndTpcClusterizerTask* tpcClusterizer = new PndTpcClusterizerTask();
//tpcClusterizer->SetPersistence();
  fRun->AddTask(tpcClusterizer);
 
  PndTpcDriftTask* tpcDrifter = new PndTpcDriftTask();
//tpcDrifter->SetPersistence();
  tpcDrifter->SetTransverseDiffuse(true);
  tpcDrifter->SetLongitudinalDiffuse(true);
  tpcDrifter->SetDistort(false);
  fRun->AddTask(tpcDrifter);

  PndTpcGemTask* tpcGem = new PndTpcGemTask();
//tpcGem->SetPersistence();
  fRun->AddTask(tpcGem);

  PndTpcPadResponseTask* tpcPadResponse = new PndTpcPadResponseTask();
  tpcPadResponse->SetPersistence();
  tpcPadResponse->SetMinSigAmp(500);
  fRun->AddTask(tpcPadResponse);

// Split the data into n files
  PndTpcSignalSplitterTask* splitter= new PndTpcSignalSplitterTask();
TString splitbase=input+".sec";
  splitter->SetOutfileBase(splitbase);
  fRun->AddTask(splitter);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------

  splitter->FillEmptyEvents();

  tpcDrifter->WriteHistograms();
  //tpcGem->WriteHistograms();
  tpcPadResponse->WriteHistograms();
  // -----   Finish   -------------------------------------------------------


//delete tpcSplitter;


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
