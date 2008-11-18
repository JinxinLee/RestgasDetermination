void runGemSmearing(Int_t nEvents = 10,int verboseLevel = 0)
{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
//  Int_t iVerbose = 1;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libGem");
  // Input file (MC events)
  TString MCFile = "Gem_Test.root";
  // Parameter file
  TString parFile = "GemParams.root";
  // Parameter output file
  // TString parOutFile = "MvdParams.root";
  // Number of events to process
//  Int_t nEvents = 100;
  // ------------------------------------------------------------------------
  // Output file
    TString outFile = "Gem_Test_Smeared.root";

    std::cout << "RecoFile: " << outFile.Data()<< std::endl;

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();


  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(MCFile);
  fRun->SetOutputFile(outFile);


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


  // -----   Ideal Track Producers   ---------------------------------------
  PndGemSmearingTask* mvdirec = new PndGemSmearingTask(0.1,0.1,0.05);
  mvdirec->SetVerbose(verboseLevel);
  fRun->AddTask(mvdirec);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);


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

}

