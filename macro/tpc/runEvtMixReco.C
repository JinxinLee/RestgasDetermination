{

  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  
  TString PANDAMC=gSystem->Getenv("PANDAMC");

  // Input file (RAW events)
  TString inFile="TEST/evtmix13/DPM.mixed.root";
  TString jobname="reco2";

  TString mcFile="TEST/DPM.mc.root";
  
  inFile.ReplaceAll("$PANDAMC",PANDAMC);

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
  TString paramIn2 = mcFile;
  paramIn1.ReplaceAll(".mixed.root",".param.root");
  paramIn2.ReplaceAll(".mc.root",".param.root");

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
  
  std::cout<<"Input: "<<inFile<<std::endl;
  std::cout<<"Output: "<<outFile<<std::endl;
  std::cout<<"MCFile: "<<mcFile<<std::endl;
  std::cout<<"ParamIn1: "<<paramIn1<<std::endl;
  std::cout<<"ParamIn2: "<<paramIn2<<std::endl;
  std::cout<<"ParamOut: "<<paramOut<<std::endl;


  
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
  fRun->AddFriend(mcFile);
  fRun->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(paramIn1.Data());
  rtdb->setFirstInput(parInput1);

  FairParRootFileIo* parInput2 = new FairParRootFileIo();
  parInput2->open(paramIn2.Data());
  rtdb->setSecondInput(parInput2);

  
  rtdb->print();

  FairParRootFileIo* parOutput1 = new FairParRootFileIo(kTRUE);
  parOutput1->open(paramOut.Data());
  rtdb->setOutput(parOutput1);
  rtdb->saveOutput();

  //fRun->LoadGeometry();
  // ------------------------------------------------------------------------
  


  // -----    Reco Sequence  --------------------------------------------
  // PndTpcRiemannMCTask* tpcRMC = new PndTpcRiemannMCTask();
  // tpcRMC->SetBkgFileName("../data/DPM/test1.mc.root");
  // tpcRMC->SetPersistence();
  // fRun->AddTask(tpcRMC);
  
  PndTpcPatternRecoTask* tpcPR= new PndTpcPatternRecoTask();
  tpcPR->SetTrkFinderParameters(20.,    // riemannScale
		       1.5,   //proxcut, 
		       0.025, // riproxcut, 
                       2.5,   // szcut,
		       0.02,  // planecut,
		       2.5,   //  TTproxcut, 
		       4.0,   // TTplanecut, 
		       2.5,   //  TTszcut,
		       6,     // t minpointsforfit,
		       200);
    tpcPR->SetTrkFinderOptions(true, // dosorting
			       3, // sortingmode
			       false, // doClean,
			       true); // doMerge
    fRun->AddTask(tpcPR);

 // PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
 //    tpcSPR->SetSortingParameters(
 // 				 true, // false: sort only according to _sorting (see next argument); true: use internal sorting when adding hits to trackcands
 // 				 3,    // -1: no sorting, 0: sort Clusters by X, 1: Y, 2: Z, 3: R, 4: distance to origin
 // 				 30.); // z-position of interaction point (for sorting 4)
 //    tpcSPR->SetTrkFinderParameters(
 // 				   1.5,  // proximity cut in 3D
 // 				   0.025, // proximity cut on rieman sphere
 // 				   0.02, // distance to plane cut
 // 				   2.5,  // szcut
 // 				   8);   // minimum hits for plane & sz-fit
 //    tpcSPR->SetMergeTracks();
 //    tpcSPR->SetTrkMergerParameters(
 // 				   2.5,  // proximity cut
 // 				   2.5,  // sz cut
 // 				   4);// plane cut (RMS)
    
 //    tpcSPR->SetPersistence();
 //    tpcSPR->SetStoreHistograms("riemann.root");
 //    //    tpcSPR->useGeane();
 //    fRun->AddTask(tpcSPR);
  






  // PndTpcRiemannTrackingTask* tpcSPR = new PndTpcRiemannTrackingTask();
  // tpcSPR->SetTrkFinderParameters(2.,// proxcut
  // 			       0.02, // proxcut on rieman sphere
  // 			       2.E-3, // planecut
  // 			       4.0, // szcut
  // 			       4); // minnumhits for fit
  // tpcSPR->SetPersistence();
// fRun->AddTask(tpcSPR);

 //  KalmanTask* kalman =new KalmanTask();
//   kalman->SetPersistence();
// //fRun->AddTask(kalman);


  // TrackFitStatTask* fitstat=new TrackFitStatTask();
  // fitstat->SetPersistence();
  // fitstat->SetMCPCut(3); // in sigma dp/p
  // fitstat->SetMCCuts(0.05, // pmin
  // 	             10., // pmax
  // 		     -TMath::Pi(),   // thetamin 5deg
  // 		     TMath::Pi(),  // thetamax
  // 		     20); // nPndTpcPoints
//fitstat->SetPdgSelection(321);
//fitstat->DoResiduals();
//fRun->AddTask(fitstat);

  

 
 fRun->Init();
  
  fRun->Run(0,0);
  // -----   Finish   -------------------------------------------------------

  // tpcRMC->WriteHistograms();
//  tpcSPR->WriteHistograms("RecoHistos.root");
//kalman->WriteHistograms("RecoHistos.root");
//fitstat->WriteHistograms("RecoHistos.root");
  // dEdx->WriteHistograms("RecoHistos.root");

  //DebugLogger::Instance()->WriteFiles();
tpcPR->WriteHistograms(plotsfile);
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
