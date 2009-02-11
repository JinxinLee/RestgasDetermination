/*** 
Reconstruct the JPsi from the TPC tracks : Dipak
***/
{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // Input file (MC events)
  //  TString inFile = "dpmtest.root";
  TString inFile = "../../macro/lhetrack/test.raw.root";


  // Output file
  TString outFile = "test.reco.root";

  
  
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
  gSystem->Load("libtpc");
  gSystem->Load("libLHETrack");
  //  gSystem->Load("libfastsim");
  gSystem->Load("libcharmtask");

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
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------


  /*
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
  */
  //  fRun->LoadGeometry();
  // ------------------------------------------------------------------------

  //Add your Task
  PndJPsiExample* jpsiexample = new PndJPsiExample();
  fRun->AddTask(jpsiexample);


  // Number of events to process
  Int_t nEvents = 3000;
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0, nEvents); //process the number selected
  //  fRun->Run(); // process all the events
  // ------------------------------------------------------------------------



  // -----   Finish   -------------------------------------------------------


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  //  cout << "Output file is "    << outFile << endl;
  //  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
