void runRecoFOPI(){
//Data analysis framework for the test bench tpc data.

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;


  //Load libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TString basedir = gSystem->Getenv("VMCWORKDIR");
  
  //Set JOBNAME and JOBDIR
  TString jobdir = "TDR_Plots";
  TString jobname="1GeV_Protons";


  TString digiDir=jobdir;
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


  TString paramIn1 = inFile;
  TString paramIn2 = inFile;
  paramIn1.ReplaceAll(".raw.root",".raw.param.root");
  paramIn2.ReplaceAll(".raw.root",".mc.param.root");

  TString paramOut = outFile;
  paramOut.ReplaceAll(".reco.root",".reco.param.root");


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
  std::cout<<"ParamIn1: "<<paramIn1<<std::endl;
  std::cout<<"ParamIn2: "<<paramIn2<<std::endl;
  std::cout<<"ParamOut: "<<paramOut<<std::endl;



  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

gStyle->SetPalette(1);
  
  // -----   Digitization run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);

  fRun->AddFriend(mcFile);
  fRun->SetOutputFile(outFile);

  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo(kTRUE);
  parInput1->open(paramIn1.Data());
 FairParRootFileIo* parInput2 = new FairParRootFileIo(kTRUE);
  parInput2->open(paramIn2.Data());

  FairParAsciiFileIo* parInput3 = new FairParAsciiFileIo();
  TString tpcDigiFile = gSystem->Getenv("VMCWORKDIR");
  tpcDigiFile += "/tpc/TestBench/tpc.TBtestChamber.par";
  //tpcDigiFile += "/tpc/tpc.par";
  parInput3->open(tpcDigiFile.Data(),"in");


  rtdb->setFirstInput(parInput1); //root file IO tends to fail, use ASCII first
  rtdb->setSecondInput(parInput2);

  rtdb->Print();


  // ------------------------------------------------------------------------

  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
  std::cout<<"\nGEANE initialised"<<std::endl;


    // -----    Reco Sequence  --------------------------------------------

  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  bool SimpleClustering = true;
  //tpcCF->SetDataMode(true); //prevents usage of FairLinks
  tpcCF->SetDigiPersistence(); // keep reference to digis in clusters
  tpcCF->SetPersistence(); // keep Clusters
  tpcCF->timeslice(5); //in samples
  tpcCF->SetThreshold(1);
  tpcCF->SetSingleDigiClusterAmpCut(15);
  tpcCF->SetClusterAmpCut(9.1);
  if(!SimpleClustering) {
    tpcCF->SetMode(2); // 0 - global time bins;
                       // 1 - individual time bins for each sector;
                       // 2 - each pad gets its time window - actually we search for gaps on a pad;
    tpcCF->SetDiffFactor(1.);
    tpcCF->SetClusterTimeCut(5.);
  }
  tpcCF->SetErrorPars(600.,500.);
  if(SimpleClustering) tpcCF->SetSimpleClustering(); // use PndTpcClusterFinderSimple
  fRun->AddTask(tpcCF);


  PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  tpcSPR->SetSortingParameters(
                   true, // false: sort only according to _sorting (see next argument); true: use internal sorting when adding hits to trackcands
                   3,    // -1: no sorting, 0: sort Clusters by X, 1: Y, 2: Z, 3: R, 4: distance to origin
                   0.); // z-position of interaction point (for sorting 4)
  tpcSPR->SetTrkFinderParameters(
                   1.9,  // proximity cut in 3D
                   0.1, // proximity cut on rieman sphere
                   0.04, // distance to plane cut
                   0.2,  // szcut
                   4);   // minimum hits for plane & sz-fit
  tpcSPR->SetMergeTracks();
  tpcSPR->SetTrkMergerParameters(
                   2.2,  // proximity cut
                   0.33,  // sz cut
                   0.025);// plane cut (RMS)
  tpcSPR->SetRiemannScale(); // sets riemannscale for the prototype;
  tpcSPR->SetPersistence();
  //tpcSPR->SetStoreHistograms(PROutFile); //
  //tpcSPR->WriteHistograms(PROutFile);
  fRun->AddTask(tpcSPR);


  KalmanTask* kalman =new KalmanTask();
  kalman->SetPersistence();
  //kalman->SetClusterBranchName("PndTpcCluster_cut");
  kalman->SetNumIterations(3); // number of fitting iterations (back and forth)
  fRun->AddTask(kalman);


  TrackFitStatTask* fitstat=new TrackFitStatTask();
  fitstat->SetPersistence();
  //  fitstat->SetMCPCut(0); // in sigma dp/p
  fitstat->SetMCCuts(0.005,       // pmin
                     10.,         // pmax
                     -TMath::Pi(),// thetamin
                     TMath::Pi(), // thetamax
                     5);          // nPndTpcPoints
  fitstat->SetPdgSelection(11);   // 321
  fitstat->DoResiduals();
  fRun->AddTask(fitstat);


  PndTpcResidualTask* Res = new PndTpcResidualTask();
  Res->SetPersistence();
  //SLres->SetClusterBranchName("PndTpcCluster_cut");
  //fRun->AddTask(Res);
  


  // -----   Initialise and run   --------------------------------------------
  fRun->Init();
  
  fRun->Run(0,0);
  // ------------------------------------------------------------------------

  FairRootManager::Instance()->GetOutFile()->mkdir("QAPlots");
  FairRootManager::Instance()->GetOutFile()->cd("QAPlots");
  qa->Write();


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
