void runRecoFOPI_batch(TString filename, TString outpath) 
{
  
// ========================================================================
// Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;
  
  
  // ----  Load libraries   -------------------------------------------------
  //gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  //basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  //gROOT->Macro("tpc/TestChamber/macro/christian_style.C");
  rootlogon();
  
  TString basedir = gSystem->Getenv("VMCWORKDIR");
  
  //Set JOBNAME and JOBDIR
  // -------------------------------------------------------------------

  TString jobdir = outpath; 
  //Fuck TString.
  std::string jobname(filename.Data());
  int last = jobname.rfind("/");
  if(last>0)
    jobname = jobname.substr(last+1,jobname.size()+1);  
    
  TString outName(jobname);
  outName.ReplaceAll(".lmd_decoded_repaired.root",".reco.root");
  TString outFile = outpath+"/";
  outFile += outName; 
  
  TString inFile=jobdir;
  inFile+="/dummy/dummy.raw.root";
  //TString inFile="/nfs/hicran/data/tpc/fopi/2010/reconstructed/dummy/dummy.raw.root";
  
  TString mcFile=inFile;
  mcFile.ReplaceAll(".raw",".mc");
  
  TString PROutFile = outFile;
  PROutFile.ReplaceAll(".reco.root",".patternReco.root");
  TFile test(PROutFile, "recreate");
  if(!test.IsZombie()) { //delete file
    gSystem->Setenv("PROUTFILENAME", PROutFile.Data());
    gROOT->ProcessLine(".! rm $PROUTFILENAME");
    gSystem->Unsetenv("PROUTFILENAME");
  }
  
  
  TString paramIn = inFile;
  paramIn.ReplaceAll(".raw.root",".param.root");
  TString paramOut = outFile;
  paramOut.ReplaceAll(".reco.root",".param.root");
  
  std::cout<<"Input: "<<inFile<<std::endl;
  std::cout<<"MCFile: "<<mcFile<<std::endl;
  std::cout<<"ParamIn: "<<paramIn<<std::endl;
  std::cout<<"ParamOut: "<<paramOut<<std::endl;

  std::cout<<"Output: "<<outFile<<std::endl;  
  std::cout<<"PROutput: "<<PROutFile<<std::endl;


  
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
  tpcDigiFile += "/tpc/TestBench/tpc.TBtestChamber.par";
  parInput2->open(tpcDigiFile.Data(),"in");
  
  rtdb->setFirstInput(parInput2); //root file IO tends to fail, use ASCII first
  rtdb->setSecondInput(parInput1);

  rtdb->Print();

  
  // ------------------------------------------------------------------------
  QAPlotCollection* qa=new QAPlotCollection("TpcDigiQAPlots");  


  // -----    Reco Sequence  --------------------------------------------
  
  PndTpcDataReaderTask* read = new PndTpcDataReaderTask();
  read->SetPersistence();
  read->SetDatafile(filename);
  read->SetClusterBranchName("PndTpcSample");
  //read->SetCutSmallPad();
  //read->SetMinSamples(1000);
  fRun->AddTask(read);
  
  //PndTpcTCcrossTalkTask* CT = new PndTpcTCcrossTalkTask();
  //CT->SetPersistence();
  //fRun->AddTask(CT);
  
  
  PndTpcPSATask* tpsa = new  PndTpcPSATask();
  //tpsa->SetPersistence();
  tpsa->SetSampleBranchName("PndTpcSample");
  fRun->AddTask(tpsa);

  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  //tpcCF->SetDataMode(true); //prevents usage of FairLinks
  tpcCF->SetMode(2); // 0 - global time bins;  1 - individual time bins for each sector;  2 - each pad gets its time window - actually we search for gaps on a pad;
  tpcCF->SetDataMode(true);
  tpcCF->SetPersistence();
  tpcCF->SetDigiBranchName("PndTpcDigi");
  tpcCF->timeslice(20); //in samples
  tpcCF->SetDiffFactor(1.);
  tpcCF->SetSingleDigiClusterAmpCut(15);
  tpcCF->SetErrorPars(600,300);
  //tpcCF->SetTrivialClustering();
  //tpcCF->SetSimpleClustering(); // use PndTpcClusterFinderSimple
  fRun->AddTask(tpcCF);

  //actually MODIFIES existing clusters, does NOT create a new branch
  PndTpcClusterCorrectionTask* tpcCC = new PndTpcClusterCorrectionTask();
  double pars[6] = {-0.115634, -1.85970, 11.5997,
        	    -24.8201, 24.9152,-9.56801};
  tpcCC->SetParameters(pars);
  // fRun->AddTask(tpcCC);


  //PndTpcCTapplyTask* CTapply = new PndTpcCTapplyTask();
  //CTapply->SetPersistence();
  //fRun->AddTask(CTapply);


  PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  tpcSPR->SetSortingParameters(
                   true, // false: sort only according to _sorting (see next argument); true: use internal sorting when adding hits to trackcands
                   3,    // -1: no sorting, 0: sort Clusters by X, 1: Y, 2: Z, 3: R, 4: distance to origin
                   30.); // z-position of interaction point (for sorting 4)
  tpcSPR->SetTrkFinderParameters(
                   1.5,  // proximity cut in 3D
                   0.025, // proximity cut on rieman sphere
                   0.02, // distance to plane cut
                   2.5,  // szcut
                   8);   // minimum hits for plane & sz-fit
  tpcSPR->SetMergeTracks();
  tpcSPR->SetTrkMergerParameters(
                   2.5,  // proximity cut
                   2.5,  // sz cut
                   8E-3);// plane cut (RMS)
  tpcSPR->SetPersistence();
  tpcSPR->SetStoreHistograms(PROutFile);
  //tpcSPR->WriteHistograms(PROutFile);
  fRun->AddTask(tpcSPR);



  PndTpcSLPatternRecoTask* tpcSLPR = new PndTpcSLPatternRecoTask();
  tpcSLPR->SetPersistence(true);
  tpcSLPR->SetStoreHistograms(PROutFile);
  tpcSLPR->SetClusterAmpCut(30.);
  tpcSLPR->SetCutTracksParallelZ(5);
  tpcSLPR->SetXSorting(true);
  double parMins[4] = {-TMath::Pi(),0.,-TMath::Pi(),0.};
  double parMaxs[4] = {TMath::Pi(),10.,TMath::Pi(),20.};
  tpcSLPR->SetParameterSpace(parMins, parMaxs);
  tpcSLPR->SetDepth(8);
  tpcSLPR->SetThresh(22);
  tpcSLPR->SetMinCandHits(22);
  //tpcSLPR->SetClusterBranchName("PndTpcCluster_cut");
  tpcSLPR->SetAbsMomentum(1000);
  //fRun->AddTask(tpcSLPR);


  //PndTpcTCtrackFit* tf = new PndTpcTCtrackFit();
  //tf->SetPersistence();
  //tf->SetDraw();
  //fRun->AddTask(tf);


  KalmanTask* kalman =new KalmanTask();
  kalman->SetPersistence();
  //kalman->SetClusterBranchName("PndTpcCluster_cut");
  kalman->SetNumIterations(3); // number of fitting iterations (back and forth)
  //fRun->AddTask(kalman);


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

  PndTpcSLResidualTask* SLres = new PndTpcSLResidualTask();
  SLres->SetPersistence();
  //SLres->SetClusterBranchName("PndTpcCluster_cut");
  SLres->SetSecondarySuppression(false);
  fRun->AddTask(SLres);
  
  

  // -----   Intialise and run   --------------------------------------------
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
