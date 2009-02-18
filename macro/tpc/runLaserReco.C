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
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libgenfit");
  gSystem->Load("libtpc");
  gSystem->Load("libtrackrep");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");
  
  
  // Input file (RAW events)


  TString inFile="eumel.laser.raw.root";
//  TString jobname="reco2";

//  jobdb* db=new jobdb("$PANDAMC/FAIRRoot/JOBDB/pandaJobDB.root");
//  int prodid=db->findProducerJobByFile(inFile);
//  jobob injob=db->getJob(prodid);

//  TString inDir=inFile(0,inFile.Last('/')+1);
  // make new subdir
//   TString jobDir=inDir; jobDir+=jobname; jobDir+="/";
//   TString cmd="mkdir ";
//   cmd+=jobDir; 
//   if(gSystem->Exec(cmd)){
//     std::cout<<"Could not create Job-Directory "<<jobDir
// 	     <<". Aborting."<<std::endl;
//     return;
//   }
 
   TString outFile = "eumel.laser.reco.root"; 
//   outFile.ReplaceAll(inDir,jobDir);
//   outFile.ReplaceAll(".raw.root",".reco.root");

   TString paramIn = "eumel.param.root";
//   TString paramOut = outFile;
//   paramOut.ReplaceAll(".reco.root",".param.root");

//   jobob mcjob=db->findRootJob(injob);
   TString mcFile= "eumel.mc.root";

std::cout<<"Input: "<<inFile<<std::endl;
std::cout<<"Output: "<<outFile<<std::endl;
std::cout<<"MCFile: "<<mcFile<<std::endl;
std::cout<<"ParamIn: "<<paramIn<<std::endl;
//std::cout<<"ParamOut: "<<paramOut<<std::endl;


  // ------------------------------------------------------------------------
  // ---  Register Job in database   -------------
  // ------------------------------------------------------------------------

  // write jobdb
//   jobob* job=new jobob(jobname,inFile,outFile,paramOut,jobob::kReco);
//   int id=db->addJob(*job);
//   std::cout<<"Added job with ID="<<id<<std::endl;
//   delete db;
//   delete job;

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
  parInput1->open(paramIn.Data());

  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  TString tpcDigiFile = gSystem->Getenv("VMCWORKDIR");
  tpcDigiFile += "/tpc/tpc.par";
  parInput2->open(tpcDigiFile.Data(),"in");


  rtdb->setFirstInput(parInput2);
  rtdb->setSecondInput(parInput1);


  rtdb->print();

//   FairParRootFileIo* parOutput1 = new FairParRootFileIo(kTRUE);
//   parOutput1->open(paramOut.Data());
//   rtdb->setOutput(parOutput1);
//   rtdb->saveOutput();

  fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  


    // -----    Reco Sequence  --------------------------------------------
  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  
  tpcCF->SetPersistence();
  tpcCF->timeslice(20); // = 4 sample times = 100ns @ 40MHz
//tpcCF->SetTrivialClustering();
  fRun->AddTask(tpcCF);

  PndTpcLaserFitTask* laserFit = new PndTpcLaserFitTask();
  laserFit->buildLaserList("LaserGridTest.dat", true);
  laserFit->setXYresiduals(true);
  laserFit->SetPersistence();
  fRun->AddTask(laserFit);
 


//   KalmanTask* kalman =new KalmanTask();
//   fRun->AddTask(kalman);

//   TrackFitStatTask* fitstat=new TrackFitStatTask();
//   fitstat->SetPersistence();
// fitstat->SetMCPCut(3); // in sigma dp/p
//   fitstat->SetMCCuts(0.05, // pmin
// 	             10., // pmax
// 		     -TMath::Pi(),   // thetamin 5deg
// 		     TMath::Pi(),  // thetamax
// 		     20); // nPndTpcPoints
// //fitstat->SetPdgSelection(321);
//   fitstat->DoResiduals();
// //fRun->AddTask(fitstat);

//   PndTpcTrackVisTask* trkVis = new PndTpcTrackVisTask();
//   trkVis->SetTrackBranchName("TrackPreFit");
//   trkVis->drawFits(true);
// fRun->AddTask(trkVis);

//   LambdaSelector* lambdaSel = new LambdaSelector();
//   lambdaSel->SetTrackBranchName("TrackPreFit");
//   lambdaSel->SetPersistence();
// // fRun->AddTask(lambdaSel);

//   V0Selector* V0Sel = new V0Selector();
//   V0Sel->SetTrackBranchName("TrackPreFit");
// //V0Sel->SetPositivePartMass(0.938272);
// //V0Sel->SetNegativePartMass(0.13957);
// //V0Sel->SetPositivePartMass(511.E-6);
// //V0Sel->SetNegativePartMass(511.E-6);

//   V0Sel->SetPersistence();
// //  fRun->AddTask(V0Sel);

// V0Selector* V0Sel2 = new V0Selector();
//   V0Sel2->SetTrackBranchName("TrackPreFit");
// //V0Sel2->SetV0BranchName("Lambda");
// //V0Sel->SetPositivePartMass(0.938272);
// //V0Sel->SetNegativePartMass(0.13957);
// //V0Sel2->SetPositivePartMass(0.13957);
// //V0Sel2->SetNegativePartMass(0.13957);

//   V0Sel2->SetPersistence();
// //fRun->AddTask(V0Sel2);



//   LambdaStatTask* lambdaStat = new LambdaStatTask();
//  lambdaStat->SetPersistence();
// //fRun->AddTask(lambdaStat);


  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  fRun->Run(0,0);
  // ------------------------------------------------------------------------



  // -----   Finish   -------------------------------------------------------

//  tpcSPR->WriteHistograms("RecoHistos.root");
//  kalman->WriteHistograms("RecoHistos.root");
//fitstat->WriteHistograms("RecoHistos.root");

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
//  cout << "Parameter file is " << paramOut << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------


}
