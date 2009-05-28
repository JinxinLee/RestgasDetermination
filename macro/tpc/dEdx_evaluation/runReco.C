{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;
gSystem->Load("libGeane");

  // Input file (RAW events)
  TString inDir=gSystem->Getenv("VMCWORKDIR");
  inDir=inDir+"/../workdir/";
  
  TString jobname="dEdxKaonMono";
  TString inFile=inDir+jobname;
  
  inFile+=".raw.root";
  
  TString mcFile=inFile;
mcFile.ReplaceAll("raw.root","mc.root");
  
  
  TString outFile = inFile;
  outFile.ReplaceAll(".raw.root",".reco.root");

  TString paramIn = inFile;
  paramIn.ReplaceAll(".raw.root",".param.root");
  TString paramOut = outFile;
  paramOut.ReplaceAll(".reco.root",".param.root");

  
  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  //inFile.ReplaceAll("$PANDAMC",PANDAMC);
  

  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);

  fRun->AddFriend(mcFile);
  
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------

  //prepare GEANE
  FairGeane *Geane = new FairGeane(mcFile);
  

  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(paramIn.Data());
  rtdb->setFirstInput(parInput1);
  
  rtdb->Print();

  FairParRootFileIo* parOutput1 = new FairParRootFileIo(kTRUE);
  parOutput1->open(paramOut.Data());
  rtdb->setOutput(parOutput1);
  rtdb->saveOutput();


  fRun->LoadGeometry();


  std::cout<<"setting GEANE field to "<<fRun->GetField();
  // Set the field(if any) to Geane
  Geane->SetField(fRun->GetField());
  // ------------------------------------------------------------------------
  


    // -----    Reco Sequence  --------------------------------------------
  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  tpcCF->SetMode(1); // individual timeslice
  tpcCF->SetPersistence();
  tpcCF->timeslice(20); // = 4 sample times = 100ns @ 40MHz
//tpcCF->SetTrivialClustering();
  fRun->AddTask(tpcCF);

//  PndTpcRiemannMCTask* tpcRMC = new PndTpcRiemannMCTask();
//  tpcRMC->SetBkgFileName("../data/DPM/test1.mc.root");
// fRun->AddTask(tpcRMC);

  PndTpcIdealTrackingTask* tpcIPR = new PndTpcIdealTrackingTask();
  tpcIPR->useGeane();
  tpcIPR->useDistSorting(kTRUE);
  fRun->AddTask(tpcIPR);
  tpcIPR->SetPersistence();



//   PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
//   tpcSPR->SetTrkFinderParameters(2.,// proxcut
// 				 0.02, // proxcut on rieman sphere
// 				 2.E-3, // planecut
// 				 4.0, // szcut
// 				 4); // minnumhits for fit
//   tpcSPR->SetPersistence();
//   tpcSPR->useGeane();
//   fRun->AddTask(tpcSPR);
  
  KalmanTask* kalman =new KalmanTask();
  kalman->SetPersistence();
  kalman->SetLazy(false); // be strict with errors that occur
  kalman->SetNumIterations(3); // number of fitting iterations (back and forth)
  fRun->AddTask(kalman);

  PndTpcdEdxTask* dEdx = new PndTpcdEdxTask();
  dEdx->SetPersistence();
  fRun->AddTask(dEdx);


  TrackFitStatTask* fitstat=new TrackFitStatTask();
  fitstat->SetPersistence();
  fitstat->SetMCPCut(10); // in sigma dp/p
  fitstat->SetMCCuts(0.05, // pmin
	             10., // pmax
		     -TMath::Pi(),   // thetamin 5deg
		     TMath::Pi(),  // thetamax
		     5); // nPndTpcPoints
//fitstat->SetPdgSelection(321);
//  fitstat->DoResiduals();
  fRun->AddTask(fitstat);

  
//PndTpcRecoDEdxTask* dEdx=new PndTpcRecoDEdxTask();
// dEdx->SetPersistence();
//fRun->AddTask(dEdx);


  PndTpcTrackVisTask* trkVis = new PndTpcTrackVisTask();
  trkVis->SetTrackBranchName("TrackPreFit");
  trkVis->drawFits(true);
// fRun->AddTask(trkVis);

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
  Geane->SetField(fRun->GetField());
  fRun->Run(0,0);
  // ------------------------------------------------------------------------



  // -----   Finish   -------------------------------------------------------

  //tpcRMC->WriteHistograms();
//  tpcSPR->WriteHistograms("RecoHistos.root");
//kalman->WriteHistograms("RecoHistos.root");
//fitstat->WriteHistograms("RecoHistos.root");
dEdx->WriteHistograms("RecoHistos.root");

  DebugLogger::Instance()->WriteFiles();

//delete tpcSplitter;
  rtdb->saveOutput();
  rtdb->print();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << paramOut << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
