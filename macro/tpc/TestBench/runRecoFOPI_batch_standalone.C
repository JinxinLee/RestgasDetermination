void runRecoFOPI_batch_standalone(TString filename, TString outpath, 
				  unsigned int smoothing = 0) 
{
  
// ========================================================================
// Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  TStopwatch timer;
  timer.Start();
  
  unsigned int nEvents = 0;

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
  if(outName.Contains("repaired."))
    outName.ReplaceAll(".lmd_decoded_repaired.root",".reco.root");
  if(outName.Contains("decoded."))
    outName.ReplaceAll(".lmd_decoded.root",".reco.root");
  if(smoothing)
    outName.ReplaceAll("reco.root", "smoothed_reco.root");
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
  if(nEvents==0) nEvents = ((TTree*)testFile.Get("tpcEvent"))->GetEntries();
  std::cout<<"Found "<<nEvents<<" events in input data file"<<std::endl;
  
  
  //--------------------SET UP TASKS ------------------------------

  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
    
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


  bool SimpleClustering = true;
  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  tpcCF->SetDigiPersistence(); // keep Digis refs in clusters
  tpcCF->SetPersistence(); // keep Clusters
  tpcCF->timeslice(6); //in samples
  tpcCF->SetSingleDigiClusterAmpCut(20);
  tpcCF->SetClusterAmpCut(9.1); // cut on mean digi amplitude
  tpcCF->SetErrorPars(600,300);
  tpcCF->SetSimpleClustering(); // use PndTpcClusterFinderSimple
  fRun->AddTask(tpcCF);
  

  PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  tpcSPR->SetSortingParameters(
        true, // false: sort only according to _sorting (see next argument); true: use internal sorting when adding hits to trackcands
        3,    // -1: no sorting, 0: sort Clusters by X, 1: Y, 2: Z, 3: R, 4: distance to origin
        0.); // z-position of interaction point (for sorting 4)
  tpcSPR->SetTrkFinderParameters(
        1.9,  // proximity cut in 3D [cm]
        0.4,  // helix cut [cm]
        5);   // minimum hits for helix-fit
  tpcSPR->SetMergeTracks();
  tpcSPR->SetTrkMergerParameters(
        2.5,  // proximity cut [cm]
        0.1,  // dip cut [rad]
        0.6,  // helix cut [cm]
        0.025);// plane cut (RMS)
  tpcSPR->SetRiemannScale(); // sets riemannscale for the prototype;
  tpcSPR->SetPersistence();
  //tpcSPR->useGeane(); // uses RKTrackrep and GeaneTrackrep
  //tpcSPR->WriteHistograms(PROutFile);
  //fRun->AddTask(tpcSPR);

  PndTpcSLPatternRecoTask* tpcSLPR = new PndTpcSLPatternRecoTask();
  tpcSLPR->SetPersistence(true);
  tpcSLPR->SetStoreHistograms(PROutFile);
  tpcSLPR->SetClusterAmpCut(20.);
  tpcSLPR->SetCutTracksParallelZ(5);
  //tpcSLPR->SetXSorting(true);
  double parMins[4] = {-TMath::Pi(),0.,-TMath::Pi(),0.};
  double parMaxs[4] = {TMath::Pi(),10.,TMath::Pi(),20.};
  tpcSLPR->SetParameterSpace(parMins, parMaxs);
  tpcSLPR->SetDepth(8);
  tpcSLPR->SetThresh(15);
  tpcSLPR->SetMinCandHits(15);
  //tpcSLPR->SetClusterBranchName("PndTpcCluster_cut");
  tpcSLPR->SetAbsMomentum(1000);
  fRun->AddTask(tpcSLPR);


  KalmanTask* kalman =new KalmanTask();
  kalman->SetPersistence();
  //kalman->SetClusterBranchName("PndTpcCluster_cut");
  kalman->SetNumIterations(3); // number of fitting iterations (back and forth)
  fRun->AddTask(kalman);


  PndTpcResidualTask* Res = new PndTpcResidualTask();
  Res->SetPersistence();
  //Res->SetNumberOfTrackReps(2); // set to 2 if you use GeaneTrackrep (tpcSPR->useGeane();)
  //SLres->SetClusterBranchName("PndTpcCluster_cut");
  fRun->AddTask(Res);
  
  PndTpcSLResidualTask* SLres = new PndTpcSLResidualTask();
  SLres->SetPersistence();
  //SLres->SetClusterBranchName("PndTpcCluster_cut");
  SLres->SetSecondarySuppression(true);
  fRun->AddTask(SLres);
  

  // -----   Intialise and run   --------------------------------------------
 
  fRun->Init();
  fRun->Run(0,nEvents);

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  std::cout<<"OutputFile: "<<outFile<<std::endl;
  testFile.Close();
}
  
