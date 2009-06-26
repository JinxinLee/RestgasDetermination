void run_reco_sttcombi(  Int_t nEvents = 10){
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;

  // Input file
  TString inDigiFile = "data/digi_sttcombi.root";
  TString inSimFile = "data/points_sttcombi.root";

  // Parameter file
  TString parFile = "data/params_sttcombi.root";

  // Output file
  TString outFile = "data/reco_sttcombi.root";
  TString lhefile = "data/lhepidmaker.root";

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->Load("libGem");
  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  // ------------------------------------------------------------------------
  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
	fRun->SetInputFile(inSimFile);
	fRun->AddFriend(inDigiFile);
//	fRun->SetInputFile(inDigiFile);
//	fRun->AddFriend(inSimFile);
  fRun->SetOutputFile(outFile);
  FairGeane *Geane = new FairGeane(inSimFile.Data());
  PndEmcMapper::Instance(2,inSimFile);
  // ------------------------------------------------------------------------

  // -----  Parameter database   --------------------------------------------
   TString allDigiFile = sysFile+"/macro/params/all.par";

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());

  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  rtdb->setSecondInput(parIo1);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  // -----   LHETRACK  ---------------------------------

  PndLheHitsMaker* trackMS = new PndLheHitsMaker("Tracking routine");
  trackMS->SetVerbose( Bool_t(iVerbose>0) );
  trackMS->SetTpcMode(0);
  trackMS->SetSttMode(3);  // 0 OFF, 1 SttPoint, 2 SttHit, (3) SttHelixHit // SttPoint smearing [cm], if negative no smearing
  trackMS->SetMvdMode(2);  // 0 OFF, 1 MVDPoint, 2 MVDHit     // MVDPoint smearing [cm], if negative no smearing
  trackMS->SetGemMode(2);  // 0 OFF, 1 GEMPoint, 2 GEMHit     // GEMPoint smearing [cm], if negative no smearing 
  fRun->AddTask(trackMS);

  PndTpcLheTrackFinder* trackFinder    = new PndTpcLheTrackFinder();
  fRun->AddTask(trackFinder);

  PndTpcLheTrackFitter* trackFitter    = new PndTpcLheTrackFitter("fitting");
  fRun->AddTask(trackFitter);

  PndLhePidMaker* pidMaker    = new PndLhePidMaker("pid");
  pidMaker->SetGeanePro(kFALSE);  // Switch Geane propagation
  pidMaker->SetDebugMode(kTRUE);  // Debug ntuples
  pidMaker->SetDebugFilename(lhefile);
  fRun->AddTask(pidMaker);

  // -----   Initialise and run   --------------------------------------------
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
