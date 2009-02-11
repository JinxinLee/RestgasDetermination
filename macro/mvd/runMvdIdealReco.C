{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("Libs.C");
  // Input file (MC events)
  TString MCFile = "Mvd_Test.root";
  // Parameter file
  TString parFile = "MvdParams.root";
  // Parameter output file
  // TString parOutFile = "MvdParams.root";
  // Number of events to process
  Int_t nEvents = 100;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("Libs.C");
  // ------------------------------------------------------------------------
  // Output file
    PndMvdFileNameCreator creator(MCFile.Data());
    TString DigiFile = creator.GetDigiFileName(false).c_str();
    TString outFile = creator.GetRecoFileName(false).c_str();
    
    std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
    std::cout << "RecoFile: " << outFile.Data()<< std::endl;

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();


  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->SetOutputFile(outFile);


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


  // -----   Ideal Track Producers   ---------------------------------------
  PndMvdIdealRecoTask* mvdirec = new PndMvdIdealRecoTask(0.01,0.01,0.005);
  mvdirec->SetVerbose(iVerbose);
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

