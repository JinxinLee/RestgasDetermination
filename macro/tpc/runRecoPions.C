{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;


  
  
  // Number of events to process
  Int_t nEvents = 100;


  // Parameter file
  TString parFile = "testparams.root";


  TString base="data/pion.30deg.nodiff";
  // Input file (raw data events)
  //TString inFile = "data/etacphiphi.raw.root";
  //TString inFile = "data/pions.raw.root";
  //TString inFile = "/afs/e18/data/panda/MC/FAIRRoot/lambdas.raw.root";

  TString inFile=base; inFile.Append(".raw.root");
  TString mcFile=base; mcFile.Append(".mc.root");
  TString outFile=base; outFile.Append(".reco.root");
  
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libgenfit");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");


  // ------------------------------------------------------------------------


  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------



  // -----   Digitization run   -------------------------------------------
  CbmRunAna *fRun= new CbmRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend(mcFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  CbmRuntimeDb* rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo* parInput1 = new CbmParRootFileIo();
  parInput1->open(parFile.Data());
  //CbmParAsciiFileIo* parInput2 = new CbmParAsciiFileIo();
  //TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  //stsDigiFile += "/parameters/sts/sts_digi_new_standard.par";
  //parInput2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
  //rtdb->setSecondInput(parInput2);
  fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  
//  GenfitTask *Genfit= new GenfitTask();
//  fRun->AddTask(Genfit);

// ----- PndTpc Event Mixer: Mixes background tracks to events -------------

    // -----    Reco Sequence  --------------------------------------------
  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  
  tpcCF->SetPersistence();
  tpcCF->timeslice(20); // = 4 sample times = 100ns @ 40MHz
//tpcCF->SetTrivialClustering();
  fRun->AddTask(tpcCF);


  PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
tpcSPR->SetTrkFinderParameters(2.,// proxcut
			       0.1, // proxcut on rieman sphere
			       2.E-3, // planecut
			       4.0, // szcut
			       4); // minnumhits for fit
  tpcSPR->SetPersistence();
  fRun->AddTask(tpcSPR);

  KalmanTask* kalman =new KalmanTask();
  fRun->AddTask(kalman);


  TrackFitStatTask* fitstat=new TrackFitStatTask();
  fitstat->SetPersistence();
  fitstat->SetMCPCut(0.1);
  fitstat->SetMCCuts(0.1, // pmin
	             10., // pmax
		     0.,   // thetamin 5deg
		     TMath::Pi()/2.,  // thetamax
		     20); // nPndTpcPoints
  fitstat->SetPdgSelection(211);
  fitstat->DoResiduals();
  fRun->AddTask(fitstat);


  PndTpcTrackVisTask* trkVis = new PndTpcTrackVisTask();
  trkVis->SetTrackBranchName("TrackPreFit");
//fRun->AddTask(trkVis);

  LambdaSelector* lambdaSel = new LambdaSelector();
  lambdaSel->SetTrackBranchName("TrackPreFit");
  lambdaSel->SetPersistence();
//fRun->AddTask(lambdaSel);
 
  LambdaStatTask* lambdaStat = new LambdaStatTask();
  lambdaStat->SetPersistence();
//fRun->AddTask(lambdaStat);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,nEvents);
  // ------------------------------------------------------------------------



  // -----   Finish   -------------------------------------------------------

  tpcSPR->WriteHistograms("RecoHistos.root");
  kalman->WriteHistograms("RecoHistos.root");
  fitstat->WriteHistograms("RecoHistos.root");

  DebugLogger::Instance()->WriteFiles();

//delete tpcSplitter;


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
