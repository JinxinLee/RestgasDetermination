{
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 3;
  Int_t nEvents = 100;
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  gSystem->Load("libGeane");
  // ------------------------------------------------------------------------
  // Output file
    TString parFile = "MvdFwdParams.root";
    PndMvdFileNameCreator namecreator("MvdFwd.root");
    std::string MCFile = namecreator.GetSimFileName();
    std::string RecoFile = namecreator.GetRecoFileName();
    std::string TrackFile  = namecreator.GetTrackFindingFileName();
    std::string outFile = namecreator.GetKalmanFileName();
    
    std::cout << "DigiFile: " << DigiFile<< std::endl;
    std::cout << "RecoFile: " << RecoFile<< std::endl;
    std::cout << "TrackFinderFile: " << TrackFile<< std::endl;
    std::cout << "KalmanFile: " << outFile << std::endl;

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(MCFile.c_str());
  fRun->AddFriend(RecoFile.c_str());
  fRun->AddFriend(TrackFile.c_str());
  
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  // THIS IS STRONGLY NEEDED
  FairGeane *Geane = new FairGeane(MCFile.Data());
  Geane->SetField(fRun->GetField());


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------


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
