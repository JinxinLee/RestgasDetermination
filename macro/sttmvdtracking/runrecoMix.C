// Macro to reconstruct the MVD data and STT data; also the Mvd+Stt Pattern
// Recognition by Gianluigi is performed.
//
// Updated 6.12.2010
// Gianluigi Boca
{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  TString MCFile = "MvdStt_Test.root";
  TString parFile = "MvdStt_Params.root";
  Int_t nEvents = 5;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");  
  rootlogon();
  gSystem->Load("libSttMvdTracking");

  // ------------------------------------------------------------------------
  // Output file
  PndFileNameCreator creator(MCFile.Data());
  TString DigiFile = creator.GetDigiFileName(false).c_str();
  TString outFile = creator.GetRecoFileName(false).c_str();
  
  std::cout << "MCFile  : " << MCFile.Data()<< std::endl;
  std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
  std::cout << "RecoFile: " << outFile.Data()<< std::endl;
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile);
  fRun->AddFriend(DigiFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
  rtdb->setFirstInput(parInput1);

  Bool_t kParameterMerged=kTRUE;


  // =========================================================================
  //-------------------------- MixBackgroundEvents  task ----------------------------------
  PndMixBackgroundEvents *  MixBackgroundEvents = new PndMixBackgroundEvents(2);
  // set the filename (digifile) from which the background is taken.
  MixBackgroundEvents->SetInputBkgFilesName(
       "Mixed_digi.root");

  fRun->AddTask(MixBackgroundEvents);


  // =========================================================================

  // =========================================================================
  // ======                       Riemann finder MVD                    ======
  // =========================================================================
  PndMvdRiemannTrackFinderTask* mvdTrackFinder = new PndMvdRiemannTrackFinderTask();
  mvdTrackFinder->AddHitBranch("MVDHitsPixelMix");
  mvdTrackFinder->AddHitBranch("MVDHitsStripMix");
  mvdTrackFinder->SetVerbose(iVerbose);
  mvdTrackFinder->SetMaxDist(0.05);
  fRun->AddTask(mvdTrackFinder);
  // -----  end  MVD  Rieman finder  --------------------------------------------



  // Stt TRACK FINDING =======================================================
  // OUTPUT: PndTrackCand                    -> STTTrackCand
  PndSttTrackFinderReal* sttTrackFinder = new PndSttTrackFinderReal(0);
  PndSttFindTracks* sttFindTracks =
	new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
  sttFindTracks->AddHitCollectionName("STTHitMix", "STTPoint");
//  sttFindTracks->AddHitCollectionName("STTHit", "STTPoint");
  fRun->AddTask(sttFindTracks);
  // =========================================================================

  // =========================================================================
  //-------------------------- stt-mvd   task ----------------------------------
  PndSttMvdTracking *  SttMvdTracking = new PndSttMvdTracking(0);
  SttMvdTracking->SetInputBranchName("STTHitMix","MVDHitsPixelMix","MVDHitsStripMix");
  fRun->AddTask(SttMvdTracking);
  // =========================================================================

  rtdb->setOutput(parInput1);
  //  rtdb->print();
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------

//  SttMvdTracking->WriteHistograms();

  rtdb->saveOutput();
  rtdb->print();
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
