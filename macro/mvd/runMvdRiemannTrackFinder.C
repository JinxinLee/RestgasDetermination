{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  // Input file (MC events)
  TString MCFile = "Mvd_TestNewVersion.root";
  // Parameter file
  TString parFile = "MvdParamsNewVersion.root";
  // Parameter output file
  TString parOutFile = "MvdParamsNewVersion.root";
  // Number of events to process
  Int_t nEvents = 100;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  //gROOT->Macro("Libs.C");

  gSystem->Load("libriemann");

  // ------------------------------------------------------------------------
  // Output file
    PndMvdFileNameCreator creator(MCFile.Data());
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
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(RecoFile);
//   fRun->AddFriend(DigiFile);
//  fRun->AddFriend(RecoFile);
  
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();

  CbmParRootFileIo* parInput1 = new CbmParRootFileIo(kTRUE);
  parInput1->open(parFile.Data(),"UPDATE");
//   CbmParAsciiFileIo* parInput1 = new CbmParAsciiFileIo();
//   parInput1->open(parFile.Data(),"in");

  rtdb->setFirstInput(parInput1);
  Bool_t kParameterMerged=kTRUE;
//  CbmParRootFileIo* output=new CbmParRootFileIo(kParameterMerged);
//  output->open(parOutFile.Data(),"RECREATE");
  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  // -----    MVD hit producer   --------------------------------------------
 
  PndMvdRiemannTrackFinderTask* mvdTrackFinder = new PndMvdRiemannTrackFinderTask();
  mvdTrackFinder->SetVerbose(iVerbose);
  mvdTrackFinder->SetMaxDist(0.05);
  fRun->AddTask(mvdTrackFinder);

// CbmParRootFileIo* output=new CbmParRootFileIo(kTRUE);
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
