{
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  Int_t nEvents = 100;

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("../Libs.C");


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // Number of events to process
  // Parameter file
//   TString parFile = "../data/mvddpm6GeV_digipar.root";
//   PndMvdFileNameCreator namecreator("../data/mvddpm6GeV.root");
  TString parFile = "../data/mvdStrip_digipar.root";
  PndMvdFileNameCreator namecreator("../data/mvdStrip.root");
  // Input file (MC events)
  std::string simFile = namecreator.GetSimFileName();
  // Input data: digis
  std::string inFile = namecreator.GetDigiFileName();
  // Output file
  std::string outFile = namecreator.GetRecoFileName();



  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(simFile.c_str());
  fRun->AddFriend(inFile.c_str());
  fRun->SetOutputFile(outFile.c_str());


  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);
  fRun->LoadGeometry();

  // Cluster finding for strip detectors
  Double_t noise = 1000.; /// put such stuff inside the task
  Double_t chargecut = 3. * noise;
  PndMvdStripClusterTask* mvdmccls = new PndMvdStripClusterTask(chargecut,simFile.c_str());
  mvdmccls->SetVerbose(iVerbose);
	fRun->AddTask(mvdmccls);

  // Cluster finder for pixel detectors
//                   (radius, FEcol, FErow, geofile)
//   PndMvdPixelClusterTask* mvdClusterizer = new
//     PndMvdPixelClusterTask(1.8,76,84, namecreator.GetSimFileName(true));
//   mvdClusterizer->SetVerbose(iVerbose);
//   fRun->AddTask(mvdClusterizer);

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

