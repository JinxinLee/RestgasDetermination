{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Input file (MC events)
  TString inFile = "testrun1.root";

  // Number of events to process
  Int_t nEvents = 0;

  // Parameter file
  TString parFile = "params_testrun1.root";

  // Output file
  TString outFile = "hit1.root";
  
  // ----  Load libraries   -------------------------------------------------
  //gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  basiclibs();

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libDrcProp");  
  gSystem->Load("libDrc");
  gSystem->Load("libTof");
  gSystem->Load("libGen");


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
  
  // -----    DRC hit producer   --------------------------------------------
 
//  PndDrcHitProducerIdeal* drchit = new PndDrcHitProducerIdeal();
  PndDrcHitProducerReal* drchit = new PndDrcHitProducerReal();
  drchit->SetVerbose(iVerbose);
  drchit->SetIsPixelization(1);
  drchit->SetIsDetEfficiency(1);
  fRun->AddTask(drchit);
  
  // -----    DRC hit Analyzer   --------------------------------------------
  
  DrawHits* drawhit = new DrawHits(1);
  fRun->AddTask(drawhit);
 
 
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
