{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

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
gSystem->Load("libtrackrep");
  gSystem->Load("librecotasks");
  
  
  TString PANDAMC=gSystem->Getenv("PANDAMC");

  // Input file (RAW events)
  TString inFile="../data/Pion40_0/DigiSlice/evtmix13/test1.raw.root";
  TString jobname="reco1";

  TString mcFile="../data/Pion40_0/test1.mc.root";
  
  inFile.ReplaceAll("$PANDAMC",PANDAMC);

  TString inDir=inFile(0,inFile.Last('/')+1);
  // make new subdir
  TString jobDir=inDir; jobDir+=jobname; jobDir+="/";
  TString cmd="mkdir ";
  cmd+=jobDir; 
  if(gSystem->Exec(cmd)){
    std::cout<<"Could not create Job-Directory "<<jobDir
	     <<". Aborting."<<std::endl;
    return;
  }
  
  TString outFile = inFile; 
  outFile.ReplaceAll(inDir,jobDir);
  outFile.ReplaceAll(".raw.root",".reco.root");

  TString paramIn = inFile;
  paramIn.ReplaceAll(".raw.root",".param.root");
  TString paramOut = outFile;
  paramOut.ReplaceAll(".reco.root",".param.root");

/*
  TString mcDir = inDir;
  mcDir=mcDir(0,mcDir.Last('/')); // remove last /
  mcDir=mcDir(0,mcDir.Last('/')+1); // one directory up
  TString mcFile= inFile;
  mcFile.ReplaceAll(inDir,mcDir);
  mcFile.ReplaceAll(".raw.root",".mc.root");
  */
  
  std::cout<<"Input: "<<inFile<<std::endl;
  std::cout<<"Output: "<<outFile<<std::endl;
  std::cout<<"MCFile: "<<mcFile<<std::endl;
  std::cout<<"ParamIn: "<<paramIn<<std::endl;
  std::cout<<"ParamOut: "<<paramOut<<std::endl;


  
  // ---  Now choose concrete engines for the different tasks   -------------
  // ------------------------------------------------------------------------


  // In general, the following parts need not be touched
  // ========================================================================




  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  inFile.ReplaceAll("$PANDAMC",PANDAMC);
  

  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  mcFile.ReplaceAll("$PANDAMC","/afs/e18/data/panda/MC");
  fRun->AddFriend(mcFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(paramIn.Data());
  rtdb->setFirstInput(parInput1);
  
  rtdb->print();

  FairParRootFileIo* parOutput1 = new FairParRootFileIo(kTRUE);
  parOutput1->open(paramOut.Data());
  rtdb->setOutput(parOutput1);
  rtdb->saveOutput();

  fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  


    // -----    Reco Sequence  --------------------------------------------
  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  tpcCF->SetMode(1); // individual timeslice
  tpcCF->SetPersistence();
  tpcCF->timeslice(20); // = 4 sample times = 100ns @ 40MHz
//tpcCF->SetTrivialClustering();
  fRun->AddTask(tpcCF);

  PndTpcRiemannMCTask* tpcRMC = new PndTpcRiemannMCTask();
  tpcRMC->SetBkgFileName("../data/DPM/test1.mc.root");
  tpcRMC->SetPersistence();
  fRun->AddTask(tpcRMC);


  PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  tpcSPR->SetTrkFinderParameters(2.,// proxcut
			       0.02, // proxcut on rieman sphere
			       2.E-3, // planecut
			       4.0, // szcut
			       4); // minnumhits for fit
  tpcSPR->SetPersistence();
// fRun->AddTask(tpcSPR);

  KalmanTask* kalman =new KalmanTask();
  kalman->SetPersistence();
//fRun->AddTask(kalman);


  TrackFitStatTask* fitstat=new TrackFitStatTask();
  fitstat->SetPersistence();
  fitstat->SetMCPCut(3); // in sigma dp/p
  fitstat->SetMCCuts(0.05, // pmin
	             10., // pmax
		     -TMath::Pi(),   // thetamin 5deg
		     TMath::Pi(),  // thetamax
		     20); // nPndTpcPoints
//fitstat->SetPdgSelection(321);
//fitstat->DoResiduals();
//fRun->AddTask(fitstat);

  
  PndTpcRecoDEdxTask* dEdx=new PndTpcRecoDEdxTask();
  dEdx->SetPersistence();
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
  fRun->Run(0);
  // ------------------------------------------------------------------------



  // -----   Finish   -------------------------------------------------------

  tpcRMC->WriteHistograms();
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
