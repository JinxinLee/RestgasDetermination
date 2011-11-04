void runRecoFOPI_CDC_batch_standalone(TString filename,
				      TString fopiFilePrefix, 
				      TString outpath="$RECOOUT",
				      unsigned int smoothing = 0,
				      unsigned int nFopiEvents = 0) 
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

  gROOT->SetBatch(); //to ensure running in batch

  TString basedir = gSystem->Getenv("VMCWORKDIR");
  FairRunAna* fRun = new FairRunAna();
  //FairRunSim* fSim = new FairRunSim();

  // Create alignment manager
  PndTpcAlignmentManager * align = PndTpcAlignmentManager::getInstance("/nfs/hicran/project/panda/SIM/fcusanno/pandatest/macro/tpc/TestBench/alignment_test.txt");

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
   
  TString fopiSuffix = outName;
  fopiSuffix.ReplaceAll(".reco.root",".root");
  std::string suffixname(fopiSuffix.Data());
  int suffl = suffixname.rfind("_");
  if(suffl>0)
    suffixname = suffixname.substr(suffl+1,suffixname.size()+1);
  TString fopiFilename(suffixname);
  TString runName = fopiFilename;
  fopiFilename = fopiFilePrefix + fopiFilename;
  //
  std::cout<<"FOPI input file: "<<fopiFilename<<std::endl;
  std::string numrun(runName.Data()); 
  int numl = numrun.rfind(".");
  runName.Remove(numl,numrun.size()+1-numl);
  Int_t runNr = runName.Atoi();
  //

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
  fMagField->SetFieldRegion(-500, 500,-500, 500, -2000, 2000);
      
  fRun->SetField(fMagField);
  
  //extract number of entries in external data tree
  TFile testFile(filename);
  if(nEvents==0) nEvents = ((TTree*)testFile.Get("tpcEvent"))->GetEntries();
  TFile testFile2(fopiFilename);
  if(nFopiEvents==0) nFopiEvents = ((TTree*)testFile2.Get("cdcTree"))->GetEntries();
  
  std::cout<<"Found "<<nEvents<<" events in  input data file"<<std::endl;
  std::cout<<"Found "<<nFopiEvents<<" events in fopi input data file"<<std::endl;
  
  
  //--------------------SET UP TASKS ------------------------------

  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
    
  PndTpcCdcDataReaderTask* read = new PndTpcCdcDataReaderTask();
  read->SetPersistence();
  read->SetRunNr(runNr);
  read->SetDatafile(filename);
  read->SetNevent(nFopiEvents);
  read->SetCdcDataFile(fopiFilename);
  read->SetSampleBranchName("PndTpcSample");
  //read->SetCutSmallPad();
  //read->SetMinSamples(1000);
  fRun->AddTask(read);
  

  PndTpcPSATask* tpsa = new  PndTpcPSATask();
  tpsa->SetPersistence();
  tpsa->SetSampleBranchName("PndTpcSample"); // Input of PSA
  fRun->AddTask(tpsa);


  //bool SimpleClustering = true;
  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  tpcCF->SetDigiPersistence(); // keep Digis refs in clusters
  tpcCF->SetPersistence(); // keep Clusters
  tpcCF->timeslice(6); //in samples
  tpcCF->SetSingleDigiClusterAmpCut(20);
  tpcCF->SetClusterAmpCut(9.1); // cut on mean digi amplitude
  tpcCF->SetErrorPars(600,400);
  tpcCF->SetSimpleClustering(); // use PndTpcClusterFinderSimple
  fRun->AddTask(tpcCF);
  
  //find PndTpcRiemannTracks in the TPC alone
  PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  //tpcSPR->SetPersistence();
  //tpcSPR->SetVerbose(1);
  fRun->AddTask(tpcSPR);

  //build GFTracks from PndTpcRiemannTracks
  PndTpcTrackInitTask* trackInit=new PndTpcTrackInitTask();
  trackInit->SetPersistence();
  //trackInit->SetVerbose(1);
  //trackInit->SetMCPid(); // use ideal particle identification
  trackInit->SetPDG(211);
  trackInit->useGeane(); // uses RKTrackrep and GeaneTrackrep
  trackInit->SetSmoothing(true);
  fRun->AddTask(trackInit);
  
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
  //fRun->AddTask(tpcSLPR);

  KalmanTask* kalman =new KalmanTask();
  kalman->SetPersistence();
  //kalman->SetClusterBranchName("PndTpcCluster_cut");
  kalman->SetNumIterations(3); // number of fitting iterations (back and forth)
  fRun->AddTask(kalman);
  
  PndTpcCdcMatchingTask* tpcCdcMatch = new PndTpcCdcMatchingTask();
  tpcCdcMatch->SetPersistence();
  tpcCdcMatch->SetMaxMatDistance(.4);
  tpcCdcMatch->SetMaxMatPhi(acos(0.));
  tpcCdcMatch->SetMinHitsperLength(0.0);
  fRun->AddTask(tpcCdcMatch);
  /*
  KalmanTask* KalmanComb = new KalmanTask();
  KalmanComb->SetPersistence();
  KalmanComb->SetTrackBranchName("PndTpcCdcPreFit");
  KalmanComb->SetNumIterations(3); // number of fitting iterations
  fRun->AddTask(KalmanComb);
  */
  // PndTpcResidualTask* Res = new PndTpcResidualTask();
  // Res->SetPersistence();
  // //Res->SetNumberOfTrackReps(1); // set to 2 if you use GeaneTrackrep (tpcSPR->useGeane();)
  // //SLres->SetClusterBranchName("PndTpcCluster_cut");
  // fRun->AddTask(Res);
  
  

  // -----   Intialise and run   --------------------------------------------
 
  fRun->Init();
  fRun->Run(0,nFopiEvents); 

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime(); 
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  std::cout<<"OutputFile: "<<outFile<<std::endl;
  testFile.Close();
}
  
