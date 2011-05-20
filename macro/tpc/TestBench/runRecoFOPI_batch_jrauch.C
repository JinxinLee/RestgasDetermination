void runRecoFOPI_batch_jrauch(TString filename, TString outpath) 
{
  
// ========================================================================
// Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  TStopwatch timer;
  timer.Start();
  
  // Load basic libraries in rootlogon
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TString basedir = gSystem->Getenv("VMCWORKDIR");
  FairRunAna* fRun = new FairRunAna();
  //FairRunSim* fSim = new FairRunSim();
  
  TString jobdir = outpath; 
  std::string jobname(filename.Data());
  int last = jobname.rfind("/");
  if(last>0)
    jobname = jobname.substr(last+1,jobname.size()+1);  
  
  TString outName(jobname);
  if(outName.Contains("repaired"))
  outName.ReplaceAll(".lmd_decoded_repaired.root",".reco.root");
  if(outName.Contains("decoded"))
    outName.ReplaceAll(".lmd_decoded.root",".reco.root");
  if(outName.Contains("raw"))
    outName.ReplaceAll(".raw.root",".reco.root");
  TString outFile = outpath+"/";
  outFile += outName; 
    
  TString PROutFile = outFile;
  PROutFile.ReplaceAll(".reco.root",".patternReco.root");
  TFile test(PROutFile, "recreate");
  if(!test.IsZombie()) { //delete file
    gSystem->Setenv("PROUTFILENAME", PROutFile.Data());
    gROOT->ProcessLine(".! rm $PROUTFILENAME");
    gSystem->Unsetenv("PROUTFILENAME");
  }
  
  fRun->SetOutputFile(outFile);
  
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParAsciiFileIo* parInput1 = new FairParAsciiFileIo();
  TString tpcDigiFile = basedir;
  tpcDigiFile += "/tpc/TestBench/tpc.TBtestChamber.par";
  parInput1->open(tpcDigiFile.Data(),"in");
  
  rtdb->setFirstInput(parInput1);
  
  rtdb->Print();
  
  TString geoFile = basedir;
  geoFile+="/tpc/TestBench/FOPIGeo.root";
  fRun->SetGeomFile(geoFile);
  PndConstField *fMagField=new PndConstField();
  fMagField->SetField(0., 0. , 6. ); // values are in kG
  // values are in cm
  fMagField->SetFieldRegion(-50, 50,-50, 50, -2000, 2000);
      
  fRun->SetField(fMagField);
  
  
  //extract number of entries in external data tree
  TFile testFile(filename);
  unsigned int nEvents;
  nEvents = ((TTree*)testFile.Get("tpcEvent"))->GetEntries();
  std::cout<<"Found "<<nEvents<<" events in input data file"<<std::endl;


  //--------------------SET UP TASKS ------------------------------

  PndTpcDataReaderTask* read = new PndTpcDataReaderTask();
  read->SetPersistence();
  read->SetDatafile(filename);
  read->SetClusterBranchName("PndTpcSample");
  //read->SetCutSmallPad();
  //read->SetMinSamples(1000);
  fRun->AddTask(read);
  
  
  PndTpcPSATask* tpsa = new  PndTpcPSATask();
  tpsa->SetPersistence();
  tpsa->SetSampleBranchName("PndTpcSample"); // Input of PSA
  fRun->AddTask(tpsa);


  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  tpcCF->SetDigiPersistence(); // keep reference to digis in clusters
  tpcCF->SetPersistence(); // keep Clusters
  tpcCF->timeslice(4); //in samples
  tpcCF->SetThreshold(1);
  tpcCF->SetSingleDigiClusterAmpCut(15);
  tpcCF->SetClusterAmpCut(9.1); // cut on mean digi amplitude
  tpcCF->SetErrorPars(600.,400.);
  tpcCF->SetSimpleClustering(); // use PndTpcClusterFinderSimple
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
  fitstat->SetMCCuts(0.005, // pmin
	             10., // pmax
		     -TMath::Pi(),   // thetamin 5deg
		     TMath::Pi(),  // thetamax
		     5); // nPndTpcPoints
  fitstat->SetPdgSelection(11);//321
  //fitstat->DoResiduals();
  //fRun->AddTask(fitstat);


  PndTpcResidualTask* Res = new PndTpcResidualTask();
  Res->SetPersistence();
  //SLres->SetClusterBranchName("PndTpcCluster_cut");
  fRun->AddTask(Res);
  
  

  // -----   Intialise and run   --------------------------------------------
 
 
  fRun->Init();
  fRun->Run(0,0);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  std::cout<<"OutputFile: "<<outFile<<std::endl;

}
  
