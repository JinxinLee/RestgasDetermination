{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  
  TString PANDAMC=gSystem->Getenv("PANDAMC");

  // Input file (Mixed tpc events)
  TString inFile="TEST/evtmix1000DD/physics.DD.mixed.root";
  // Input physics events with MVD and GEM
  TString physFile="TEST/physics.DD.raw.root"; 
  TString jobname="reco1";

  // TString mcFile="TEST/DPM.mc.root";
  
  inFile.ReplaceAll("$PANDAMC",PANDAMC);
  physFile.ReplaceAll("$PANDAMC",PANDAMC);


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
  outFile.ReplaceAll(".mixed.root",".reco.root");
  TString plotsfile = outFile;
  plotsfile.ReplaceAll("reco.root","plots.root");


  TString paramIn1 = inFile;
  TString paramIn2 = physFile;
  paramIn1.ReplaceAll(".mixed.root",".param.root");
  paramIn2.ReplaceAll(".raw.root",".param.root");

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
  
  std::cout<<"Input Mixed : "<<inFile<<std::endl;
  std::cout<<"Physics     : "<<physFile<<std::endl;
  std::cout<<"Output      : "<<outFile<<std::endl;
  std::cout<<"ParamIn1    : "<<paramIn1<<std::endl;
  std::cout<<"ParamIn2    : "<<paramIn2<<std::endl;
  std::cout<<"ParamOut    : "<<paramOut<<std::endl;


  
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
  //mcFile.ReplaceAll("$PANDAMC","/afs/e18/data/panda/MC");
  fRun->AddFriend(physFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();

  TString allDigiFile = "macro/params/all.par";
  FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
  parIo1->open(allDigiFile.Data(),"in");
  
  FairParRootFileIo* parInput2 = new FairParRootFileIo();
  parInput2->open(paramIn2.Data());

  rtdb->setFirstInput(parInput2);
  rtdb->setSecondInput(parIo1);

  PndGeoHandling* geoH = PndGeoHandling::Instance();

  rtdb->print();

  FairParRootFileIo* parOutput1 = new FairParRootFileIo(kTRUE);
  parOutput1->open(paramOut.Data());
  rtdb->setOutput(parOutput1);
  rtdb->saveOutput();

  //fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  
  FairGeane *Geane = new FairGeane();
  fRun->AddTask(Geane);
  std::cout<<"\nGEANE initialised"<<std::endl;

  // -----    Reco Sequence  --------------------------------------------
   
  //find track candidates in the TPC alone
  PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  tpcSPR->SetPersistence();
  //tpcSPR->useGeane(); // use RKTrackrep and GeaneTrackrep
  //tpcSPR->SetMCPid(); // use ideal particle identification
  tpcSPR->SetPDG(211);
  fRun->AddTask(tpcSPR);

  PndTpcEvtDeconvTask* evtDeconv=new PndTpcEvtDeconvTask();
  evtDeconv->SetPersistence();
  evtDeconv->SetCuts(15,5);
  evtDeconv->SetOutTrackBranchName("RiemannTrackTagged");
  fRun->AddTask(evtDeconv);


  PndTpcTrackInitTask* trackInit=new PndTpcTrackInitTask();
  trackInit->SetPersistence();
  trackInit->SetVerbose(false);
  trackInit->SetRiemannBranchName("RiemannTrackTagged");
  trackInit->SetOutBranchNames("TrackPreFitTagged",
			       "PndTrackCandTagged",
			       "PndTrackTpcTagged");
  trackInit->useGeane(true); // uses RKTrackrep and GeaneTrackrep
  trackInit->SetSmoothing(true);
  fRun->AddTask(trackInit);



  KalmanTask* kalman =new KalmanTask();
  kalman->SetPersistence();
  kalman->SetTrackBranchName("TrackPreFitTagged");
  kalman->SetOutBranchName("TrackFitTagged");
  kalman->SetNumIterations(1); // number of fitting iterations (back and forth)
  fRun->AddTask(kalman);       // creates TrackPostFit branch

  //correlate fitted track with MVD pixels and strips
  PndTpcMVDCorrelatorTask* corr = new PndTpcMVDCorrelatorTask();
  corr->SetMatchDistance(0.18);   //cm
  corr->SetMinMVDHits(2);
  corr->RequireMatch(true);
  corr->SetTrackBranchName("TrackFitTagged");
  corr->SetOutTrackBranchName("TrackPreFitTaggedMVD");
  corr->SetPersistence(true);
  fRun->AddTask(corr);

  
  //fit after MVD corr
  KalmanTask* kalman2 =new KalmanTask();
  kalman2->SetPersistence();
  kalman2->SetNumIterations(1); // number of fitting iterations (back and forth)
  kalman2->SetTrackBranchName("TrackPreFitTaggedMVD");
  kalman2->SetOutBranchName("TrackFitTaggedMVD");
  fRun->AddTask(kalman2);

  PndTpcGEMCorrelatorTask* corrG = new PndTpcGEMCorrelatorTask();
  corrG->SetMatchDistance(0.5);   //cm 
  corrG->SetMinGEMHits(2);
  //corrG->RequireMatch(true);
  corrG->SetTrackBranchName("TrackFitTaggedMVD");
  corrG->SetOutTrackBranchName("TrackPreFitTaggedGEM");
  corrG->SetPersistence(true);
  fRun->AddTask(corrG);

  //final fit
  KalmanTask* kalman3 =new KalmanTask();
  kalman3->SetPersistence();
  kalman3->SetNumIterations(1); // number of fitting iterations (back and forth)
  kalman3->SetTrackBranchName("TrackPreFitTaggedGEM");
  kalman3->SetOutBranchName("TrackPostFitComplete");
  fRun->AddTask(kalman3);





  fRun->Init();
  
  fRun->Run(0,1);
  // -----   Finish   -------------------------------------------------------

  // tpcRMC->WriteHistograms();
//  tpcSPR->WriteHistograms("RecoHistos.root");
//kalman->WriteHistograms("RecoHistos.root");
//fitstat->WriteHistograms("RecoHistos.root");
  // dEdx->WriteHistograms("RecoHistos.root");

  //DebugLogger::Instance()->WriteFiles();

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
