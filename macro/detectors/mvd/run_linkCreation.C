{
  // ========================================================================
	  Int_t iVerbose = 0;
	TString MCFile = "Mvd_Test.root";
	  TString parFile = "Mvd_Params.root";
	  Int_t nEvents = 1000000;
	  // ----  Load libraries   -------------------------------------------------
	  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	  // ------------------------------------------------------------------------
	  // Output file
	  PndFileNameCreator creator(MCFile.Data());
	  TString DigiFile = creator.GetDigiFileName(false).c_str();
	  TString RecoFile = creator.GetRecoFileName(false).c_str();
	  TString FitFile = creator.GetTrackFindingFileName(false).c_str();

	  TString outFile = "Mvd_Test_links.root";

	  std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
	  std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
	  std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
	  std::cout << "FitFile:  " << FitFile.Data() << std::endl;

	  gSystem->Load("libMCMatch");
	  gSystem->Load("libMCMatchExamples");


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

  fRun->SetInputFile(MCFile);
  fRun->AddFriend(DigiFile);
  fRun->AddFriend(RecoFile);
  fRun->AddFriend(FitFile);
//  fRun->AddFriend(PidFile);

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

 PndMCTestPatterRecoQuality* mcTest = new PndMCTestPatternRecoQuality();
 fRun->AddTask(mcTest);

  fRun->Init();
  fRun->Run(0,nEvents);

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
