// Example of Macro showing how to do the mixing of the physical event with
// background events.
// This is the equivalent of the    runreco.C      Macro.
//
// Updated 22.3.2011
// Gianluigi Boca
{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  TString MCFile = "MvdStt_Test.root";	// name of the 'normal' MC physics event digi file.
  TString parFile = "MvdStt_Params.root";// name of the 'normal' MC physics event Param input file.
  Int_t nEvents = 0;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");  
  rootlogon();
  gSystem->Load("libSttMvdTracking");

  // ------------------------------------------------------------------------
  // Output file
  TString DigiFile = "MvdStt_Test_digi.root" ; // output, digi file;
  TString outFile = "MvdStt_Test_reco.root";   // output, reco file.

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




// -----    Default MVD hit producer   --------------------------------------------
  PndMvdClusterTask* mvdmccls = new PndMvdClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);
// =========================================================================




  // =========================================================================
  //-------------------------- MixBackgroundEvents  task ----------------------------------
// The following is the task that mixes physics evt and "bkg events". The latter
// are already digitized and for them also the MVD hit producer PndMvdClusterTask
// must already have been run in the digitization Macro.
   PndMixBackgroundEvents *  mix = new PndMixBackgroundEvents(0);
	//  here two Background input files are given :
	//  1)  the DIGI file of the bkg, to extract the Stt bkg  hits;
	//  2)  the reconstructed bkg file ( necessary for extracting the Mvd bkg hits)
	//      previously produced by the Macro runPreliminaryMvdReco.C (Mix_Generation_reco.root)
	//  The user must change the filenames here obviously!
   mix->SetInputBkgFilesName("../dpm/MvdStt_Test_digi.root");// Mvd and Stt DPM background file.
   fRun->AddTask(mix);
  // =========================================================================



//   in the following there is an example of   Mvd and Stt Pattern Recognition tasks. 




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
//  use the constructor with input :
//	  printout flag (int) , plotting flag (bool), MC comparison flag (bool).
  PndSttTrackFinderReal* sttTrackFinder = new PndSttTrackFinderReal(0,false,true);

  PndSttFindTracks* sttFindTracks = new PndSttFindTracks("Track Finder", "FairTask", sttTrackFinder, iVerbose);
  sttFindTracks->AddHitCollectionName("STTHitMix", "STTPoint");
  fRun->AddTask(sttFindTracks);
  // =========================================================================



  // =========================================================================
  //-------------------------- stt-mvd   task ----------------------------------
//  use the constructor with input :
//	  printout flag (int) , plotting flag (bool), MC comparison flag (bool).
  PndSttMvdTracking*  sttmvd = new PndSttMvdTracking(0,false,false);
  sttmvd->SetInputBranchName("STTHitMix","MVDHitsPixelMix","MVDHitsStripMix");
  fRun->AddTask(sttmvd);
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
