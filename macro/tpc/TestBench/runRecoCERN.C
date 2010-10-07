{
//Data analysis framework for the test bench tpc data.
//Maxence Vandenbroucke 11/01/2010

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

  TString jobdir = "TEST";
  TString jobname="CERN1"; 

  TString digiDir=(basedir+"/")+jobdir;
  TString inFile=(digiDir+"/")+jobname;
  inFile+=".raw.root";
 
  TString mcFile=inFile;
  mcFile.ReplaceAll("raw","mc");

  TString outFile = inFile; 
  outFile.ReplaceAll(".raw.root",".reco.root");


  TString paramIn = inFile;
  paramIn.ReplaceAll(".raw.root",".param.root");
  TString paramOut = outFile;
  paramOut.ReplaceAll(".reco.root",".param.root");
  
  std::cout<<"Input: "<<inFile<<std::endl;
  std::cout<<"Output: "<<outFile<<std::endl;  
  std::cout<<"MCFile: "<<mcFile<<std::endl;

  std::cout<<"ParamIn: "<<paramIn<<std::endl;
  std::cout<<"ParamOut: "<<paramOut<<std::endl;



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
  read->SetDatafile("run-2064.root");
  read->SetClusterBranchName("PndTpcSample");
  //read->SetCutSmallPad();
  //read->SetMinSamples(1000);
  fRun->AddTask(read);

  //PndTpcTCcrossTalkTask* CT = new PndTpcTCcrossTalkTask();
  //CT->SetPersistence();
  //fRun->AddTask(CT);

  
  PndTpcPSATask* tpsa = new  PndTpcPSATask();
  tpsa->SetPersistence();
  tpsa->SetSampleBranchName("PndTpcSample");
  fRun->AddTask(tpsa);

  PndTpcClusterFinderTask* tpcCF = new PndTpcClusterFinderTask();
  //tpcCF->SetDataMode(true); //prevents usage of FairLinks
  tpcCF->SetMode(1); // individual timeslice
  tpcCF->SetDataMode(true);
  tpcCF->SetPersistence();
  tpcCF->SetDigiBranchName("PndTpcDigi");
  tpcCF->timeslice(10); // = 4 sample times = 100ns @ 40MHz
  //tpcCF->SetTrivialClustering();
  fRun->AddTask(tpcCF);


  //PndTpcCTapplyTask* CTapply = new PndTpcCTapplyTask();
  //CTapply->SetPersistence();
  //fRun->AddTask(CTapply);
  
    
  //PndTpcIdealTrackingTask* tpcIPR = new PndTpcIdealTrackingTask();
  //tpcIPR->useGeane(true);
  //tpcIPR->useDistSorting(true);
  //tpcIPR->SetPersistence();
  //tpcIPR->SetSecondarySuppression(true);
  //tpcIPR->SetMinHits(5);
  //fRun->AddTask(tpcIPR);

  PndTpcSLPatternRecoTask* tpcSLPR = new PndTpcSLPatternRecoTask();
  tpcSLPR->SetStoreHistograms("moppel.root");
  tpcSLPR->SetClusterAmpCut(100.);
  //tpcSLPR->SetProjectionXY();
  tpcSLPR->SetXSorting(true);
  double parMins[4] = {0.,0.,0.,0.};
  double parMaxs[4] = {TMath::Pi(),10.,TMath::Pi(),10.};
  tpcSLPR->SetParameterSpace(parMins, parMaxs);
  tpcSLPR->SetDepth(7);
  tpcSLPR->SetThresh(12);
  tpcSLPR->SetMinCandHits(12);
  //tpcSLPR->SetClusterBranchName("PndTpcCluster_cut");
  fRun->AddTask(tpcSLPR);


//  PndTpcTCtrackFit* tf = new PndTpcTCtrackFit();
//  tf->SetPersistence();
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
  SLres->SetSecondarySuppression(true);
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
