{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  // Input file (MC events)
  TString MCFile = "Mvd_D+D-_10G.root";
  // Parameter file
  TString parFile = "MvdParams.root";
  // Parameter output file
  TString parOutFile = "MvdParams.root";
  // Number of events to process
  Int_t nEvents = 1000;
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  //gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");


  gSystem->Load("libriemann");

  // ------------------------------------------------------------------------
  // Output file
    PndFileNameCreator creator(MCFile.Data());
    TString DigiFile = creator.GetDigiFileName(false).c_str();
    TString RecoFile = creator.GetRecoFileName(false).c_str();
    TString outFile  = creator.GetTrackFindingFileName(false).c_str();

    std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
    std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
    std::cout << "TrackFinderFile: " << outFile.Data()<< std::endl;

  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------
  // In general, the following parts need not be touched
  // ========================================================================

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->AddFriend(DigiFile);
  fRun->AddFriend(RecoFile);

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
  // ------------------------------------------------------------------------



  // -----   LHETRACK  ---------------------------------

   PndLheHitsMaker* trackMS = new PndLheHitsMaker("Tracking routine");
   trackMS->SetTpcMode(2);  // 0 OFF, 1 TpcPoint, 2 TpcCluster // TpcPoint smearing [cm], if negative no smearing
//   trackMS->SetSttMode(3);  // 0 OFF, 1 SttPoint, 2 SttHit, (3) SttHelixHit // SttPoint smearing [cm], if negative no smearing
   trackMS->SetMvdMode(2);  // 0 OFF, 1 MVDPoint, 2 MVDHit     // MVDPoint smearing [cm], if negative no smearing
   trackMS->SetVerbose(3);
   fRun->AddTask(trackMS);

   PndLheTrackFinder* trackFinder    = new PndLheTrackFinder();
   fRun->AddTask(trackFinder);

   PndLheTrackFitter* trackFitter    = new PndLheTrackFitter("fitting");
   fRun->AddTask(trackFitter);

   PndLhePidMaker* pidMaker    = new PndLhePidMaker("pid");
   pidMaker->SetGeanePro(kFALSE);  // Switch ON Geane propagation
   pidMaker->SetDebugMode(kTRUE);  // Debug ntuples
   fRun->AddTask(pidMaker);

/*  PndMvdRiemannTrackFinderTask* mvdTrackFinder = new PndMvdRiemannTrackFinderTask();
  mvdTrackFinder->SetVerbose(iVerbose);
  mvdTrackFinder->SetMaxDist(0.05);
  fRun->AddTask(mvdTrackFinder);
*/
// FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
// output->open(parOutFile.Data());
 rtdb->setOutput(parInput1);
 rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
     PndMvdGeoPar* geoPar  = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,nEvents);
 // fRun->Run(96,97);
  // ------------------------------------------------------------------------

 rtdb->saveOutput();
 rtdb->print();
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
