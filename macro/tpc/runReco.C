{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // ----  Load libraries   -------------------------------------------------
  //gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  //basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TString basedir = gSystem->Getenv("VMCWORKDIR");
  
  //Set JOBNAME and JOBDIR
  // -------------------------------------------------------------------

  TString jobdir = "";
  TString jobname="Test";

 
  TString digiDir=(basedir+"/")+jobdir;
 
 
  TString inFile=(digiDir+"/")+jobname;
  inFile+=".raw.root";


  TString mcFile=inFile;
  mcFile.ReplaceAll("raw","mc");

  ///TString inDir=inFile(0,inFile.Last('/')+1);
  // make new subdir
  //TString jobDir=inDir; jobDir+=jobname; jobDir+="/";
  //TString cmd="mkdir ";
  //cmd+=jobDir; 
  //if(gSystem->Exec(cmd)){
  //  std::cout<<"Could not create Job-Directory "<<jobDir
  //     <<". Aborting."<<std::endl;
  //  return;
  // }


  TString outFile = inFile; 
  //outFile.ReplaceAll(inDir,jobDir);
  outFile.ReplaceAll(".raw.root",".reco.root");


  TString paramIn = inFile;
  paramIn.ReplaceAll(".raw.root",".param.root");
  TString paramOut = outFile;
  paramOut.ReplaceAll(".reco.root",".param.root");


  /*TString mcDir = inDir;
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

  //return;
  
  
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
  
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
  std::cout<<"\nGEANE initialised"<<std::endl;
  // ------------------------------------------------------------------------

   

  // -----  Parameter database   --------------------------------------------
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(paramIn.Data());
  
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString tpcDigiFile = gSystem->Getenv("VMCWORKDIR");
  tpcDigiFile += "/tpc/tpc.par";
  parInput2->open(tpcDigiFile.Data(),"in");

  rtdb->setFirstInput(parInput2); //root file IO tends to fail, use ASCII first
  rtdb->setSecondInput(parInput1);

  rtdb->Print();

  
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

  PndTpcLaserCorrectionTask* laser = new PndTpcLaserCorrectionTask();
  TString laserfile=basedir+"tpc/laser.new.reco.root";
  laser->SetLaserRecoFile(laserfile);
  laser->SetPersistence(true);
  fRun->AddTask(laser);

  
  PndTpcIdealTrackingTask* tpcIPR = new PndTpcIdealTrackingTask();
  tpcIPR->useGeane(true);
  tpcIPR->useDistSorting(true);
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
  kalman->SetNumIterations(3); // number of fitting iterations (back and forth)
  fRun->AddTask(kalman);


  TrackFitStatTask* fitstat=new TrackFitStatTask();
  fitstat->SetPersistence();
  fitstat->SetMCPCut(10); // in sigma dp/p
  fitstat->SetMCCuts(0.05, // pmin
	             10., // pmax
		     -TMath::Pi(),   // thetamin 5deg
		     TMath::Pi(),  // thetamax
		     5); // nPndTpcPoints
  //fitstat->SetPdgSelection(321);
  //fitstat->DoResiduals();
  fRun->AddTask(fitstat);

  

  // PndTpcTrackVisTask* trkVis = new PndTpcTrackVisTask();
  // trkVis->SetTrackBranchName("TrackPreFit");
  //trkVis->drawFits(true);
  // fRun->AddTask(trkVis);
  
  //LambdaSelector* lambdaSel = new LambdaSelector();
  //lambdaSel->SetTrackBranchName("TrackPreFit");
  //lambdaSel->SetPersistence();
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

  //tpcRMC->WriteHistograms();
  //  tpcSPR->WriteHistograms("RecoHistos.root");
  //kalman->WriteHistograms("RecoHistos.root");
  //fitstat->WriteHistograms("RecoHistos.root");
  //dEdx->WriteHistograms("RecoHistos.root");

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
