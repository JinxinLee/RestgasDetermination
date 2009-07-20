void runReco(TString base="mc", int nEvents = 1 )
{


  // ------------------------------------------------------------------------
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  
  // ========================================================================
  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 0; // just forget about it, for the moment
  
  // ----------------------------------------------------------------------
  // Set of input file directory
  
  // Input file (MC events)
  TString inFile = base+".digi.root";
    //TString inFile1 = base+".digi_1.root";
    //TString inFile2 = base+".digi_2.root";

  TString mcFile = base+".mc.root";

  //Parameter file
  TString parFile = base+".param.root";
  // Output file
  TString outFile = base+".reco.root";



  


  // ----  Load libraries   -------------------------------------------------
 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon(); 


  
  // -----   Reco run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  //fRun->AddFile(inFile1);
  //fRun->AddFile(inFile2);
  fRun->SetOutputFile(outFile);
  fRun->AddFriend(mcFile);

  FairGeane *Geane = new FairGeane(mcFile);


  // ------------------------------------------------------------------------
  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(parFile.Data());
	
        
  rtdb->setFirstInput(parInput1);
  fRun->LoadGeometry();
 
  Geane->SetField(fRun->GetField());

 
  // ------------------------------------------------------------------------
  // -----  Digitization part   --------------------------------------------








  //----- MVD track finder --------------------
  PndMvdIdealTrackFinderTask* mvdTrackFinder = new PndMvdIdealTrackFinderTask();
  mvdTrackFinder->SetVerbose(iVerbose);
  fRun->AddTask(mvdTrackFinder);


 

  // ------------------------------------------------- 
  PndDchFindTracks* dchfinderTask = new PndDchFindTracks("dchFindTracks");
  dchfinderTask->SetUseHitOrDigi("chit"); //hit, digi, chit(?)
  dchfinderTask->SetVerbose(0);
  fRun->AddTask(dchfinderTask);
  // ------------------------------------------------- 
  //This is only for chit!
  PndDchTrackFinderIdealCylHit* mcTrackFinder = new  PndDchTrackFinderIdealCylHit();
  mcTrackFinder->SetVerbose(0);//0  
  mcTrackFinder->SetPrimary(0);  // 1 = Only primary tracks are processed, 0 = all (default)
  dchfinderTask->UseFinder(mcTrackFinder);
  //--------------------------------------------------
  PndDchMatchTracks *matchTask = new PndDchMatchTracks();//match PndDchTracks and MCTracks
  matchTask->SetUseHitOrDigi("chit");
  matchTask->SetVerbose(1);
  fRun->AddTask(matchTask);	

  PndDchConvertTrackCandTask *dchConvertTask = 
    new PndDchConvertTrackCandTask();
  fRun->AddTask(dchConvertTask);	



 


  //------ Ideal Track finder --------------------
  //Create and add finder task
  /*
  PndGemFindTracks* gemfinderTask = new PndGemFindTracks("PndGemFindTracks");
  gemfinderTask->SetUseHitOrDigi("hit"); // hit = (default), digi
  fRun->AddTask(gemfinderTask);
 
  PndGemTrackFinderIdeal* gemmcTrackFinder = new  PndGemTrackFinderIdeal();
  gemmcTrackFinder->SetVerbose(0);  // verbosity level
  gemmcTrackFinder->SetPrimary(0);  // 1 = Only primary tracks are processed, 0 = all (default)
  gemfinderTask->UseFinder(gemmcTrackFinder);
  */
  PndGemIdealPatternRecoTask* gemfinderTask = new PndGemIdealPatternRecoTask();
  fRun->AddTask(gemfinderTask);


  GlobalTrackMergingTask* globTrackTask = 
    new GlobalTrackMergingTask();
  fRun->AddTask(globTrackTask);	

  KalmanTask *kalman = new KalmanTask();
  fRun->AddTask(kalman);

  // ------------------------------------------------- 
  // -----   Intialise and run   --------------------------------------------
  //fRun->SetStoreTraj(kFALSE);  
  fRun->Init();  
  //Geane->SetField(fRun->GetField());
  fRun->Run(0,nEvents);
  rtdb->saveOutput();
  rtdb->print();

  //DemoKalman->WriteHistograms();
  //DemoTools->WriteHistograms();


  // ------------------------------------------------------------------------
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
  
  
  
}
