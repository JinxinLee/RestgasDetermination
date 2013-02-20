{

  // ========================================================================
  
  
// Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;//iVerbose = 3
  TString inFile = "sim_piTr.root";//sim_pidC.root";
  Int_t nEvents = 0;//50000;
  TString parFile="PiTrparams.root";//pidparams.root";
  TString RecoFile = "hypIdealreco.root";
  //hypIdealPidreco.root";
  TString outFile = "HypIdrecdos2.root";//HypPidIdrecdos2.root";
 
 // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================

 // ----  Load libraries   -------------------------------------------------
  
 gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
 rootlogon();

 gSystem->Load("libHyp"); 

// -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
 
  // ------------------------------------------------------------------------
  
  // -----   Reconstruction run   -------------------------------------------
  
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile.Data());
  fRun->AddFriend(RecoFile);
/*fRun->AddFile(inFile2.Data());
  fRun->AddFile(inFile3.Data());
  fRun->AddFile(inFile4.Data());*/
  //fRun->AddFile(inFile5.Data());
  fRun->SetOutputFile(outFile.Data());
  
// ------------------------------------------------------------------------
  // ----- Prepare GEANE --------------------------------------------
  // this will load Geant3 and execute setup macros to initialize geometry:
  //
  FairGeane *Geane = new FairGeane();
  //fRun->AddTask(Geane);
  fRun->AddTask(Geane);
  // -----  Parameter database   --------------------------------------------
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  
  Bool_t kParameterMerged=kTRUE;
  rtdb->setFirstInput(parInput1);

  //fRun->LoadGeometry();
  // ------------------------------------------------------------------------

   //Geane->SetField(fRun->GetField());

  // =========================================================================
  // ======                       Reco Sequence                        ======
  // =========================================================================
  
   PndHypDPatternRecoTask* DemoPR = new PndHypDPatternRecoTask();
   DemoPR->AddHitBranch(2,"HypHit");
   //DemoPR->AddHitBranch(2,"PndTpcPoint");
   //DemoPR->UseMVD(kTRUE);
   //DemoPR->AddHitBranch(3,"MVDHit");
   DemoPR->SetPersistence();
   DemoPR->UseGeane();
   //DemoPR->SetHitFL(kTRUE);//starting fit values from first hit in HYP
   
   fRun->AddTask(DemoPR);
   
   PndHypDKalmanTask* DemoKalman = new PndHypDKalmanTask();
   DemoKalman->AddHitBranch(2,"HypHit");
   //DemoKalman->AddHitBranch(2,"PndTpcPoint");
   //DemoKalman->UseMVD(kTRUE);
   //DemoKalman->AddHitBranch(3,"MVDHit");
   DemoKalman->SetPersistence();
   
   fRun->AddTask(DemoKalman);
   
  
  
  // =========================================================================
  
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  

  fRun->Run(0,nEvents);
  
  DemoKalman->WriteHistograms(outFile.Data());
     
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
