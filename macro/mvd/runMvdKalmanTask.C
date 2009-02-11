{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;
  // Input file (MC events)
  TString MCFile = "Mvd_Test.root";
  // Parameter file
  TString parFile = "MvdParams.root";
  // Parameter output file
  // TString parOutFile = "MvdParams.root";
  // Number of events to process
  Int_t nEvents = 2;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("Libs.C");
  gSystem->Load("libGeane");
  // ------------------------------------------------------------------------
  // Output file
    PndMvdFileNameCreator creator(MCFile.Data());
    TString DigiFile = creator.GetDigiFileName(false).c_str();
    TString RecoFile = creator.GetRecoFileName(false).c_str();
    TString TrackFile  = creator.GetTrackFindingFileName(false).c_str();
    TString outFile = creator.GetKalmanFileName(false).c_str();
    
    std::cout << "DigiFile: " << DigiFile.Data()<< std::endl;
    std::cout << "RecoFile: " << RecoFile.Data()<< std::endl;
    std::cout << "TrackFinderFile: " << TrackFile.Data()<< std::endl;
    std::cout << "KalmanFile: " << outFile.Data() << std::endl;
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
//   fRun->AddFriend(DigiFile);
  fRun->AddFriend(RecoFile);
  fRun->AddFriend(TrackFile);
  
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
//  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  FairParAsciiFileIo* parInput1 = new FairParAsciiFileIo();
  parInput1->open(parFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
  /*Bool_t kParameterMerged=kTRUE;
  FairParRootFileIo* output=new FairParRootFileIo(kParameterMerged);
  output->open(parOutFile);
  rtdb->setOutput(output);
*/  fRun->LoadGeometry();
  // ------------------------------------------------------------------------



  // =========================================================================
  // ======                       Hit Producers                         ======
  // =========================================================================
  
  // -----    MVD hit producer   --------------------------------------------
 
  PndMvdKalmanTask* mvdKalman = new PndMvdKalmanTask();
  mvdKalman->SetVerbose(iVerbose);
  fRun->AddTask(mvdKalman);

//  FairParRootFileIo* output=new FairParRootFileIo(kTRUE);
//  output->open(parOutFile.Data());
//  rtdb->setOutput(output);
 rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
     PndMvdGeoPar* geoPar  = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar")); 
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------
  mvdKalman->WriteHistograms("MvdKalmanHistos.root");

  TFile histos("MvdKalmanHistos.root","READ");

//  rtdb->saveOutput();
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
