{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  // Input file (MC events)
  TString MCFile = "MvdTpc_Pions.root";
  // Parameter file
  TString parFile = "MvdParams.root";
  // Parameter output file
  // TString parOutFile = "MvdParams.root";
  // Number of events to process
  Int_t nEvents = 10;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  // ------------------------------------------------------------------------
  // Output file
    PndFileNameCreator creator(MCFile.Data());
    TString DigiFile = creator.GetDigiFileName(false).c_str();
    TString outFile = creator.GetRecoFileName(false).c_str();

    std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
    std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
    std::cout << "RecoFile: " << outFile.Data()<< std::endl;
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
  fRun->SetInputFile(DigiFile);
  fRun->AddFriend(MCFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
//   FairParAsciiFileIo* parInput1 = new FairParAsciiFileIo();
//   parInput1->open(parFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
  /*Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parOutFile);
  rtdb->setOutput(output);
*/
// fRun->LoadGeometry();
  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================

  // -----    MVD hit producer   --------------------------------------------

  Double_t chargecut = 1.e5;
  PndMvdStripClusterTask* mvdmccls = new PndMvdStripClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);

  PndMvdPixelClusterTask* mvdClusterizer = new PndMvdPixelClusterTask();
   mvdClusterizer->SetVerbose(iVerbose);
   fRun->AddTask(mvdClusterizer);

 // -----    STT hit producer   --------------------------------------------
 // trackfinding ....
 /*  PndSttTrackFinderIdeal* sttTrackFinder = new PndSttTrackFinderIdeal(iVerbose);
   PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
   sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
   fRun->AddTask(sttFindTracks);

   // trackmatching ....
   PndSttMatchTracks* sttTrackMatcher = new PndSttMatchTracks("Match tracks", "STT", iVerbose);
   sttTrackMatcher->AddHitCollectionName("STTHit", "STTPoint");
   fRun->AddTask(sttTrackMatcher);

   // trackfitting ....
   PndSttTrackFitter* sttTrackFitter = new PndSttHelixTrackFitter(0);
   PndSttFitTracks* sttFitTracks = new PndSttFitTracks("STT Track Fitter", "FairTask", sttTrackFitter);
   sttFitTracks->AddHitCollectionName("STTHit");
   fRun->AddTask(sttFitTracks);

   // helix hit production ....
   PndSttHelixHitProducer* sttHHProducer = new PndSttHelixHitProducer();
   fRun->AddTask(sttHHProducer);
   */
 // -----    TPC hit producer   --------------------------------------------

   PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
    tpcCF->SetPersistence();
    tpcCF->timeslice(20); // = 4 sample times = 100ns @ 40MHz
    fRun->AddTask(tpcCF);

//   FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
//   output->open(parOutFile.Data());
//   rtdb->setOutput(output);
  rtdb->setOutput(parInput1);
  rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
//   PndMvdGeoPar* geoPar  = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,nEvents);
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
