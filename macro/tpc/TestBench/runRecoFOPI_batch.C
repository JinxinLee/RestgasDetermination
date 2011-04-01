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
  inFile+="/dummy/dummy5.raw.root";
   
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
  tpcCF->SetMode(2); 
  tpcCF->SetDataMode(true);
  tpcCF->SetPersistence();
  tpcCF->SetDigiBranchName("PndTpcDigi");
  //tpcCF->SetClusterBranchName("PndTpcCluster_raw");
  tpcCF->timeslice(5); //in samples
  tpcCF->SetDiffFactor(1.);
  tpcCF->SetSingleDigiClusterAmpCut(15);
  tpcCF->SetErrorPars(600,300);
  //tpcCF->SetTrivialClustering();
  fRun->AddTask(tpcCF);

  //actually MODIFIES existing clusters, does NOT create a new branch
  PndTpcClusterCorrectionTask* tpcCC = new PndTpcClusterCorrectionTask();
  double pars[6] = {-1.03313e-01,-2.17371e+00,1.30198e+01,
		    -2.82209e+01,2.93526e+01,-1.17837e+01};
  tpcCC->SetPersistence();
  tpcCC->SetParameters(pars);
  //fRun->AddTask(tpcCC);


  //PndTpcCTapplyTask* CTapply = new PndTpcCTapplyTask();
  //CTapply->SetPersistence();
  //fRun->AddTask(CTapply);

  /*
  PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  tpcSPR->SetTrkFinderParameters(1.1,// proxcut
                                 0.075, // proxcut on rieman sphere
                                 7.E-3, // planecut
                                 4.0, // szcut
                                 5); // minnumhits for fit
  tpcSPR->SetPersistence();
  tpcSPR->SetStoreHistograms(PROutFile);
  //tpcSPR->WriteHistograms(PROutFile);
  //fRun->AddTask(tpcSPR);
  */


  PndTpcSLPatternRecoTask* tpcSLPR = new PndTpcSLPatternRecoTask();
  tpcSLPR->SetPersistence(true);
  tpcSLPR->SetStoreHistograms(PROutFile);
  tpcSLPR->SetClusterAmpCut(30.);
  //tpcSLPR->SetCutTracksParallelZ(5);
  tpcSLPR->SetSortMode(2); //Y-sorting
  double parMins[4] = {-TMath::Pi(),0.,-TMath::Pi(),0.};
  double parMaxs[4] = {TMath::Pi(),10.,TMath::Pi(),20.};
  tpcSLPR->SetParameterSpace(parMins, parMaxs);
  tpcSLPR->SetDepth(7);
  tpcSLPR->SetThresh(20);
  tpcSLPR->SetMinCandHits(20);
  //tpcSLPR->SetClusterBranchName("PndTpcCluster_cut");
  tpcSLPR->SetAbsMomentum(1000);
  fRun->AddTask(tpcSLPR);


  //PndTpcTCtrackFit* tf = new PndTpcTCtrackFit();
  //tf->SetPersistence();
  //tf->SetDraw();
  //fRun->AddTask(tf);


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
