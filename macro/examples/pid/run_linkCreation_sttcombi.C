{
  // ========================================================================
	  Int_t iVerbose = 0;
	  TString MCFile = "points_sttcombi.root";
	  TString parFile = "params_sttcombi.root";
	  //Int_t nEvents = 10;
	  Int_t startEvent = 0;
	  Int_t stopEvent = 10;
	  // ----  Load libraries   -------------------------------------------------
	  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	  // ------------------------------------------------------------------------
	  // Output file

	  TString DigiFile = "digi_sttcombi.root";
	  TString RecoFile = "reco_sttcombi.root";
	  TString PidFile = "pid_sttcombi.root";
	  TString outFile = "links_sttcombi.root";

	  std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
	  std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
	  std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
	  std::cout << "PidFile: " << PidFile.Data() << std::endl;

	  gSystem->Load("libMCMatch");


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

  fRun->SetInputFile(MCFile);
  fRun->AddFriend(DigiFile);
  fRun->AddFriend(RecoFile);
  fRun->AddFriend(PidFile);

  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");

  rtdb->setFirstInput(parInput1);
  Bool_t kParameterMerged=kTRUE;
  rtdb->setOutput(parInput1);
  rtdb->print();


  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================

  // -----    MVD hit producer   --------------------------------------------


  PndMCMatchCreatorTask* mcMatch = new PndMCMatchCreatorTask();
  fRun->AddTask(mcMatch);

  fRun->Init();

  fRun->Run(startEvent,stopEvent);


  // ------------------------------------------------------------------------

// rtdb->saveOutput();
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
