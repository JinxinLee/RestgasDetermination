{
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  Int_t nEvents = 10000;
// this is a test comment
  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // Number of events to process
  // Parameter file
  TString parFile = "../data/mvddpm6GeV_digipar.root";
  PndFileNameCreator namecreator("../data/mvddpm6GeV.root");
//   TString parFile = "../data/mvdStrip_digipar.root";
//   PndFileNameCreator namecreator("../data/mvdStrip.root");
  // Input file (MC events)
  std::string simFile = namecreator.GetSimFileName();
  // Input data: digis
  std::string inFile = namecreator.GetDigiFileName();
  // Output file
  std::string outFile = namecreator.GetRecoFileName();



  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(simFile.c_str());
  fRun->AddFriend(inFile.c_str());
  fRun->SetOutputFile(outFile.c_str());


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
  fRun->LoadGeometry();

  // Cluster finding for strip detectors
  Double_t chargecut = 5000.;
  PndMvdStripClusterTask* mvdmccls = new PndMvdStripClusterTask();
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);

  // Cluster finder for pixel detectors (radius, geofile)
  PndMvdPixelClusterTask* mvdClusterizer = new
    PndMvdPixelClusterTask();
  mvdClusterizer->SetVerbose(iVerbose);
  fRun->AddTask(mvdClusterizer);

  fRun->AddTask(new PndMvdAccessRTDBTask());

  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);

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

}

