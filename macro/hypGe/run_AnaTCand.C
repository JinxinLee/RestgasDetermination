{

  // ========================================================================
  
  
// Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;//iVerbose = 3
  TString inFile = "sim_hypGe_BgUrqmd2.root";
  Int_t nEvents = 0;//50000;
  TString parFile="simparams.root";
  TString outFile = "hypGe_IdealTC.root";

 // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================

 // ----  Load libraries   -------------------------------------------------
  
 gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
 rootlogon();

 gSystem->Load("libHypGe"); 

// -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
 
  // ------------------------------------------------------------------------
  
  // -----   Reconstruction run   -------------------------------------------
  
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile.Data());
/*fRun->AddFile(inFile2.Data());
  fRun->AddFile(inFile3.Data());
  fRun->AddFile(inFile4.Data());*/
  //fRun->AddFile(inFile5.Data());
  fRun->SetOutputFile(outFile.Data());
  // ------------------------------------------------------------------------

  
  
  // -----  Parameter database   --------------------------------------------
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  Bool_t kParameterMerged=kTRUE;
  rtdb->setFirstInput(parInput1);

  //fRun->LoadGeometry();
  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  // -----    HYPGE track Cand producer   --------------------------------------------
  PndHypGeIdealTrackFinderTask* idealt = new PndHypGeIdealTrackFinderTask();
  idealt->SetVerbose(iVerbose);
  fRun->AddTask(idealt);

  
  
  // =========================================================================
  
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  
  fRun->Run(0,nEvents);
  // --     retrieving info to histograms --------

  idealt->WriteHistograms();
  
     
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
delete fRun;
  exit(0);

}
