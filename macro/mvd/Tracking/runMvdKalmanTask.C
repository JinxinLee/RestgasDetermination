{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 3;
  Int_t nEvents = 100;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
//   gSystem->Load("libGeane");
  // ------------------------------------------------------------------------
  // Output file
    TString parFile = "../data/MvdTrackingParams.root";
    PndMvdFileNameCreator namecreator("../data/MvdTracking.root");
    namecreator.SetVerbose(1);
    TString MCFile = namecreator.GetSimFileName(false).c_str();
    TString DigiFile = namecreator.GetDigiFileName(false).c_str();
    TString RecoFile = namecreator.GetRecoFileName(false).c_str();
    TString TrackFile  = namecreator.GetTrackFindingFileName(false).c_str();
    TString outFile = namecreator.GetKalmanFileName(false).c_str();
    
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
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(MCFile);
//   fRun->AddFriend(DigiFile);
  fRun->AddFriend(RecoFile);
  fRun->AddFriend(TrackFile);
  
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // THIS IS STRONGLY NEEDED
  CbmGeane *Geane = new CbmGeane(MCFile.Data());
  Geane->SetField(fRun->GetField());


  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------


 
//   PndMvdKalmanTask* mvdKalman = new PndMvdKalmanTask();
//   mvdKalman->SetVerbose(iVerbose);
//   fRun->AddTask(mvdKalman);

  PndLheKalmanTask* lheKalman = new PndLheKalmanTask();
  lheKalman->SetVerbose(iVerbose);
  //lheKalman->SetGeane(kTRUE);
  lheKalman->SetSmooth(kTRUE);
  lheKalman->SetNumIterations(3);
  fRun->AddTask(lheKalman);


 rtdb->print();
  // =====                 End of HitProducers                           =====
  // =========================================================================
     PndMvdGeoPar* geoPar  = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar")); 
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();

  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------
//   mvdKalman->WriteHistograms("MvdKalmanHistos.root");

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
