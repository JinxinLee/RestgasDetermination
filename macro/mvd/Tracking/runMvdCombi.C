runMvdCombi()
{
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0;
  Int_t nEvents = 1000;

  // ----  Load libraries   -------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  // Number of events to process
  // Parameter file
  TString parFile = "../data/MvdTrackingParams.root";
  PndMvdFileNameCreator namecreator("../data/MvdTracking.root");
  std::string simFile = namecreator.GetSimFileName();
  std::string outFile = namecreator.GetRecoFileName();
  TString digiparFile = gSystem->Getenv("VMCWORKDIR");
  digiparFile += "/mvd/MvdTools/mvd.digi.par";

  // -----   Reconstruction run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(simFile.c_str());
  fRun->SetOutputFile(outFile.c_str());

  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  rtdb->setFirstInput(parInput1);

  CbmParAsciiFileIo* parInput2 = new CbmParAsciiFileIo();
  parInput2->open(digiparFile.Data(),"in");
  rtdb->setSecondInput(parInput2);

  fRun->LoadGeometry();



  //----------------------------------------------------------------------//
  // DIGI
//   double   topPitch=0.015,//cm
//            botPitch=0.015,//cm
//            orient=TMath::Pi()*(0.5),
//            skew=TMath::Pi()*(0.5);
//   TVector2 topAnchor(0.,0.);
//   TVector2 botAnchor(0.,0.);
//   int      topFE = 10,
//            botFE = 4,
//            nrFEChannels = 128;
//   double   threshold=3000., noise=1000.;
//   PndMvdStripHitProducer* mvdHitProd
//     = new PndMvdStripHitProducer(topPitch, botPitch,
//                               orient, skew,
//                               topAnchor,botAnchor,
//                               topFE, botFE, nrFEChannels,
//                               threshold, noise);

  PndMvdStripHitProducer* mvdHitProd = new PndMvdStripHitProducer();
  mvdHitProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdHitProd);

  Double_t  lx=0.01, ly=0.01, threshold=600, noise=200;
  PndMvdHybridHitProducer* mvdPixProd = new PndMvdHybridHitProducer(lx,ly,threshold,noise);
  mvdPixProd->SetVerbose(iVerbose);
  fRun->AddTask(mvdPixProd);
  //----------------------------------------------------------------------//
  //----------------------------------------------------------------------//




  //----------------------------------------------------------------------//
  // CLUST
  // Cluster finding for strip detectors
  Double_t noise = 1000.; /// put such stuff inside the parameter lists
  Double_t chargecut = 3. * noise;
  PndMvdStripClusterTask* mvdmccls = new PndMvdStripClusterTask(chargecut,namecreator.GetSimFileName(true));
  mvdmccls->SetVerbose(iVerbose);
  fRun->AddTask(mvdmccls);
  // Cluster finder for pixel detectors
  PndMvdPixelClusterTask* mvdClusterizer = new
    PndMvdPixelClusterTask(1.8,76,84, namecreator.GetSimFileName(true));
  mvdClusterizer->SetVerbose(iVerbose);
  fRun->AddTask(mvdClusterizer);
  //----------------------------------------------------------------------//
  //----------------------------------------------------------------------//



  //----------------------------------------------------------------------//
  // TRACKFINDER
  PndMvdIdealTrackFinderTask* mvdTrackFinder = new PndMvdIdealTrackFinderTask();
  mvdTrackFinder->SetVerbose(iVerbose);
  fRun->AddTask(mvdTrackFinder);
  //----------------------------------------------------------------------//


  // KALMAN
//   PndMvdKalmanTask* mvdKalman = new PndMvdKalmanTask();
//   mvdKalman->SetVerbose(3);
//   fRun->AddTask(mvdKalman);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);

//   mvdKalman->WriteHistograms("MvdKalmanHistos.root");
//   TFile histos("MvdKalmanHistos.root","READ");

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

