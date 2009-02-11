{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;


  
  
  // Parameter fil
  TString base = "data/elsa2";

  TString parFile = base +".param.root";
  // Input file (raw data events)
  TString inFile = base+".raw.root";
  TString mcFile = base+".mc.root";
  TString outFile = base+".reco.root";
  //TString inFile = "data/pions.raw.root";
  //TString inFile = "/afs/e18/data/panda/MC/FAIRRoot/lambdas.raw.root";
  //TString inFile="/afs/e18/data/panda/MC/FAIRRoot/pion.40deg.raw.root";

  // Input mc-truth
  //TString mcFile = "data/pions.mc.root";
  //TString mcFile = "/afs/e18/data/panda/MC/FAIRRoot/lambdas.mc.root";
  //TString mcFile="/afs/e18/data/panda/MC/FAIRRoot/pion.40deg.mc.root";
  
  // Output file
  //TString outFile = "/afs/e18/data/panda/MC/FAIRRoot/lambdas.test2.reco.root";
  //TString outFile = "$PANDAMC/FAIRRoot/pions.reco.root";
  //TString outFile="/afs/e18/data/panda/MC/FAIRRoot/pion.40deg.reco.root";
  
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndBase");
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
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend(mcFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
  //FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  //TString stsDigiFile = gSystem->Getenv("VMCWORKDIR");
  //stsDigiFile += "/parameters/sts/sts_digi_new_standard.par";
  //parInput2->open(stsDigiFile.Data(),"in");
  rtdb->setFirstInput(parInput1);
  //rtdb->setSecondInput(parInput2);

  rtdb->print();

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
fitstat->SetMCPCut(3); // in sigma dp/p
  fitstat->SetMCCuts(0.05, // pmin
	             10., // pmax
		     -TMath::Pi(),   // thetamin 5deg
		     TMath::Pi(),  // thetamax
		     20); // nPndTpcPoints
//fitstat->SetPdgSelection(321);
fitstat->DoResiduals();
  fRun->AddTask(fitstat);


  PndTpcTrackVisTask* trkVis = new PndTpcTrackVisTask();
  trkVis->SetTrackBranchName("TrackPreFit");
//fRun->AddTask(trkVis);

  LambdaSelector* lambdaSel = new LambdaSelector();
  lambdaSel->SetTrackBranchName("TrackPreFit");
  lambdaSel->SetPersistence();
// fRun->AddTask(lambdaSel);

  V0Selector* V0Sel = new V0Selector();
  V0Sel->SetTrackBranchName("TrackPreFit");
//V0Sel->SetPositivePartMass(0.938272);
//V0Sel->SetNegativePartMass(0.13957);
//V0Sel->SetPositivePartMass(511.E-6);
//V0Sel->SetNegativePartMass(511.E-6);

  V0Sel->SetPersistence();
//  fRun->AddTask(V0Sel);

V0Selector* V0Sel2 = new V0Selector();
  V0Sel2->SetTrackBranchName("TrackPreFit");
//V0Sel2->SetV0BranchName("Lambda");
//V0Sel->SetPositivePartMass(0.938272);
//V0Sel->SetNegativePartMass(0.13957);
//V0Sel2->SetPositivePartMass(0.13957);
//V0Sel2->SetNegativePartMass(0.13957);

  V0Sel2->SetPersistence();
//fRun->AddTask(V0Sel2);



  LambdaStatTask* lambdaStat = new LambdaStatTask();
 lambdaStat->SetPersistence();
//fRun->AddTask(lambdaStat);




  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,0);
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
