{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file
  TString inDigiFile = "digi_sttcombi.root";
  TString inSimFile = "points_sttcombi.root";

  // Parameter file
  TString parFile = "params_sttcombi.root";

  // Output file
  TString outFile = "reco_sttcombi.root";

  // Number of events to process
  Int_t nEvents = 0;
 
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  // ------------------------------------------------------------------------
  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Digitization run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inDigiFile);
  fRun->AddFriend(inSimFile);
  fRun->SetOutputFile(outFile);
  CbmGeane *Geane = new CbmGeane(inSimFile.Data());
  PndEmcMapper::Instance(2,inSimFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
   TString allDigiFile = sysFile+"/macro/params/all.par";

  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
	
  CbmParAsciiFileIo* parIo1 = new CbmParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
        
  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  // -----   LHETRACK  ---------------------------------
  
  PndTpcLheHitsMaker* trackMS = new PndTpcLheHitsMaker("Tracking routine");
  trackMS->SetSttMode(3);  // 0 OFF, 1 SttPoint, 2 SttHit, (3) SttHelixHit // SttPoint smearing [cm], if negative no smearing
  trackMS->SetMvdMode(2);  // 0 OFF, 1 MVDPoint, 2 MVDHit     // MVDPoint smearing [cm], if negative no smearing
  fRun->AddTask(trackMS);
  
  PndTpcLheTrackFinder* trackFinder    = new PndTpcLheTrackFinder();
  fRun->AddTask(trackFinder);
  
  PndTpcLheTrackFitter* trackFitter    = new PndTpcLheTrackFitter("fitting");
  fRun->AddTask(trackFitter);
  
  PndLhePidMaker* pidMaker    = new PndLhePidMaker("pid");
  pidMaker->SetGeanePro(kFALSE);  // Switch ON Geane propagation
  pidMaker->SetDebugMode(kTRUE);  // Debug ntuples
  fRun->AddTask(pidMaker);
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  Geane->SetField(fRun->GetField());
  fRun->Run(0, nEvents);

  rtdb->saveOutput();
  rtdb->print();

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
