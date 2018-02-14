{
  // ========================================================================
	  Int_t iVerbose = 0;
	  TString parFile = "simparams.root";
	  //Int_t nEvents = 10;
	  Int_t startEvent = 0;
	  Int_t stopEvent = 10;
	  // ----  Load libraries   -------------------------------------------------
	  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	  // ------------------------------------------------------------------------
	  // Output file

	  TString linkFile = "links_emc.root";
	  TString outFile = "Dummy.root";


	  std::cout << "LinkFile: " << linkFile.Data() << std::endl;

	  gSystem->Load("libMCMatch");


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

  fRun->SetInputFile(linkFile);

  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
//   FairParAsciiFileIo* parInput1 = new FairParAsciiFileIo();
//   parInput1->open(parFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  Bool_t kParameterMerged=kTRUE;
//  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
//  output->open(parOutFile.Data(),"RECREATE");
  rtdb->setOutput(parInput1);
  rtdb->print();
//  fRun->LoadGeometry();

  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================

  // -----    MVD hit producer   --------------------------------------------


  PndMCMatchLoaderTask* mcMatch = new PndMCMatchLoaderTask();
  fRun->AddTask(mcMatch);

  PndMCMatchSelectorTask* mcSelector = new PndMCMatchSelectorTask(16,1); // MC Match for charged particles
  mcSelector->SetAllWeights(1.);
  fRun->AddTask(mcSelector);

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
